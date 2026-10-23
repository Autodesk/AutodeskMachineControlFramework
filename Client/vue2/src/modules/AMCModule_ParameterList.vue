<!--

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

!-->

<template>

<div v-if="module.visible !== false" class="plist-root">
	<div class="plist-toolbar">
		<span v-if="hasPreferences && filterSummary" class="plist-filter-summary">{{ filterSummary }}</span>
		<v-btn small text class="plist-csv-btn" title="Download parameters as CSV" @click="downloadCsv()">
			<v-icon small left>mdi-download</v-icon>
			CSV
		</v-btn>
		<v-menu v-if="hasPreferences" offset-y :close-on-content-click="false" left>
			<template v-slot:activator="{ on, attrs }">
				<v-btn icon small v-bind="attrs" v-on="on" aria-label="Parameter list options">
					<v-icon small>mdi-tune-variant</v-icon>
				</v-btn>
			</template>
			<v-list dense class="plist-menu">
				<v-subheader>Sort by</v-subheader>
				<v-list-item v-for="h in module.headers" :key="'sort-' + h.value" @click="sortBy(h)">
					<v-list-item-title>{{ h.text }}</v-list-item-title>
					<v-list-item-icon v-if="sortState.column === h.value">
						<v-icon small>{{ sortState.direction === 'asc' ? 'mdi-arrow-up' : 'mdi-arrow-down' }}</v-icon>
					</v-list-item-icon>
				</v-list-item>

				<v-divider></v-divider>
				<v-list-item @click="setShowOnlyFavorites(!showOnlyFavorites)">
					<v-list-item-icon>
						<v-icon small>{{ showOnlyFavorites ? 'mdi-checkbox-marked' : 'mdi-checkbox-blank-outline' }}</v-icon>
					</v-list-item-icon>
					<v-list-item-title>Show only favorites</v-list-item-title>
				</v-list-item>
				<v-list-item @click="setShowOnlyFavorites(false)">
					<v-list-item-title>Show everything</v-list-item-title>
				</v-list-item>

				<template v-if="groupOptions.length > 0">
					<v-divider></v-divider>
					<v-subheader>Filter by group</v-subheader>
					<v-list-item v-for="g in groupOptions" :key="'group-' + g" @click="toggleGroupFilter(g)">
						<v-list-item-icon>
							<v-icon small>{{ activeGroups.indexOf(g) >= 0 ? 'mdi-checkbox-marked' : 'mdi-checkbox-blank-outline' }}</v-icon>
						</v-list-item-icon>
						<v-list-item-title>{{ g }}</v-list-item-title>
					</v-list-item>
				</template>

				<template v-if="systemOptions.length > 0">
					<v-divider></v-divider>
					<v-subheader>Filter by system</v-subheader>
					<v-list-item v-for="s in systemOptions" :key="'system-' + s" @click="toggleSystemFilter(s)">
						<v-list-item-icon>
							<v-icon small>{{ activeSystems.indexOf(s) >= 0 ? 'mdi-checkbox-marked' : 'mdi-checkbox-blank-outline' }}</v-icon>
						</v-list-item-icon>
						<v-list-item-title>{{ s }}</v-list-item-title>
					</v-list-item>
				</template>

				<v-list-item v-if="activeGroups.length > 0 || activeSystems.length > 0" @click="clearFilters()">
					<v-list-item-title>Clear filters</v-list-item-title>
				</v-list-item>

				<v-divider></v-divider>
				<v-list-item @click="openSaveDialog()">
					<v-list-item-title>Save view as preset…</v-list-item-title>
				</v-list-item>
				<template v-if="presetNames.length > 0">
					<v-subheader>Load preset</v-subheader>
					<v-list-item v-for="name in presetNames" :key="'preset-' + name">
						<v-list-item-title @click="loadPreset(name)" class="plist-preset-name">{{ name }}</v-list-item-title>
						<v-list-item-icon @click.stop="deletePreset(name)">
							<v-icon small>mdi-delete-outline</v-icon>
						</v-list-item-icon>
					</v-list-item>
				</template>
			</v-list>
		</v-menu>
	</div>

	<v-data-table
		:headers="tableHeaders"
		:items="viewEntries"
		:items-per-page="module.entriesperpage || -1"
		class="plist-table"
		disable-pagination
		disable-sort
		hide-default-footer
		width="100%"
	>
		<template v-if="hasPreferences" v-slot:[`item.__favorite`]="{ item }">
			<v-icon
				small
				class="plist-icon-btn"
				:class="{ 'plist-fav-active': isFavorite(item) }"
				@click="toggleFavorite(item)"
			>{{ isFavorite(item) ? 'mdi-star' : 'mdi-star-outline' }}</v-icon>
		</template>

		<template v-slot:[`item.paramValue`]="{ item }">
			<div v-if="isEditable(item) && editingKey === rowKey(item)" class="plist-edit-wrap">
				<input
					class="plist-edit-input"
					:type="isNumeric(item) ? 'number' : 'text'"
					:min="item.paramMin || null"
					:max="item.paramMax || null"
					:step="item.paramStep || null"
					v-model="editValue"
					v-focus
					@keydown.enter.prevent="acceptEdit(item)"
					@keydown.esc.prevent="cancelEdit()"
				/>
				<v-icon small class="plist-icon-btn plist-accept" @click="acceptEdit(item)">mdi-check</v-icon>
				<v-icon small class="plist-icon-btn plist-cancel" @click="cancelEdit()">mdi-close</v-icon>
			</div>
			<span
				v-else-if="isEditable(item)"
				class="plist-value-display"
				title="Click to edit"
				@click="startEdit(item)"
			>{{ item.paramValue }}</span>
			<span v-else>{{ item.paramValue }}</span>
		</template>

		<template v-slot:no-data>
			<div class="plist-empty">No entries available</div>
		</template>
	</v-data-table>

	<v-dialog v-model="saveDialog" max-width="420">
		<v-card>
			<v-card-title>Save view preset</v-card-title>
			<v-card-text>
				<v-text-field
					v-model="presetName"
					label="Preset name"
					autofocus
					@keydown.enter.prevent="confirmSavePreset()"
				></v-text-field>
			</v-card-text>
			<v-card-actions>
				<v-spacer></v-spacer>
				<v-btn text @click="saveDialog = false">Cancel</v-btn>
				<v-btn color="primary" text :disabled="presetName.trim().length === 0" @click="confirmSavePreset()">Save</v-btn>
			</v-card-actions>
		</v-card>
	</v-dialog>
