/*++

Copyright (C) 2026 Autodesk Inc.

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

/**
 * Loads a JSON theme file and injects token overrides as CSS custom properties
 * on the document root element.
 *
 * Theme JSON format:
 * {
 *   "name": "My Theme",
 *   "tokens": {
 *     "--amcf-color-primary": "#1a73e8",
 *     "--amcf-color-bg": "#f0f2f5"
 *   }
 * }
 *
 * Only tokens present in the JSON are overridden; all others retain their
 * default values from tokens.css.
 */

const THEME_STORAGE_KEY = "amcf-theme-overrides";

/**
 * Apply a set of token overrides to the document root.
 * @param {Object} tokens  key-value pairs of CSS custom property names to values
 */
export function applyTokens (tokens)
{
	if (!tokens || typeof tokens !== "object")
		return;

	const root = document.documentElement;
	for (const [prop, value] of Object.entries (tokens)) {
		if (typeof prop === "string" && prop.startsWith ("--amcf-"))
			root.style.setProperty (prop, value);
	}
}

/**
 * Load a theme from a URL (typically `/api/theme`) and apply its tokens.
 * Falls back silently to the default tokens.css values on any error.
 *
 * @param {string} url  The URL to fetch the theme JSON from
 * @returns {Promise<Object|null>}  The parsed theme object, or null on failure
 */
export async function loadThemeFromURL (url)
{
	try {
		const resp = await fetch (url);
		if (!resp.ok)
			return null;

		const theme = await resp.json ();
		if (theme && theme.tokens)
			applyTokens (theme.tokens);

		return theme;

	} catch (e) {
		console.warn ("[AMCF Theme] Could not load theme from", url, e);
		return null;
	}
}

/**
 * Load a theme from a JSON object and apply its tokens.
 * Useful for embedding a theme directly in the application configuration.
 *
 * @param {Object} themeJSON  The theme object with a "tokens" property
 */
export function loadThemeFromJSON (themeJSON)
{
	if (themeJSON && themeJSON.tokens)
		applyTokens (themeJSON.tokens);
}

/**
 * Toggle the high-contrast accessibility mode.
 * Adds or removes the `amcf-high-contrast` class on the root element and
 * persists the preference in localStorage.
 *
 * @param {boolean} [enable]  Force on/off; omit to toggle
 * @returns {boolean}  The new high-contrast state
 */
export function toggleHighContrast (enable)
{
	const root = document.documentElement;
	const isOn = root.classList.contains ("amcf-high-contrast");
	const next = (enable !== undefined) ? enable : !isOn;

	if (next) {
		root.classList.add ("amcf-high-contrast");
		try { localStorage.setItem ("amcf-high-contrast", "1"); } catch (_) { /* noop */ }
	} else {
		root.classList.remove ("amcf-high-contrast");
		try { localStorage.removeItem ("amcf-high-contrast"); } catch (_) { /* noop */ }
	}

	return next;
}

/**
 * Restore high-contrast preference from localStorage on startup.
 */
export function restoreHighContrastPreference ()
{
	try {
		if (localStorage.getItem ("amcf-high-contrast") === "1")
			document.documentElement.classList.add ("amcf-high-contrast");
	} catch (_) {
		/* localStorage unavailable */
	}
}

/**
 * Toggle dark mode on or off.
 * Adds or removes the `dark` class on the root element and
 * persists the preference in localStorage.
 *
 * @param {boolean} [enable]  Force on/off; omit to toggle
 * @returns {boolean}  The new dark-mode state
 */
export function toggleDarkMode (enable)
{
	const root = document.documentElement;
	const isOn = root.classList.contains ("dark");
	const next = (enable !== undefined) ? enable : !isOn;

	if (next) {
		root.classList.add ("dark");
		try { localStorage.setItem ("amcf-dark-mode", "1"); } catch (_) { /* noop */ }
	} else {
		root.classList.remove ("dark");
		try { localStorage.setItem ("amcf-dark-mode", "0"); } catch (_) { /* noop */ }
	}

	return next;
}

/**
 * Restore dark-mode preference from localStorage on startup.
 * Priority: (1) localStorage, (2) OS prefers-color-scheme, (3) server default.
 *
 * @param {string} [serverDefault]  "dark", "light", or "system" (default: "system")
 * @returns {boolean}  Whether dark mode was activated
 */
export function restoreDarkModePreference (serverDefault)
{
	try {
		const stored = localStorage.getItem ("amcf-dark-mode");
		if (stored !== null) {
			const wantDark = stored === "1";
			if (wantDark)
				document.documentElement.classList.add ("dark");
			else
				document.documentElement.classList.remove ("dark");
			return wantDark;
		}
	} catch (_) {
		/* localStorage unavailable */
	}

	const sd = (serverDefault || "system").toLowerCase ();
	let wantDark;
	if (sd === "dark") {
		wantDark = true;
	} else if (sd === "light") {
		wantDark = false;
	} else {
		wantDark = window.matchMedia && window.matchMedia ("(prefers-color-scheme: dark)").matches;
	}

	if (wantDark)
		document.documentElement.classList.add ("dark");
	else
		document.documentElement.classList.remove ("dark");
	return wantDark;
}

/**
 * Check whether dark mode is currently active.
 *
 * @returns {boolean}
 */
export function isDarkMode ()
{
	return document.documentElement.classList.contains ("dark");
}
