// Imports Client/core modules for tests. The loader hooks must be registered before any core module
// is linked, so tests import core code through importCore (a dynamic import) rather than statically.

import { register } from "node:module";

register ("./coreLoaderHooks.mjs", import.meta.url);

export async function importCore (pathInCore) {
	const module = await import (new URL ("../../core/" + pathInCore, import.meta.url).href);
	return module.default;
}
