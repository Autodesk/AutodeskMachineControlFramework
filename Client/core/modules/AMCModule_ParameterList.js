/*++

Copyright (C) 2021 Autodesk Inc.

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


import * as Assert from "../common/AMCAsserts.js";
import * as Common from "../common/AMCCommon.js"


export default class AMCApplicationModule_ParameterList extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_ParameterList");

		this.usesV2Frontend = true;

		this.entries = [];

		// Per-column configuration (identifier -> {visible, width, sizeable}).
		// Defaults: all visible, flexible width, not resizable. Overridable via
		// <column> subnodes in config.xml.
		this.columns = this.defaultColumns ();

		this.headers = [];
		this.rebuildHeaders ();

		this.stateid = 1;
		this.loadingtext = "";
		this.entriesperpage = 25;
		this.editevent = "";

		// Stable per-list key (server provided via v2 attribute "preferencekey").
		// Used together with the page name to scope persisted view preferences.
		this.preferenceKey = "";

		// Client-side view state. This is applied on top of the raw server entries
		// by applyView() and persisted per-user through the preferences API.
		this.favorites = [];                       // array of rowKey strings, insertion ordered
		this.showOnlyFavorites = false;
		this.sort = { column: "", direction: "asc" };   // column is a header "value" key
		this.filters = { groups: [], systems: [] };     // empty array = no restriction
		this.presets = {};                          // presetName -> preference document
		this.activePreset = "";

		// Persistence bookkeeping. preferencesLoaded guards against saving before the
		// initial load has completed. onPreferencesChanged is an optional hook the
		// frontend can set to react to state mutations (e.g. trigger a redraw).
		// _loadTriggered ensures the one-time restore only fires once it is safe
		// (stable key present, application available and session authenticated).
		this.preferencesLoaded = false;
		this._loadTriggered = false;
		this.onPreferencesChanged = null;
		this._saveTimer = null;

		this.PREFERENCE_DOMAIN = "parameterlist";

		// Rows and values come from /frontend/parameterlist/{uuid}: the static row definitions
		// are fetched once (definitionHash), afterwards only the values are polled (valuesHash).
		this.definitionHash = "";
		this.valuesHash = "";
		this.parameterFetchInFlight = false;
		this.onEntriesChanged = null;

		// A frontend that reports its display state (setDisplayed) only gets values while the
		// list is actually shown, e.g. not while it sits in an inactive tab. Frontends that never
		// report it are treated as always displaying the list while its page is active.
		this.displayTracked = false;
		this.displayed = false;

		this.updateFromJSON (moduleJSON);
	}


	// Coerces a server flag (boolean or "1"/"0"/"true"/"false") to a boolean.
	parseFlag (value, fallback)
	{
		if (value === undefined || value === null)
			return fallback;
		return (value === true || value === "1" || value === "true");
	}


	// The canonical column set in fixed order, all visible and flexible.
	defaultColumns ()
	{
		return [
			{ identifier: 'parameter', value: 'paramDescription', text: 'Parameter', visible: true, width: '', sizeable: false },
			{ identifier: 'value',     value: 'paramValue',       text: 'Value',     visible: true, width: '', sizeable: false },
			{ identifier: 'group',     value: 'paramGroup',       text: 'Group',     visible: true, width: '', sizeable: false },
			{ identifier: 'system',    value: 'paramSystem',      text: 'System',    visible: true, width: '', sizeable: false },
		];
	}


	// Replaces this.columns from a server-provided column array (if any). Unknown
	// or missing entries fall back to the canonical defaults, preserving order.
	applyColumns (incoming)
	{
		if (!Array.isArray (incoming) || incoming.length === 0)
			return;

		const byIdentifier = {};
		for (let column of incoming) {
			if (column && column.identifier)
				byIdentifier[column.identifier] = column;
		}

		this.columns = this.defaultColumns ().map ((base) => {
			const override = byIdentifier[base.identifier];
			if (!override)
				return base;
			return {
				identifier: base.identifier,
				value: base.value,
				text: (override.text !== undefined && override.text !== null && override.text !== '') ? override.text : base.text,
				visible: this.parseFlag (override.visible, true),
				width: (override.width !== undefined && override.width !== null) ? String (override.width) : '',
				sizeable: this.parseFlag (override.sizeable, false),
			};
		});
	}


	// Assembles the visible headers, carrying width and resize flags to clients.
	rebuildHeaders ()
	{
		const visibleColumns = this.columns
			.filter ((column) => column.visible)
			.map ((column) => ({ text: column.text, value: column.value, width: column.width, sizeable: column.sizeable }));

		while (this.headers.length > 0) this.headers.pop ();
		for (let column of visibleColumns) this.headers.push (column);
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);

		if (!updateJSON.entries)
			return;

		if (updateJSON.loadingtext)
			this.loadingtext = Assert.StringValue (updateJSON.loadingtext);
		if (updateJSON.entriesperpage)
			this.entriesperpage = Assert.IntegerValue (updateJSON.entriesperpage);
		if (updateJSON.editevent !== undefined)
			this.editevent = Assert.StringValue (updateJSON.editevent);

		this.applyColumns (updateJSON.columns);
		this.rebuildHeaders ();

		let oldEntryCount = this.entries.length;
		for (let index = 0; index < oldEntryCount; index++) {
			this.entries.pop();
		}
		for (let entry of updateJSON.entries) {
			this.entries.push(entry);
		}
	}


	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return;

		if (attrs.loadingtext !== undefined)
			this.loadingtext = attrs.loadingtext;
		if (attrs.entriesperpage !== undefined)
			this.entriesperpage = attrs.entriesperpage;
		if (attrs.editevent !== undefined)
			this.editevent = attrs.editevent;
		if (attrs.preferencekey !== undefined)
			this.preferenceKey = String (attrs.preferencekey);

		this.applyColumns (attrs.columns);
		this.rebuildHeaders ();
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");

		// The stable key and the authentication token both arrive asynchronously
		// with the polled frontend state, so drive the one-time preference restore
		// from here. maybeLoadPreferences() is guarded and only fires once ready.
		this.maybeLoadPreferences ();

		// Called once per frontend poll while the page is active, and on every pushed change.
		this.refreshParameters ();
	}


	// -----------------------------------------------------------------------
	// Parameter data (definition + values) via the parameter list endpoint
	// -----------------------------------------------------------------------

	setDisplayed (flag)
	{
		this.displayTracked = true;
		this.displayed = !!flag;
		if (this.displayed)
			this.refreshParameters ();
	}

	isDisplayed ()
	{
		return (!this.displayTracked) || this.displayed;
	}

	// The values are not part of the frontend status, so the application calls this periodically
	// while the status is pushed instead of polled.
	refreshLiveData ()
	{
		this.maybeLoadPreferences ();
		this.refreshParameters ();
	}

	// Comma separated row indices/ranges of the rows that pass the active filters, or ""
	// if no filter is active (= all rows). Returns null if no row passes the filters.
	requestedRows ()
	{
		if (!this.hasActiveFilters ())
			return "";

		const ranges = [];
		let rangeStart = -1;
		let rangeEnd = -1;
		this.entries.forEach ((row, index) => {
			if (!this.rowPassesFilters (row))
				return;
			if (rangeStart >= 0 && index === rangeEnd + 1) {
				rangeEnd = index;
				return;
			}
			if (rangeStart >= 0)
				ranges.push ((rangeStart === rangeEnd) ? `${rangeStart}` : `${rangeStart}-${rangeEnd}`);
			rangeStart = index;
			rangeEnd = index;
		});
		if (rangeStart >= 0)
			ranges.push ((rangeStart === rangeEnd) ? `${rangeStart}` : `${rangeStart}-${rangeEnd}`);

		return (ranges.length > 0) ? ranges.join (",") : null;
	}

	refreshParameters ()
	{
		if (this.parameterFetchInFlight || !this.isActive () || !this.isDisplayed ())
			return;

		if (!this.definitionHash) {
			this.fetchDefinition ();
			return;
		}

		const rows = this.requestedRows ();
		if (rows === null)
			return;

		this.fetchValues (rows, this.valuesHash);
	}

	fetchDefinition ()
	{
		const app = this.getApplication ();
		if (!app)
			return Promise.resolve ();

		this.parameterFetchInFlight = true;
		return app.axiosGetRequest ("/frontend/parameterlist/" + this.uuid)
		.then (resultJSON => {
			this.parameterFetchInFlight = false;
			const data = resultJSON.data || {};
			const definitions = Array.isArray (data.entries) ? data.entries : [];
			const values = Array.isArray (data.values) ? data.values : [];

			const newEntries = definitions.map ((definition, index) =>
				Object.assign ({}, definition, { paramValue: (index < values.length) ? values[index] : "" }));
			this.entries.splice (0, this.entries.length, ...newEntries);

			this.definitionHash = data.definitionhash || "";
			this.valuesHash = "";
			this.notifyEntriesChanged ();
		})
		.catch (() => {
			this.parameterFetchInFlight = false;
		});
	}

	fetchValues (rows, knownValuesHash)
	{
		const app = this.getApplication ();
		if (!app)
			return Promise.resolve ();

		const params = { definitionhash: this.definitionHash };
		if (rows)
			params.rows = rows;
		if (knownValuesHash)
			params.valueshash = knownValuesHash;

		this.parameterFetchInFlight = true;
		return app.axiosGetRequest ("/frontend/parameterlist/" + this.uuid + "/values", { params: params })
		.then (resultJSON => {
			this.parameterFetchInFlight = false;
			const data = resultJSON.data || {};

			if (data.definitionchanged) {
				this.definitionHash = "";
				return this.fetchDefinition ();
			}

			this.valuesHash = data.valueshash || "";
			if (data.unchanged || !Array.isArray (data.values))
				return;

			const indices = rows ? this.expandRows (rows) : this.entries.map ((row, index) => index);
			let changed = false;
			indices.forEach ((entryIndex, valueIndex) => {
				const row = this.entries[entryIndex];
				const value = data.values[valueIndex];
				if (row && value !== undefined && row.paramValue !== value) {
					// Replace instead of mutating so frontends that compare row identity re-render the row.
					this.entries[entryIndex] = Object.assign ({}, row, { paramValue: value });
					changed = true;
				}
			});
			if (changed)
				this.notifyEntriesChanged ();
		})
		.catch (() => {
			this.parameterFetchInFlight = false;
		});
	}

	// Refreshes the values of all rows regardless of filters, e.g. before an export.
	loadAllValues ()
	{
		if (!this.definitionHash)
			return this.fetchDefinition ();
		return this.fetchValues ("", "");
	}

	expandRows (rows)
	{
		const indices = [];
		for (let token of rows.split (",")) {
			const bounds = token.split ("-").map ((part) => parseInt (part, 10));
			const last = (bounds.length > 1) ? bounds[1] : bounds[0];
			for (let index = bounds[0]; index <= last; index++)
				indices.push (index);
		}
		return indices;
	}

	notifyEntriesChanged ()
	{
		if (typeof this.onEntriesChanged === "function")
			this.onEntriesChanged ();
	}


	// -----------------------------------------------------------------------
	// View state: identity, sorting, filtering, favorites
	// -----------------------------------------------------------------------

	// Stable identity of a parameter row, matching the frontend rowKey().
	rowKey (row)
	{
		if (!row)
			return "";
		return `${row.paramInstance ?? ''}.${row.paramGroupName ?? ''}.${row.paramName ?? ''}`;
	}

	// Fully-qualified preference scope key for this list, combining the page and
	// the server-provided stable list key. Returns "" if no stable key exists.
	preferenceScopeKey ()
	{
		if (!this.preferenceKey)
			return "";
		const pageName = (this.page && this.page.name) ? this.page.name : "";
		return `${pageName}.${this.preferenceKey}`;
	}

	isFavorite (row)
	{
		return this.favorites.indexOf (this.rowKey (row)) >= 0;
	}

	toggleFavorite (row)
	{
		const key = this.rowKey (row);
		const index = this.favorites.indexOf (key);
		if (index >= 0)
			this.favorites.splice (index, 1);
		else
			this.favorites.push (key);
		this.markPreferencesDirty ();
	}

	setShowOnlyFavorites (flag)
	{
		this.showOnlyFavorites = !!flag;
		this.markPreferencesDirty ();
	}

	// Toggles the sort on a column value key: unsorted -> asc -> desc -> unsorted.
	toggleSort (columnValue)
	{
		if (this.sort.column !== columnValue) {
			this.sort = { column: columnValue, direction: "asc" };
		} else if (this.sort.direction === "asc") {
			this.sort = { column: columnValue, direction: "desc" };
		} else {
			this.sort = { column: "", direction: "asc" };
		}
		this.markPreferencesDirty ();
	}

	setSort (columnValue, direction)
	{
		this.sort = { column: columnValue || "", direction: (direction === "desc") ? "desc" : "asc" };
		this.markPreferencesDirty ();
	}

	// Distinct group descriptions present in the current entries (for the filter UI).
	distinctGroups ()
	{
		return this.distinctValues ("paramGroup");
	}

	// Distinct system descriptions present in the current entries (for the filter UI).
	distinctSystems ()
	{
		return this.distinctValues ("paramSystem");
	}

	distinctValues (field)
	{
		const seen = {};
		const result = [];
		for (let row of this.entries) {
			const value = row ? row[field] : undefined;
			if (value !== undefined && value !== null && value !== "" && !seen[value]) {
				seen[value] = true;
				result.push (value);
			}
		}
		return result;
	}

	setGroupFilter (groups)
	{
		this.filters.groups = Array.isArray (groups) ? groups.slice () : [];
		this.markPreferencesDirty ();
	}

	setSystemFilter (systems)
	{
		this.filters.systems = Array.isArray (systems) ? systems.slice () : [];
		this.markPreferencesDirty ();
	}

	clearFilters ()
	{
		this.filters = { groups: [], systems: [] };
		this.markPreferencesDirty ();
	}

	// Applies favorites-first ordering, category filters, favorite-only visibility
	// and column sorting to a raw entries array, returning a new array. The input
	// is never mutated.
	hasActiveFilters ()
	{
		return ((this.filters.groups || []).length > 0) || ((this.filters.systems || []).length > 0) || this.showOnlyFavorites;
	}

	// Category filters (AND across facets, OR within a facet) and favorites-only visibility.
	rowPassesFilters (row)
	{
		const groupFilter = this.filters.groups || [];
		const systemFilter = this.filters.systems || [];
		if (groupFilter.length > 0 && groupFilter.indexOf (row.paramGroup) < 0)
			return false;
		if (systemFilter.length > 0 && systemFilter.indexOf (row.paramSystem) < 0)
			return false;
		if (this.showOnlyFavorites && !this.isFavorite (row))
			return false;
		return true;
	}

	applyView (rows)
	{
		let result = Array.isArray (rows) ? rows.slice () : [];

		if (this.hasActiveFilters ())
			result = result.filter ((row) => this.rowPassesFilters (row));

		// Sorting. A stable sort keeps insertion order for equal keys. Favorites
		// are always floated to the top regardless of the active column sort.
		const sortColumn = this.sort.column;
		const sortDir = (this.sort.direction === "desc") ? -1 : 1;
		result = result
			.map ((row, index) => ({ row, index }))
			.sort ((a, b) => {
				const aFav = this.isFavorite (a.row) ? 0 : 1;
				const bFav = this.isFavorite (b.row) ? 0 : 1;
				if (aFav !== bFav)
					return aFav - bFav;

				if (sortColumn) {
					const aValue = a.row[sortColumn];
					const bValue = b.row[sortColumn];
					const comparison = this.compareValues (aValue, bValue);
					if (comparison !== 0)
						return comparison * sortDir;
				}

				return a.index - b.index;
			})
			.map ((entry) => entry.row);

		return result;
	}

	// Compares two cell values numerically when both look numeric, otherwise
	// case-insensitively as strings.
	compareValues (a, b)
	{
		const aStr = (a === undefined || a === null) ? "" : String (a);
		const bStr = (b === undefined || b === null) ? "" : String (b);

		const aNum = parseFloat (aStr);
		const bNum = parseFloat (bStr);
		const bothNumeric = !isNaN (aNum) && !isNaN (bNum) &&
			aStr.trim () !== "" && bStr.trim () !== "";
		if (bothNumeric) {
			if (aNum < bNum) return -1;
			if (aNum > bNum) return 1;
			return 0;
		}

		return aStr.toLowerCase ().localeCompare (bStr.toLowerCase ());
	}


	// -----------------------------------------------------------------------
	// Presets
	// -----------------------------------------------------------------------

	// The current active view collapsed into a plain, serializable object.
	currentViewSnapshot ()
	{
		return {
			favorites: this.favorites.slice (),
			showOnlyFavorites: this.showOnlyFavorites,
			sort: { column: this.sort.column, direction: this.sort.direction },
			filters: {
				groups: (this.filters.groups || []).slice (),
				systems: (this.filters.systems || []).slice ()
			}
		};
	}

	applyViewSnapshot (snapshot)
	{
		if (!snapshot)
			return;
		if (Array.isArray (snapshot.favorites))
			this.favorites = snapshot.favorites.slice ();
		if (snapshot.showOnlyFavorites !== undefined)
			this.showOnlyFavorites = !!snapshot.showOnlyFavorites;
		if (snapshot.sort) {
			this.sort = {
				column: snapshot.sort.column || "",
				direction: (snapshot.sort.direction === "desc") ? "desc" : "asc"
			};
		}
		if (snapshot.filters) {
			this.filters = {
				groups: Array.isArray (snapshot.filters.groups) ? snapshot.filters.groups.slice () : [],
				systems: Array.isArray (snapshot.filters.systems) ? snapshot.filters.systems.slice () : []
			};
		}
	}

	listPresets ()
	{
		return Object.keys (this.presets).sort ((a, b) => a.toLowerCase ().localeCompare (b.toLowerCase ()));
	}

	savePreset (name)
	{
		const trimmed = (name || "").trim ();
		if (trimmed.length === 0)
			return false;
		this.presets[trimmed] = this.currentViewSnapshot ();
		this.activePreset = trimmed;
		this.markPreferencesDirty ();
		return true;
	}

	loadPreset (name)
	{
		const preset = this.presets[name];
		if (!preset)
			return false;
		this.applyViewSnapshot (preset);
		this.activePreset = name;
		this.markPreferencesDirty ();
		return true;
	}

	deletePreset (name)
	{
		if (!Object.prototype.hasOwnProperty.call (this.presets, name))
			return false;
		delete this.presets[name];
		if (this.activePreset === name)
			this.activePreset = "";
		this.markPreferencesDirty ();
		return true;
	}


	// -----------------------------------------------------------------------
	// Persistence (serialization + REST via the owning application)
	// -----------------------------------------------------------------------

	// The complete preference document persisted for this list.
	serializePreferences ()
	{
		const snapshot = this.currentViewSnapshot ();
		return {
			version: 1,
			favorites: snapshot.favorites,
			showOnlyFavorites: snapshot.showOnlyFavorites,
			sort: snapshot.sort,
			filters: snapshot.filters,
			activePreset: this.activePreset,
			presets: this.presets
		};
	}

	// Applies a persisted preference document to the live view state.
	applyPreferenceDocument (doc)
	{
		if (!doc || typeof doc !== "object")
			return;
		this.applyViewSnapshot (doc);
		this.presets = (doc.presets && typeof doc.presets === "object") ? doc.presets : {};
		this.activePreset = (typeof doc.activePreset === "string") ? doc.activePreset : "";
	}

	getApplication ()
	{
		return (this.page && this.page.application) ? this.page.application : null;
	}

	// Returns true once it is safe to talk to the preferences backend: a stable
	// scope key exists, the owning application is available and the session is
	// authenticated (the token arrives asynchronously after login).
	preferencesReady ()
	{
		const scopeKey = this.preferenceScopeKey ();
		const app = this.getApplication ();
		if (!scopeKey || !app || typeof app.getUserPreference !== "function")
			return false;
		if (app.API && typeof Common.nullToken === "function" && app.API.authToken === Common.nullToken ())
			return false;
		return true;
	}

	// Triggers the one-time preference restore as soon as the prerequisites are
	// met. Called repeatedly from updateFromV2Attributes (once per poll) and
	// refreshLiveData so it waits for the stable key and authentication to become available.
	maybeLoadPreferences ()
	{
		if (this._loadTriggered)
			return;
		if (!this.preferencesReady ())
			return;
		this._loadTriggered = true;
		this.loadPreferences ();
	}

	// Loads persisted preferences for this list from the backend. Safe to call
	// when no stable key or application is available (resolves to false).
	async loadPreferences ()
	{
		const scopeKey = this.preferenceScopeKey ();
		const app = this.getApplication ();
		if (!scopeKey || !app || typeof app.getUserPreference !== "function") {
			this.preferencesLoaded = true;
			return false;
		}

		try {
			const doc = await app.getUserPreference (this.PREFERENCE_DOMAIN, scopeKey);
			if (doc)
				this.applyPreferenceDocument (doc);
		} catch (loadError) {
			// Non-fatal: fall back to defaults if preferences cannot be loaded.
		}

		this.preferencesLoaded = true;
		if (typeof this.onPreferencesChanged === "function")
			this.onPreferencesChanged ();
		return true;
	}

	// Flags the preference state as changed. Notifies any listener and schedules a
	// debounced persist so rapid UI interactions collapse into a single request.
	markPreferencesDirty ()
	{
		if (typeof this.onPreferencesChanged === "function")
			this.onPreferencesChanged ();
		this.schedulePersist ();

		// Rows that just became visible through a filter change may carry outdated values.
		this.refreshParameters ();
	}

	schedulePersist (delayMs)
	{
		if (!this.preferencesLoaded)
			return;
		const scopeKey = this.preferenceScopeKey ();
		const app = this.getApplication ();
		if (!scopeKey || !app || typeof app.storeUserPreference !== "function")
			return;

		if (this._saveTimer)
			clearTimeout (this._saveTimer);

		this._saveTimer = setTimeout (() => {
			this._saveTimer = null;
			try {
				app.storeUserPreference (this.PREFERENCE_DOMAIN, scopeKey, this.serializePreferences ())
					.catch (() => { /* transient persist failures are non-fatal */ });
			} catch (persistError) {
				// Ignore synchronous persist errors; state remains in memory.
			}
		}, (delayMs === undefined) ? 400 : delayMs);
	}

}
