// Module loader hooks for running Client/core under node:test without installing the client's
// npm packages. Registered by coreLoader.mjs.
//
// - Client/core is plain ES module code in .js files without a package.json "type", so its files
//   are loaded as ES modules explicitly (no reliance on Node's syntax detection).
// - The few third-party packages Client/core imports (axios, luxon, three) are replaced by stubs.
//   The tests never reach the network or render anything; requests are stubbed on the application.

import { readFile } from "node:fs/promises";

const STUB_PREFIX = "amc-test-stub:";

const STUBS = {
	"axios": "export default function axios () { throw new Error ('network access is stubbed out in tests'); }",
	"luxon": "export const DateTime = {};",
	"three": "export default {};",
	"three/examples/jsm/loaders/SVGLoader": "export class SVGLoader {}",
	"three/addons/controls/OrbitControls.js": "export class OrbitControls {}",
};

const CORE_URL_PART = "/Client/core/";

export async function resolve (specifier, context, nextResolve) {
	if (Object.prototype.hasOwnProperty.call (STUBS, specifier))
		return { url: STUB_PREFIX + specifier, shortCircuit: true };
	return nextResolve (specifier, context);
}

export async function load (url, context, nextLoad) {
	if (url.startsWith (STUB_PREFIX))
		return { format: "module", source: STUBS[url.substring (STUB_PREFIX.length)], shortCircuit: true };

	if (url.startsWith ("file:") && url.includes (CORE_URL_PART) && url.endsWith (".js"))
		return { format: "module", source: await readFile (new URL (url), "utf8"), shortCircuit: true };

	return nextLoad (url, context);
}
