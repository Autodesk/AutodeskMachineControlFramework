// Run with: cd Client/tests && npm test   (or from the repository root: node --test 'Client/tests/*.test.mjs')
//
// The fallback /api/frontend polling, used when the event stream is unavailable, never adds a second
// request: a tick is skipped while a request is pending, and the gate opens again however the request
// ends (reply, error, timeout).

import { test } from "node:test";
import assert from "node:assert/strict";

import { importCore } from "./support/coreLoader.mjs";

const AMCApplication = await importCore ("common/AMCApplication.js");
const AMCApplicationPage = await importCore ("common/AMCPage.js");

const POLL_INTERVAL_MS = 600;
const MAIN_PAGE_UUID = "00000000-0000-4000-8000-000000000001";
const OTHER_PAGE_UUID = "00000000-0000-4000-8000-000000000002";

function frontendJSON (revision) {
	return {
		protocol: "amcf-frontend", version: 2, revision: revision,
		menuitems: [], toolbaritems: [], custompages: [], dialogs: [],
		pages: [
			{ name: "main", uuid: MAIN_PAGE_UUID, visible: true, modules: [] },
			{ name: "other", uuid: OTHER_PAGE_UUID, visible: true, modules: [] }
		]
	};
}

function flushPromises () {
	return new Promise ((resolve) => setImmediate (resolve));
}

// A logged-in application on page "main" with the HTTP boundary stubbed. By default GET requests stay
// pending until the test settles them; with serverDelayMS they are answered after that (mocked) delay.
function createApp (t, serverDelayMS) {
	t.mock.timers.enable ({ apis: ["setInterval", "setTimeout"] });

	const app = new AMCApplication ("http://127.0.0.1:1/api", null);
	app.API.authToken = "1".repeat (64);

	const server = { gets: [], inFlight: 0, maxInFlight: 0, revision: 1 };
	app.axiosGetRequest = (url, config) => new Promise ((resolve, reject) => {
		const request = { url, config, resolve, reject };
		server.gets.push (request);
		server.inFlight++;
		server.maxInFlight = Math.max (server.maxInFlight, server.inFlight);
		request.settle = (fn, value) => { server.inFlight--; fn (value); };
		if (serverDelayMS !== undefined)
			setTimeout (() => request.settle (resolve, { data: frontendJSON (++server.revision) }), serverDelayMS);
	});
	app.axiosPostRequest = () => Promise.resolve ({ data: {} });

	for (const pageJSON of frontendJSON (1).pages) {
		const page = new AMCApplicationPage (app, pageJSON);
		app.AppContent.Pages.push (page);
		app.AppContent.PageMap.set (page.name, page);
	}
	app.AppState.activePage = "main";
	app._setFrontendState (frontendJSON (1), "main|");

	app.FrontendUpdates.enabled = true;
	app._startFrontendPolling ();
	t.after (() => app.stopFrontendUpdates ());

	return { app, server };
}

function polls (server) {
	return server.gets.filter (request => request.url === "/frontend" && request.config && request.config.params);
}

// Advances the mocked timers in small steps, letting the request promises settle in between.
async function advance (t, ms) {
	const stepMS = 100;
	for (let elapsedMS = 0; elapsedMS < ms; elapsedMS += stepMS) {
		t.mock.timers.tick (stepMS);
		await flushPromises ();
	}
}

test ("a poll tick is skipped while the previous poll is pending", async (t) => {
	const { app, server } = createApp (t);

	await advance (t, POLL_INTERVAL_MS);
	assert.equal (polls (server).length, 1);
	assert.equal (app.API.frontendRequestsInFlight, 1);

	await advance (t, POLL_INTERVAL_MS * 10);
	assert.equal (polls (server).length, 1, "no further poll while the first one is pending");
});

test ("the gate opens after a reply", async (t) => {
	const { app, server } = createApp (t);

	await advance (t, POLL_INTERVAL_MS);
	polls (server)[0].settle (polls (server)[0].resolve, { data: frontendJSON (2) });
	await flushPromises ();
	assert.equal (app.API.frontendRequestsInFlight, 0);
	assert.equal (app.API.frontendRevision, 2);

	await advance (t, POLL_INTERVAL_MS);
	assert.equal (polls (server).length, 2);
});

