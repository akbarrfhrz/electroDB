import assert from 'node:assert/strict';
import test from 'node:test';
import {
  ARTICLES,
  DEFAULT_COMPONENTS,
  calculate,
  computeStats,
  exportCsv,
  filterComponents,
  normalizeComponent,
} from '../lib/electrodb.js';

test('default catalog retains its 10 seeded components and their full specifications', () => {
  assert.equal(DEFAULT_COMPONENTS.length, 10);
  assert.equal(DEFAULT_COMPONENTS[0].code, 'RES-10K-025');
  assert.equal(DEFAULT_COMPONENTS[0].pin_count, 2);
  assert.equal(DEFAULT_COMPONENTS[9].code, 'MCU-ESP32-WROOM');
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
  assert.equal(computeStats(DEFAULT_COMPONENTS).total_components, 10);
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
