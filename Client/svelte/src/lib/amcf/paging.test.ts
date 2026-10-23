// @ts-nocheck -- run by `node --test` (npm test); the project has no @types/node.
import { test } from 'node:test';
import assert from 'node:assert/strict';

import { paginate } from './paging.ts';

const executions = Array.from({ length: 10000 }, (_, i) => ({ executionUUID: `exec-${i}` }));

test('a long list shows only the first page of entries', () => {
	const p = paginate(executions, 1, 20);

	assert.equal(p.rows.length, 20);
	assert.equal(p.rows[0].executionUUID, 'exec-0');
	assert.equal(p.rows[19].executionUUID, 'exec-19');
	assert.equal(p.page, 1);
	assert.equal(p.pageCount, 500);
	assert.equal(p.first, 1);
	assert.equal(p.last, 20);
	assert.equal(p.total, 10000);
});

test('the last page holds the remaining entries', () => {
	const p = paginate(executions.slice(0, 43), 3, 20);

	assert.deepEqual(p.rows.map((r) => r.executionUUID), ['exec-40', 'exec-41', 'exec-42']);
	assert.equal(p.pageCount, 3);
	assert.equal(p.first, 41);
	assert.equal(p.last, 43);
});

test('a page past the end shows the last page, e.g. after entries were deleted', () => {
	const p = paginate(executions.slice(0, 25), 3, 20);

	assert.equal(p.page, 2);
	assert.deepEqual(p.rows.map((r) => r.executionUUID), ['exec-20', 'exec-21', 'exec-22', 'exec-23', 'exec-24']);
	assert.equal(p.first, 21);
	assert.equal(p.last, 25);
});

test('an empty list is a single empty page', () => {
	const p = paginate([], 1, 20);

	assert.deepEqual(p.rows, []);
	assert.equal(p.page, 1);
	assert.equal(p.pageCount, 1);
	assert.equal(p.first, 0);
	assert.equal(p.last, 0);
	assert.equal(p.total, 0);
});