test ("the gate opens after a failed request", async (t) => {
	const { app, server } = createApp (t);

	await advance (t, POLL_INTERVAL_MS);
	const error = new Error ("Request failed with status code 500");
	error.response = { status: 500, data: { message: "internal error" } };
	polls (server)[0].settle (polls (server)[0].reject, error);
	await flushPromises ();
	assert.equal (app.API.frontendRequestsInFlight, 0);
	assert.equal (app.API.unsuccessfulFrontendCounter, 1);

	await advance (t, POLL_INTERVAL_MS);
	assert.equal (polls (server).length, 2);
});

test ("the gate opens when a poll times out", async (t) => {
	const { app, server } = createApp (t);

	await advance (t, POLL_INTERVAL_MS);

	// axios rejects a request that exceeds its timeout (axiosGetRequest's default) with ECONNABORTED.
	const error = new Error ("timeout of 10000ms exceeded");
	error.code = "ECONNABORTED";
	polls (server)[0].settle (polls (server)[0].reject, error);
	await flushPromises ();
	assert.equal (app.API.frontendRequestsInFlight, 0);

	await advance (t, POLL_INTERVAL_MS);
	assert.equal (polls (server).length, 2);
});

test ("requests never overlap against a server slower than the poll interval", async (t) => {
	const { app, server } = createApp (t, 1500);

	await advance (t, 20000);
	assert.equal (server.maxInFlight, 1);
	// One request per 1500 ms reply plus up to one interval of waiting for the next tick.
	assert.ok (polls (server).length >= 9 && polls (server).length <= 14, "polls: " + polls (server).length);
	// Every reply was applied, in order.
	assert.equal (app.API.frontendRevision, server.revision);
	assert.equal (app.API.frontendAppliedSerial, app.API.frontendRequestSerial - server.inFlight);
});

test ("a navigation refresh is sent at once and holds back the polling until it settles", async (t) => {
	const { app, server } = createApp (t);

	await advance (t, POLL_INTERVAL_MS);
	const poll = polls (server)[0];

	app.changePage ("other");
	assert.equal (polls (server).length, 2, "the new scope is requested without waiting for the pending poll");
	const navigation = polls (server)[1];
	assert.equal (navigation.config.params.pages, "other");

	navigation.settle (navigation.resolve, { data: frontendJSON (3) });
	await flushPromises ();
	await advance (t, POLL_INTERVAL_MS * 3);
	assert.equal (polls (server).length, 2, "the old poll is still pending");

	// The older poll's reply arrives last and must not replace the newer state.
	poll.settle (poll.resolve, { data: frontendJSON (2) });
	await flushPromises ();
	assert.equal (app.API.frontendRevision, 3);
	assert.equal (app.API.frontendScope, "other|");
	assert.equal (app.API.frontendRequestsInFlight, 0);

	await advance (t, POLL_INTERVAL_MS);
	assert.equal (polls (server).length, 3);
	assert.equal (polls (server)[2].config.params.since, 3);
});

test ("a poll pending across a switch to the stream and back keeps the gate until it settles", async (t) => {
	const { app, server } = createApp (t);

	await advance (t, POLL_INTERVAL_MS);
	const poll = polls (server)[0];

	// The stream takes over: polling stops, and its structure load is not a poll.
	app._enterFrontendStreamMode ();
	assert.equal (app.FrontendUpdates.mode, "stream");
	assert.equal (app.FrontendUpdates.pollTimer, null);

	// The stream fails again before the poll has settled.
	app._startFrontendPolling ();
	await advance (t, POLL_INTERVAL_MS * 3);
	assert.equal (polls (server).length, 1, "still gated by the poll sent before the switch");

	poll.settle (poll.reject, Object.assign (new Error ("timeout"), { code: "ECONNABORTED" }));
	await flushPromises ();
	assert.equal (app.API.frontendRequestsInFlight, 0);

	await advance (t, POLL_INTERVAL_MS);
	assert.equal (polls (server).length, 2);
});

test ("a poll reply arriving in stream mode is dropped and releases the gate", async (t) => {
	const { app, server } = createApp (t);

	await advance (t, POLL_INTERVAL_MS);
	const poll = polls (server)[0];
	app._stopFrontendPolling ();
	app.FrontendUpdates.mode = "stream";

	poll.settle (poll.resolve, { data: frontendJSON (5) });
	await flushPromises ();
	assert.equal (app.API.frontendRevision, 1, "a scoped poll reply must not replace the stream's state");
	assert.equal (app.API.frontendRequestsInFlight, 0);
});