</div>

</template>

<script>
export default {
	props: ['Application', 'module'],

	directives: {
		focus: {
			inserted(el) {
				el.focus();
				if (typeof el.select === 'function')
					el.select();
			},
		},
	},

	data() {
		return {
			// Only one row is edited at a time. editValue is a local buffer so live
			// polling never clobbers what the user is typing.
			editingKey: null,
			editValue: '',
			// Bumped whenever the shared module's view state changes so that the
			// computed properties below re-evaluate (module is a plain JS object
			// outside Vue's reactivity graph).
			stateTick: 0,
			saveDialog: false,
			presetName: '',
		};
	},

	computed: {
		hasPreferences() {
			this.stateTick;
			return !!this.module.preferenceKey;
		},
		tableHeaders() {
			this.stateTick;
			const headers = this.module.headers || [];
			if (!this.hasPreferences)
				return headers;
			return [{ text: '', value: '__favorite', sortable: false, width: '48px' }].concat(headers);
		},
		viewEntries() {
			this.stateTick;
			const raw = this.module.entries || [];
			return (typeof this.module.applyView === 'function') ? this.module.applyView(raw) : raw.slice();
		},
		sortState() {
			this.stateTick;
			return this.module.sort || { column: '', direction: 'asc' };
		},
		showOnlyFavorites() {
			this.stateTick;
			return !!this.module.showOnlyFavorites;
		},
		groupOptions() {
			this.stateTick;
			return (typeof this.module.distinctGroups === 'function') ? this.module.distinctGroups() : [];
		},
		systemOptions() {
			this.stateTick;
			return (typeof this.module.distinctSystems === 'function') ? this.module.distinctSystems() : [];
		},
		activeGroups() {
			this.stateTick;
			return (this.module.filters && this.module.filters.groups) ? this.module.filters.groups : [];
		},
		activeSystems() {
			this.stateTick;
			return (this.module.filters && this.module.filters.systems) ? this.module.filters.systems : [];
		},
		presetNames() {
			this.stateTick;
			return (typeof this.module.listPresets === 'function') ? this.module.listPresets() : [];
		},
		filterSummary() {
			const parts = [];
			if (this.showOnlyFavorites)
				parts.push('Favorites only');
			if (this.activeGroups.length > 0)
				parts.push(this.activeGroups.length + ' group filter(s)');
			if (this.activeSystems.length > 0)
				parts.push(this.activeSystems.length + ' system filter(s)');
			return parts.join(' \u00b7 ');
		},
	},

	mounted() {
		this.module.onPreferencesChanged = () => { this.stateTick++; };
		// The restore is driven from the core module's polled update once the stable
		// key and auth token are available; this is just a best-effort early nudge.
		if (typeof this.module.maybeLoadPreferences === 'function')
			this.module.maybeLoadPreferences();
	},

	beforeDestroy() {
		if (this.module)
			this.module.onPreferencesChanged = null;
	},

	methods: {
		rowKey(item) {
			return (item.paramInstance || '') + '.' + (item.paramGroupName || '') + '.' + (item.paramName || '');
		},
		isEditable(item) {
			return !!this.module.editevent && (item.paramEditable === true || item.paramEditable === 'true');
		},
		isNumeric(item) {
			return item.paramType === 'integer' || item.paramType === 'double';
		},
		startEdit(item) {
			this.editingKey = this.rowKey(item);
			this.editValue = String(item.paramValue !== undefined && item.paramValue !== null ? item.paramValue : '');
		},
		cancelEdit() {
			this.editingKey = null;
		},
		acceptEdit(item) {
			const params = {
				instance: item.paramInstance || '',
				group: item.paramGroupName || '',
				parameter: item.paramName || '',
				value: String(this.editValue != null ? this.editValue : ''),
				type: item.paramType || '',
			};
			if (item.paramMin)
				params.min = String(item.paramMin);
			if (item.paramMax)
				params.max = String(item.paramMax);
			this.Application.triggerUIEvent(this.module.editevent, this.module.uuid, {}, undefined, params);
			this.editingKey = null;
		},

		// -- view-state actions (delegated to the shared core module) --
		isFavorite(item) {
			return (typeof this.module.isFavorite === 'function') ? this.module.isFavorite(item) : false;
		},
		toggleFavorite(item) {
			if (typeof this.module.toggleFavorite === 'function') this.module.toggleFavorite(item);
		},
		sortBy(h) {
			if (typeof this.module.toggleSort === 'function') this.module.toggleSort(h.value);
		},
		setShowOnlyFavorites(flag) {
			if (typeof this.module.setShowOnlyFavorites === 'function') this.module.setShowOnlyFavorites(flag);
		},
		toggleGroupFilter(group) {
			const active = this.activeGroups;
			const next = active.indexOf(group) >= 0
				? active.filter((g) => g !== group)
				: active.concat([group]);
			if (typeof this.module.setGroupFilter === 'function') this.module.setGroupFilter(next);
		},
		toggleSystemFilter(system) {
			const active = this.activeSystems;
			const next = active.indexOf(system) >= 0
				? active.filter((s) => s !== system)
				: active.concat([system]);
			if (typeof this.module.setSystemFilter === 'function') this.module.setSystemFilter(next);
		},
		clearFilters() {
			if (typeof this.module.clearFilters === 'function') this.module.clearFilters();
		},
		openSaveDialog() {
			this.presetName = this.module.activePreset || '';
			this.saveDialog = true;
		},
		confirmSavePreset() {
			const name = this.presetName.trim();
			if (name.length === 0) return;
			if (typeof this.module.savePreset === 'function') this.module.savePreset(name);
			this.saveDialog = false;
			this.presetName = '';
		},
		loadPreset(name) {
			if (typeof this.module.loadPreset === 'function') this.module.loadPreset(name);
		},
		deletePreset(name) {
			if (typeof this.module.deletePreset === 'function') this.module.deletePreset(name);
		},

		// Quote a CSV field only when needed, doubling embedded quotes (RFC 4180).
		csvEscape(value) {
			const s = String(value !== undefined && value !== null ? value : '');
			return /[",\r\n]/.test(s) ? '"' + s.replace(/"/g, '""') + '"' : s;
		},
		// Exports the full, unfiltered parameter set (description, value, state
		// machine, group) as a CSV download, regardless of the active view filters.
		downloadCsv() {
			const header = ['Parameter', 'Value', 'State Machine', 'Group'];
			const entries = this.module.entries || [];
			const rows = entries.map((e) =>
				[e.paramDescription, e.paramValue, e.paramSystem, e.paramGroup].map(this.csvEscape).join(','));
			const csv = [header.join(','), ...rows].join('\r\n');
			const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
			const url = URL.createObjectURL(blob);
			const a = document.createElement('a');
			a.href = url;
			a.download = (this.module.name || 'parameters') + '.csv';
			document.body.appendChild(a);
			a.click();
			document.body.removeChild(a);
			URL.revokeObjectURL(url);
		},
	},
};
</script>

<style scoped>
.plist-root {
	width: 100%;
	min-height: 0;
}

.plist-toolbar {
	display: flex;
	align-items: center;
	justify-content: flex-end;
	gap: 8px;
	padding: 2px 4px;
}

.plist-filter-summary {
	margin-right: auto;
	font-size: 0.75rem;
	color: rgba(0, 0, 0, 0.55);
}

.plist-fav-active {
	color: #f9a825 !important;
}

.plist-preset-name {
	cursor: pointer;
}

.plist-table {
	border: 1px solid rgba(0, 0, 0, 0.1);
	border-radius: 6px;
	overflow: hidden;
}

/* Override Vuetify table header */
.plist-table >>> thead th {
	font-size: 0.75rem !important;
	font-weight: 600 !important;
	color: rgba(0, 0, 0, 0.55) !important;
	text-transform: none !important;
	letter-spacing: 0 !important;
	border-bottom: 1px solid rgba(0, 0, 0, 0.1) !important;
	padding: 0 16px !important;
	height: 36px !important;
	background: #fafafa !important;
}

/* Body rows */
.plist-table >>> tbody td {
	font-size: 0.875rem !important;
	color: rgba(0, 0, 0, 0.87) !important;
	padding: 0 16px !important;
	height: 40px !important;
	border-bottom: 1px solid rgba(0, 0, 0, 0.06) !important;
}

.plist-table >>> tbody tr:last-child td {
	border-bottom: none !important;
}

.plist-table >>> tbody tr:hover {
	background: rgba(0, 0, 0, 0.03) !important;
}

/* No elevation */
.plist-table >>> .v-data-table__wrapper {
	box-shadow: none !important;
}

.plist-empty {
	padding: 24px;
	text-align: center;
	font-size: 0.875rem;
	font-style: italic;
	color: rgba(0, 0, 0, 0.38);
}

/* Inline editing */
.plist-value-display {
	cursor: pointer;
	border-bottom: 1px dotted rgba(0, 0, 0, 0.4);
}

.plist-value-display:hover {
	background: rgba(0, 0, 0, 0.04);
}

.plist-edit-wrap {
	display: inline-flex;
	align-items: center;
	gap: 4px;
}

.plist-edit-input {
	width: 6rem;
	padding: 2px 6px;
	font-size: 0.875rem;
	border: 1px solid rgba(0, 0, 0, 0.3);
	border-radius: 4px;
	background: #fff;
	color: rgba(0, 0, 0, 0.87);
}

.plist-icon-btn {
	cursor: pointer;
}

.plist-accept {
	color: #2e7d32 !important;
}

.plist-cancel {
	color: #c62828 !important;
}
</style>
