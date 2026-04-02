/**
 * Reactive wrapper around the shared AMCF core application.
 *
 * The core AMCApplication class (Client/core) is framework-agnostic JS.
 * This module wraps it in a Svelte 5 reactive store so that any component
 * reading `app` will re-render when the underlying state changes.
 */

// @ts-ignore — core JS module has no type declarations yet
import AMCApplication from '@core/common/AMCApplication.js';

let _app = $state<any>(null);
let _timer: ReturnType<typeof setInterval> | null = null;
let _tick = $state(0);

export function getApp () { return _app; }
export function getTick () { return _tick; }

export function initApp () {
	if (_app) return _app;

	let baseURL = '/api';
	if (import.meta.env.DEV) {
		baseURL = 'http://localhost:8869/api';
	}

	_app = new AMCApplication(baseURL, () => {});

	_app.retrieveConfiguration(null);

	_timer = setInterval(() => {
		if (_app) {
			_app.retrieveFrontendState()
				.finally(() => {
					_app.updateModules();
					_tick++;
				});
		}
	}, 600);

	return _app;
}

export function destroyApp () {
	if (_timer) {
		clearInterval(_timer);
		_timer = null;
	}
	_app = null;
}
