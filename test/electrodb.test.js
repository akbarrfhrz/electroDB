import assert from 'node:assert/strict';
import test from 'node:test';
import {
  ARTICLES,
  DEFAULT_COMPONENTS,
  appendSeedExpansion,
  calculate,
  computeStats,
  exportCsv,
  filterComponents,
  normalizeComponent,
} from '../lib/electrodb.js';

test('default catalog includes the expanded seed list and its full specifications', () => {
  assert.equal(DEFAULT_COMPONENTS.length, 80);
  assert.equal(DEFAULT_COMPONENTS[0].code, 'RES-10K-025');
  assert.equal(DEFAULT_COMPONENTS[0].pin_count, 2);
  assert.equal(DEFAULT_COMPONENTS[79].code, 'MOD-RELAY-5V');
});

test('seed migration preserves existing records and avoids duplicate codes and IDs', () => {
  const customizedSeed = {
    ...DEFAULT_COMPONENTS[10],
    id: 50,
    name: 'Customized module',
  };
  const legacyCatalog = [...DEFAULT_COMPONENTS.slice(0, 10), customizedSeed];
  const migrated = appendSeedExpansion(legacyCatalog, 11);

  assert.equal(migrated.components.length, 80);
  assert.equal(migrated.components.find((component) => component.id === 50).name, 'Customized module');
  assert.equal(
    migrated.components.filter((component) => component.code === customizedSeed.code).length,
    1,
  );
  assert.equal(new Set(migrated.components.map((component) => component.id)).size, 80);
  assert.ok(migrated.nextId > Math.max(...migrated.components.map((component) => component.id)));
});

test('component normalization requires identifiers and fills optional specifications', () => {
  assert.match(normalizeComponent({ code: '', name: 'Part' }).error, /wajib diisi/);

  const { component } = normalizeComponent({ code: 'NEW-1', name: 'New part' });
  assert.equal(component.category, 'Sensor & Modul');
  assert.equal(component.nominal_value, '-');
  assert.equal(component.pin_count, 0);
});

test('catalog filtering and stats use the same category and search semantics', () => {
  const matches = filterComponents(DEFAULT_COMPONENTS, 'esp32', 'Sensor & Modul');
  assert.equal(matches.length, 1);
  assert.equal(matches[0].id, 10);
  assert.equal(computeStats(DEFAULT_COMPONENTS).total_components, 80);
  assert.equal(computeStats(DEFAULT_COMPONENTS).total_datasheets, 10);
});

test('calculator supports E24 recommendations and returns validation errors', () => {
  const led = calculate({ mode: 'LED_RES', v: 5, vf: 2, if_ma: 15 });
  assert.equal(led.valid, true);
  assert.equal(led.primary_value, 200);
  assert.match(led.recommendation, /200 Ohm/);

  assert.match(calculate({ mode: 'LED_RES', v: 2, vf: 2, if_ma: 15 }).error, /harus lebih besar/);
});

test('knowledge and CSV export keep their expected response shapes', () => {
  assert.equal(ARTICLES.length, 6);
  assert.equal(ARTICLES[0].id, 1);

  const csv = exportCsv([{ ...DEFAULT_COMPONENTS[0], name: 'Part, "A"' }]);
  assert.match(csv, /"Part, ""A"""/);
  assert.match(csv, /^ID,Kode,Nama,Kategori/m);
});
