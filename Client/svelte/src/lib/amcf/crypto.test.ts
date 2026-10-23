// @ts-nocheck -- run by `node --test` (npm test); the project has no @types/node.
import { test } from 'node:test';
import assert from 'node:assert/strict';

import {
	sha256Hex,
	sha256HexFallback
} from '../../../../core/common/AMCCrypto.js';

test('the SHA-256 fallback matches standard string test vectors', () => {
	assert.equal(
		sha256HexFallback(''),
		'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'
	);
	assert.equal(
		sha256HexFallback('abc'),
		'ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad'
	);
	assert.equal(
		sha256HexFallback('The quick brown fox jumps over the lazy dog'),
		'd7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592'
	);
});

test('the SHA-256 fallback hashes binary data', () => {
	assert.equal(
		sha256HexFallback(new Uint8Array([0, 1, 2, 253, 254, 255])),
		'3f2d1552cdc7483f40dd720c80b900225dfecfd5cae7cd168d79ab6ee5959885'
	);
});

test('the preferred SHA-256 helper produces the same result', async () => {
	assert.equal(
		await sha256Hex('abc'),
		sha256HexFallback('abc')
	);
});
