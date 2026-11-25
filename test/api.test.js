import assert from 'node:assert/strict';
import test from 'node:test';
import handler from '../api/index.js';

test('serverless API serves knowledge and calculator without a database connection', async () => {
  const knowledge = await handler.fetch(new Request('https://electrodb.test/api?__route=knowledge'));
  const knowledgeBody = await knowledge.json();
  assert.equal(knowledge.status, 200);
  assert.equal(knowledgeBody.count, 6);

  const calculation = await handler.fetch(new Request('https://electrodb.test/api?__route=calculator', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ mode: 'LED_RES', v: 5, vf: 2, if_ma: 15 }),
  }));
  const calculationBody = await calculation.json();
  assert.equal(calculation.status, 200);
  assert.equal(calculationBody.valid, true);
  assert.equal(calculationBody.primary_value, 200);
});

test('catalog mutations require an admin token before accessing storage', async () => {
  const previousToken = process.env.ELECTRODB_ADMIN_TOKEN;
  process.env.ELECTRODB_ADMIN_TOKEN = 'a'.repeat(32);
  try {
    const response = await handler.fetch(new Request('https://electrodb.test/api?__route=components', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ code: 'NEW-1', name: 'New component' }),
    }));
    assert.equal(response.status, 401);
    assert.match((await response.json()).error, /kunci admin/i);
  } finally {
    if (previousToken === undefined) delete process.env.ELECTRODB_ADMIN_TOKEN;
    else process.env.ELECTRODB_ADMIN_TOKEN = previousToken;
  }
});
