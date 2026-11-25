import { timingSafeEqual } from 'node:crypto';
import { neon } from '@neondatabase/serverless';
import {
  ARTICLES,
  DEFAULT_COMPONENTS,
  calculate,
  computeStats,
  exportCsv,
  filterComponents,
  normalizeComponent,
} from '../lib/electrodb.js';

let sql;
let schemaPromise;

function jsonResponse(body, status = 200, headers = {}) {
  return new Response(JSON.stringify(body), {
    status,
    headers: {
      'Content-Type': 'application/json; charset=utf-8',
      'Cache-Control': 'no-store',
      ...headers,
    },
  });
}

function getDatabase() {
  if (!process.env.DATABASE_URL) {
    const error = new Error('DATABASE_URL belum diatur pada environment Vercel.');
    error.status = 503;
    throw error;
  }
  if (!sql) sql = neon(process.env.DATABASE_URL);
  return sql;
}

async function ensureDatabase() {
  const client = getDatabase();
  if (!schemaPromise) {
    schemaPromise = (async () => {
      await client`
        CREATE TABLE IF NOT EXISTS electrodb_state (
          id smallint PRIMARY KEY CHECK (id = 1),
          version bigint NOT NULL,
          next_id integer NOT NULL,
          components jsonb NOT NULL
        )
      `;
      await client`
        INSERT INTO electrodb_state (id, version, next_id, components)
        VALUES (1, 0, 11, ${JSON.stringify(DEFAULT_COMPONENTS)}::jsonb)
        ON CONFLICT (id) DO NOTHING
      `;
    })();
  }

  try {
    await schemaPromise;
  } catch (error) {
    schemaPromise = undefined;
    throw error;
  }
}

async function loadState() {
  await ensureDatabase();
  const rows = await getDatabase()`
    SELECT version, next_id, components
    FROM electrodb_state
    WHERE id = 1
  `;
  if (!rows.length) throw new Error('Baris katalog ElectroDB tidak ditemukan setelah inisialisasi.');
  const row = rows[0];
  return {
    version: Number(row.version),
    nextId: Number(row.next_id),
    components: typeof row.components === 'string' ? JSON.parse(row.components) : row.components,
  };
}

async function updateState(expectedVersion, nextId, components) {
  const rows = await getDatabase()`
    UPDATE electrodb_state
    SET version = version + 1,
        next_id = ${nextId},
        components = ${JSON.stringify(components)}::jsonb
    WHERE id = 1 AND version = ${expectedVersion}
    RETURNING version
  `;
  return rows.length > 0;
}

async function mutateState(operation) {
  for (let attempt = 0; attempt < 5; attempt += 1) {
    const current = await loadState();
    const draft = {
      nextId: current.nextId,
      components: current.components.map((component) => ({ ...component })),
    };
    const result = operation(draft);
    if (!result.changed) return result;
    if (await updateState(current.version, draft.nextId, draft.components)) return result;
  }
  return {
    status: 409,
    body: { error: 'Katalog sedang diperbarui oleh pengguna lain. Silakan coba lagi.' },
    changed: false,
  };
}

function adminAuthorization(request) {
  const expected = process.env.ELECTRODB_ADMIN_TOKEN;
  if (!expected || expected.length < 32) {
    return jsonResponse({
      error: 'ELECTRODB_ADMIN_TOKEN belum dikonfigurasi dengan aman di Vercel.',
    }, 503);
  }

  const supplied = request.headers.get('authorization') || '';
  const match = /^Bearer (.+)$/.exec(supplied);
  if (!match) {
    return jsonResponse({ error: 'Aksi ini memerlukan kunci admin.' }, 401);
  }

  const actualBytes = Buffer.from(match[1]);
  const expectedBytes = Buffer.from(expected);
  if (actualBytes.length !== expectedBytes.length ||
      !timingSafeEqual(actualBytes, expectedBytes)) {
    return jsonResponse({ error: 'Kunci admin tidak valid.' }, 401);
  }
  return null;
}

async function readJson(request) {
  try {
    return { value: await request.json() };
  } catch {
    return { error: jsonResponse({ error: 'Body permintaan harus berupa JSON yang valid.' }, 400) };
  }
}

function invalidMutation(message, status = 400) {
  return { changed: false, status, body: { error: message } };
}

function addComponent(draft, input) {
  if (draft.components.length >= 500) {
    return invalidMutation('Kapasitas database penuh (maksimum 500 komponen).');
  }
  const normalized = normalizeComponent(input);
  if (normalized.error) return invalidMutation(normalized.error);
  const duplicate = draft.components.some((component) =>
    component.code.toLocaleLowerCase() === normalized.component.code.toLocaleLowerCase());
  if (duplicate) return invalidMutation('Kode komponen tersebut sudah ada di katalog referensi.');

  const created = { id: draft.nextId++, ...normalized.component };
  draft.components.push(created);
  return {
    changed: true,
    status: 201,
    body: { message: 'Referensi komponen berhasil ditambahkan', item: created },
  };
}

