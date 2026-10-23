/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
 * Neither the name of the Autodesk Inc. nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

const SHA256_INITIAL_HASH = new Uint32Array([
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
]);

const SHA256_ROUND_CONSTANTS = new Uint32Array([
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
]);

function rotateRight(value, shift)
{
	return (value >>> shift) | (value << (32 - shift));
}

function normalizeBytes(value)
{
	if (typeof value === "string")
		return new TextEncoder().encode(value);
	if (value instanceof Uint8Array)
		return value;
	if (value instanceof ArrayBuffer)
		return new Uint8Array(value);
	if (ArrayBuffer.isView(value))
		return new Uint8Array(value.buffer, value.byteOffset, value.byteLength);

	throw new TypeError("SHA-256 input must be a string, ArrayBuffer, or typed array.");
}

function bytesToHex(bytes)
{
	return Array.from(bytes)
		.map(byte => byte.toString(16).padStart(2, "0"))
		.join("");
}

// Pure JavaScript fallback for browsers where Web Crypto is unavailable, most commonly when
// the AMCF client is served over plain HTTP from a non-localhost machine address.
export function sha256BytesFallback(value)
{
	const bytes = normalizeBytes(value);
	const paddedLength = Math.ceil((bytes.length + 9) / 64) * 64;
	const padded = new Uint8Array(paddedLength);
	padded.set(bytes);
	padded[bytes.length] = 0x80;

	const paddedView = new DataView(padded.buffer);
	const bitLengthHigh = Math.floor(bytes.length / 0x20000000);
	const bitLengthLow = (bytes.length << 3) >>> 0;
	paddedView.setUint32(paddedLength - 8, bitLengthHigh, false);
	paddedView.setUint32(paddedLength - 4, bitLengthLow, false);

	const hash = new Uint32Array(SHA256_INITIAL_HASH);
	const words = new Uint32Array(64);

	for (let blockOffset = 0; blockOffset < paddedLength; blockOffset += 64) {
		for (let index = 0; index < 16; index++)
			words[index] = paddedView.getUint32(blockOffset + index * 4, false);

		for (let index = 16; index < 64; index++) {
			const word15 = words[index - 15];
			const word2 = words[index - 2];
			const sigma0 = rotateRight(word15, 7) ^ rotateRight(word15, 18) ^ (word15 >>> 3);
			const sigma1 = rotateRight(word2, 17) ^ rotateRight(word2, 19) ^ (word2 >>> 10);
			words[index] = (words[index - 16] + sigma0 + words[index - 7] + sigma1) >>> 0;
		}

		let a = hash[0];
		let b = hash[1];
		let c = hash[2];
		let d = hash[3];
		let e = hash[4];
		let f = hash[5];
		let g = hash[6];
		let h = hash[7];

		for (let index = 0; index < 64; index++) {
			const sum1 = rotateRight(e, 6) ^ rotateRight(e, 11) ^ rotateRight(e, 25);
			const choose = (e & f) ^ (~e & g);
			const temporary1 = (h + sum1 + choose + SHA256_ROUND_CONSTANTS[index] + words[index]) >>> 0;
			const sum0 = rotateRight(a, 2) ^ rotateRight(a, 13) ^ rotateRight(a, 22);
			const majority = (a & b) ^ (a & c) ^ (b & c);
			const temporary2 = (sum0 + majority) >>> 0;

			h = g;
			g = f;
			f = e;
			e = (d + temporary1) >>> 0;
			d = c;
			c = b;
			b = a;
			a = (temporary1 + temporary2) >>> 0;
		}

		hash[0] = (hash[0] + a) >>> 0;
		hash[1] = (hash[1] + b) >>> 0;
		hash[2] = (hash[2] + c) >>> 0;
		hash[3] = (hash[3] + d) >>> 0;
		hash[4] = (hash[4] + e) >>> 0;
		hash[5] = (hash[5] + f) >>> 0;
		hash[6] = (hash[6] + g) >>> 0;
		hash[7] = (hash[7] + h) >>> 0;
	}

	const result = new Uint8Array(32);
	const resultView = new DataView(result.buffer);
	for (let index = 0; index < hash.length; index++)
		resultView.setUint32(index * 4, hash[index], false);

	return result;
}

export function sha256HexFallback(value)
{
	return bytesToHex(sha256BytesFallback(value));
}

export async function sha256Hex(value)
{
	const bytes = normalizeBytes(value);
	const subtleCrypto = globalThis.crypto?.subtle;
	if (subtleCrypto && typeof subtleCrypto.digest === "function") {
		try {
			const hashBuffer = await subtleCrypto.digest("SHA-256", bytes);
			return bytesToHex(new Uint8Array(hashBuffer));
		}
		catch (error) {
			console.warn("Web Crypto SHA-256 failed; using the local fallback.", error);
		}
	}

	return sha256HexFallback(bytes);
}