function editComponent(draft, id, input) {
  const index = draft.components.findIndex((component) => component.id === id);
  if (index < 0) return invalidMutation('Referensi komponen dengan ID tersebut tidak ditemukan.', 404);
  const normalized = normalizeComponent(input);
  if (normalized.error) return invalidMutation(normalized.error);
  const duplicate = draft.components.some((component, componentIndex) =>
    componentIndex !== index &&
    component.code.toLocaleLowerCase() === normalized.component.code.toLocaleLowerCase());
  if (duplicate) return invalidMutation('Kode komponen bentrok dengan referensi lain.');

  const updated = { id, ...normalized.component };
  draft.components[index] = updated;
  return {
    changed: true,
    status: 200,
    body: { message: 'Referensi komponen diperbarui', item: updated },
  };
}

function deleteComponent(draft, id) {
  const index = draft.components.findIndex((component) => component.id === id);
  if (index < 0) return invalidMutation('Referensi komponen tidak ditemukan.', 404);
  draft.components.splice(index, 1);
  return {
    changed: true,
    status: 200,
    body: { message: 'Referensi komponen berhasil dihapus' },
  };
}

function parseId(value) {
  if (!/^[1-9]\d*$/.test(value)) return null;
  const id = Number(value);
  return Number.isSafeInteger(id) ? id : null;
}

async function handle(request) {
  const url = new URL(request.url);
  const route = (url.searchParams.get('__route') ||
    url.pathname.replace(/^\/api\/?/, '')).replace(/^\/+|\/+$/g, '');
  const method = request.method.toUpperCase();

  if (method === 'OPTIONS') return new Response(null, { status: 204 });

  if (route === 'components' && method === 'GET') {
    const state = await loadState();
    const items = filterComponents(
      state.components,
      url.searchParams.get('q') || '',
      url.searchParams.get('category') || '',
    );
    return jsonResponse({ count: items.length, items });
  }

  if (route === 'components' && method === 'POST') {
    const denied = adminAuthorization(request);
    if (denied) return denied;
    const parsed = await readJson(request);
    if (parsed.error) return parsed.error;
    const result = await mutateState((draft) => addComponent(draft, parsed.value));
    return jsonResponse(result.body, result.status);
  }

  const componentRoute = /^components\/([^/]+)$/.exec(route);
  if (componentRoute) {
    const id = parseId(componentRoute[1]);
    if (id === null) return jsonResponse({ error: 'ID komponen tidak valid.' }, 400);

    if (method === 'GET') {
      const state = await loadState();
      const item = state.components.find((component) => component.id === id);
      return item
        ? jsonResponse(item)
        : jsonResponse({ error: 'Referensi tidak ditemukan' }, 404);
    }

    if (method === 'PUT') {
      const denied = adminAuthorization(request);
      if (denied) return denied;
      const parsed = await readJson(request);
      if (parsed.error) return parsed.error;
      const result = await mutateState((draft) => editComponent(draft, id, parsed.value));
      return jsonResponse(result.body, result.status);
    }

    if (method === 'DELETE') {
      const denied = adminAuthorization(request);
      if (denied) return denied;
      const result = await mutateState((draft) => deleteComponent(draft, id));
      return jsonResponse(result.body, result.status);
    }
    return jsonResponse({ error: 'Metode HTTP tidak didukung.' }, 405, { Allow: 'GET, PUT, DELETE' });
  }

  if (route === 'stats' && method === 'GET') {
    const state = await loadState();
    return jsonResponse(computeStats(state.components));
  }

  if (route === 'calculator' && method === 'POST') {
    const parsed = await readJson(request);
    if (parsed.error) return parsed.error;
    const result = calculate(parsed.value);
    return jsonResponse(result, result.valid ? 200 : 400);
  }

  if (route === 'knowledge' && method === 'GET') {
    return jsonResponse({ count: ARTICLES.length, articles: ARTICLES });
  }

  if (route === 'reset' && method === 'POST') {
    const denied = adminAuthorization(request);
    if (denied) return denied;
    const result = await mutateState((draft) => {
      draft.components = DEFAULT_COMPONENTS.map((component) => ({ ...component }));
      draft.nextId = draft.components.length + 1;
      return {
        changed: true,
        status: 200,
        body: { message: 'Database referensi dikembalikan ke 10 komponen bawaan' },
      };
    });
    return jsonResponse(result.body, result.status);
  }

  if (route === 'export/csv' && method === 'GET') {
    const state = await loadState();
    return new Response(exportCsv(state.components), {
      status: 200,
      headers: {
        'Content-Type': 'text/csv; charset=utf-8',
        'Content-Disposition': 'attachment; filename="electrodb_referensi.csv"',
        'Cache-Control': 'no-store',
      },
    });
  }

  if (['components', 'stats', 'calculator', 'knowledge', 'reset', 'export/csv'].includes(route)) {
    const allowed = route === 'components' ? 'GET, POST'
      : route === 'calculator' || route === 'reset' ? 'POST'
        : 'GET';
    return jsonResponse({ error: 'Metode HTTP tidak didukung.' }, 405, { Allow: allowed });
  }
  return jsonResponse({ error: 'Endpoint tidak ditemukan.' }, 404);
}

export default {
  async fetch(request) {
    try {
      return await handle(request);
    } catch (error) {
      console.error('ElectroDB API request failed:', error);
      return jsonResponse({
        error: error.status === 503
          ? error.message
          : 'Gagal memproses permintaan. Periksa log Vercel untuk detail.',
      }, error.status || 500);
    }
  },
};
