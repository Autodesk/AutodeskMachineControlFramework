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

	<div v-if="(module.type == 'logs') && module.visible !== false" class="logs-module-root">

		<!-- Vertical toolbar -->
		<div v-if="module.showToolbar" class="logs-toolbar">

			<!-- Scroll to bottom -->
			<v-tooltip right>
				<template v-slot:activator="{ on }">
					<v-btn icon small class="logs-toolbar-btn" v-on="on" @click="scrollToBottomManual">
						<v-icon small>mdi-arrow-collapse-down</v-icon>
					</v-btn>
				</template>
				<span>Scroll to bottom</span>
			</v-tooltip>

			<!-- Count limiter -->
			<v-menu offset-x left>
				<template v-slot:activator="{ on }">
					<v-tooltip right>
						<template v-slot:activator="{ on: tip }">
							<v-btn icon small class="logs-toolbar-btn" v-on="{ ...on, ...tip }">
								<v-icon small>mdi-format-list-numbered</v-icon>
							</v-btn>
						</template>
						<span>Limit shown entries ({{ effectiveLimit }})</span>
					</v-tooltip>
				</template>
				<v-list dense>
					<v-list-item
						v-for="opt in countOptions"
						:key="opt.value"
						@click="displayLimit = opt.value"
						:class="{ 'v-list-item--active': effectiveLimit === opt.value }"
					>
						<v-list-item-title>{{ opt.label }}</v-list-item-title>
					</v-list-item>
				</v-list>
			</v-menu>

			<!-- Subsystem filter -->
			<v-menu offset-x left :close-on-content-click="true">
				<template v-slot:activator="{ on }">
					<v-tooltip right>
						<template v-slot:activator="{ on: tip }">
							<v-badge
								:value="subsystemFilter !== null"
								color="primary"
								dot
								overlap
							>
								<v-btn icon small class="logs-toolbar-btn" v-on="{ ...on, ...tip }">
									<v-icon small>mdi-filter-variant</v-icon>
								</v-btn>
							</v-badge>
						</template>
						<span>{{ subsystemFilter ? 'Filter: ' + subsystemFilter : 'Filter by subsystem' }}</span>
					</v-tooltip>
				</template>
				<v-list dense>
					<v-list-item @click="subsystemFilter = null">
						<v-list-item-icon class="mr-2">
							<v-icon small :color="subsystemFilter === null ? 'primary' : ''">mdi-filter-off-outline</v-icon>
						</v-list-item-icon>
						<v-list-item-title>Clear filter</v-list-item-title>
					</v-list-item>
					<v-divider />
					<v-list-item
						v-for="sys in availableSubsystems"
						:key="sys"
						@click="subsystemFilter = sys"
						:class="{ 'v-list-item--active': subsystemFilter === sys }"
					>
						<v-list-item-title>{{ sys }}</v-list-item-title>
					</v-list-item>
					<v-list-item v-if="availableSubsystems.length === 0" disabled>
						<v-list-item-title class="grey--text">No entries</v-list-item-title>
					</v-list-item>
				</v-list>
			</v-menu>

			<!-- Download -->
			<v-menu offset-x left>
				<template v-slot:activator="{ on }">
					<v-tooltip right>
						<template v-slot:activator="{ on: tip }">
							<v-btn icon small class="logs-toolbar-btn" v-on="{ ...on, ...tip }">
								<v-icon small>mdi-download</v-icon>
							</v-btn>
						</template>
						<span>Download log</span>
					</v-tooltip>
				</template>
				<v-list dense>
					<v-list-item @click="downloadJSON(visibleItems, 'view')">
						<v-list-item-icon class="mr-2"><v-icon small>mdi-code-json</v-icon></v-list-item-icon>
						<v-list-item-title>Current view – JSON</v-list-item-title>
					</v-list-item>
					<v-list-item @click="downloadJSON(module.DisplayItems, 'all')">
						<v-list-item-icon class="mr-2"><v-icon small>mdi-code-json</v-icon></v-list-item-icon>
						<v-list-item-title>All historic – JSON</v-list-item-title>
					</v-list-item>
					<v-divider />
					<v-list-item @click="downloadCSV(visibleItems, 'view')">
						<v-list-item-icon class="mr-2"><v-icon small>mdi-file-delimited-outline</v-icon></v-list-item-icon>
						<v-list-item-title>Current view – CSV</v-list-item-title>
					</v-list-item>
					<v-list-item @click="downloadCSV(module.DisplayItems, 'all')">
						<v-list-item-icon class="mr-2"><v-icon small>mdi-file-delimited-outline</v-icon></v-list-item-icon>
						<v-list-item-title>All historic – CSV</v-list-item-title>
					</v-list-item>
				</v-list>
			</v-menu>

		</div>

		<!-- Log area: sticky header + scrollable body -->
		<div class="logs-area">

			<div class="logs-header">
				<table class="logs-table">
					<colgroup>
						<col style="width:110px" />
						<col style="width:130px" />
						<col />
					</colgroup>
					<thead>
						<tr>
							<th>Time</th>
							<th>Subsystem</th>
							<th>Text</th>
						</tr>
					</thead>
				</table>
			</div>

			<div ref="logScroll" class="logs-scroll" @scroll="onUserScroll">
				<table class="logs-table">
					<colgroup>
						<col style="width:110px" />
						<col style="width:130px" />
						<col />
					</colgroup>
					<tbody>
						<tr v-if="hiddenCount > 0" class="logs-row-hidden-notice">
							<td colspan="3" class="logs-cell-hidden-notice">
								{{ hiddenCount }} previous {{ hiddenCount === 1 ? 'entry' : 'entries' }} not shown
							</td>
						</tr>
						<tr v-for="entry in visibleItems" :key="entry.logIndex">
							<td class="logs-cell-time">{{ entry.logTime }}</td>
							<td class="logs-cell-subsystem">{{ entry.logSubsystem }}</td>
							<td class="logs-cell-text">{{ entry.logText }}</td>
						</tr>
					</tbody>
				</table>
			</div>

		</div>

	</div>

</template>

<script>

	export default {
		props: ["Application", "module"],

		components: {
		},

		data: () => ({
			userScrolledUp: false,
			displayLimit: null,
			subsystemFilter: null
		}),

		computed: {
			effectiveLimit () {
				return this.displayLimit !== null ? this.displayLimit : (this.module.defaultCount || 200);
			},

			countOptions () {
				let opts = [10, 20, 50, 100].map(n => ({ value: n, label: String(n) }));
				let def = this.module.defaultCount || 200;
				if (![10, 20, 50, 100].includes(def)) {
					opts.push({ value: def, label: 'Default (' + def + ')' });
				} else {
					opts.find(o => o.value === def).label += ' (default)';
				}
				return opts;
			},

			availableSubsystems () {
				let set = new Set(this.module.DisplayItems.map(e => e.logSubsystem));
				return [...set].filter(Boolean).sort();
			},

			filteredItems () {
				if (!this.subsystemFilter)
					return this.module.DisplayItems;
				return this.module.DisplayItems.filter(e => e.logSubsystem === this.subsystemFilter);
			},

			visibleItems () {
				let limit = this.effectiveLimit;
				let items = this.filteredItems;
				if (items.length <= limit)
					return items;
				return items.slice(items.length - limit);
			},

			hiddenCount () {
				let excess = this.filteredItems.length - this.effectiveLimit;
				return excess > 0 ? excess : 0;
			}
		},

		watch: {
			'module.DisplayItems.length' () {
				if (!this.userScrolledUp) {
					this.$nextTick(() => { this.scrollToBottom(); });
				}
			}
		},

		mounted () {
			this.$nextTick(() => { this.scrollToBottom(); });
		},

		methods: {
			scrollToBottom () {
				let el = this.$refs.logScroll;
				if (el)
					el.scrollTop = el.scrollHeight;
			},

			scrollToBottomManual () {
				this.userScrolledUp = false;
				this.scrollToBottom();
			},

			onUserScroll () {
				let el = this.$refs.logScroll;
				if (!el) return;
				let distFromBottom = el.scrollHeight - el.scrollTop - el.clientHeight;
				this.userScrolledUp = distFromBottom > 40;
			},

			downloadTimestamp () {
				let now = new Date();
				let pad = n => String(n).padStart(2, '0');
				return now.getFullYear()
					+ pad(now.getMonth() + 1)
					+ pad(now.getDate())
					+ '_'
					+ pad(now.getHours())
					+ pad(now.getMinutes())
					+ pad(now.getSeconds());
			},

			buildFilename (scope, extension) {
				let prefix = (this.module.downloadPrefix || 'log').replace(/[^a-zA-Z0-9_-]/g, '_');
				return prefix + '_' + scope + '_' + this.downloadTimestamp() + '.' + extension;
			},

			downloadJSON (items, scope) {
				let data = items.map(e => ({
					time: e.logTime,
					subsystem: e.logSubsystem,
					text: e.logText
				}));
				let content = JSON.stringify(data, null, 2);
				this.triggerDownload(content, 'application/json', this.buildFilename(scope, 'json'));
			},

			downloadCSV (items, scope) {
				let lines = ['Time,Subsystem,Text'];
				for (let e of items) {
					let time = this.csvEscape(e.logTime);
					let sub  = this.csvEscape(e.logSubsystem);
					let text = this.csvEscape(e.logText);
					lines.push(time + ',' + sub + ',' + text);
				}
				this.triggerDownload(lines.join('\r\n'), 'text/csv', this.buildFilename(scope, 'csv'));
			},

			csvEscape (value) {
				let s = (value === undefined || value === null) ? '' : String(value);
				if (s.includes('"') || s.includes(',') || s.includes('\n') || s.includes('\r')) {
					return '"' + s.replace(/"/g, '""') + '"';
				}
				return s;
			},

			triggerDownload (content, mimeType, filename) {
				let blob = new Blob([content], { type: mimeType });
				let url  = URL.createObjectURL(blob);
				let a    = document.createElement('a');
				a.href     = url;
				a.download = filename;
				document.body.appendChild(a);
				a.click();
				document.body.removeChild(a);
				URL.revokeObjectURL(url);
			}
		}
	};

</script>

<style scoped>
.logs-module-root {
	width: 100%;
	height: 100%;
	padding: 10px;
	display: flex;
	flex-direction: row;
	overflow: hidden;
	gap: 4px;
}

.logs-toolbar {
	flex: 0 0 auto;
	width: 36px;
	display: flex;
	flex-direction: column;
	align-items: center;
	padding-top: 2px;
	gap: 2px;
}

.logs-toolbar-btn {
	width: 32px !important;
	height: 32px !important;
}

.logs-area {
	flex: 1 1 0;
	display: flex;
	flex-direction: column;
	overflow: hidden;
	min-width: 0;
}

.logs-header {
	flex: 0 0 auto;
	border: 1px solid rgba(0, 0, 0, 0.1);
	border-bottom: none;
	border-radius: 6px 6px 0 0;
	overflow: hidden;
}

.logs-scroll {
	flex: 1 1 0;
	overflow-y: auto;
	overflow-x: hidden;
	border: 1px solid rgba(0, 0, 0, 0.1);
	border-top: none;
	border-radius: 0 0 6px 6px;
}

.logs-table {
	width: 100%;
	border-collapse: collapse;
	table-layout: fixed;
}

.logs-table thead th {
	position: sticky;
	top: 0;
	background: #fafafa;
	text-align: left;
	padding: 4px 8px;
	font-size: 0.75rem;
	font-weight: 600;
	border-bottom: 1px solid rgba(0, 0, 0, 0.1);
	color: rgba(0, 0, 0, 0.55);
	text-transform: none;
	letter-spacing: 0;
}

.logs-table tbody tr:hover {
	background: rgba(0, 0, 0, 0.03);
}

.logs-table td {
	padding: 2px 8px;
	font-size: 0.75rem;
	font-family: 'Roboto Mono', monospace;
	white-space: nowrap;
	overflow: hidden;
	text-overflow: ellipsis;
	border-bottom: 1px solid rgba(0, 0, 0, 0.06);
}

.logs-cell-time {
	color: rgba(0, 0, 0, 0.45);
}

.logs-cell-subsystem {
	color: rgba(0, 0, 0, 0.45);
}

.logs-cell-text {
	white-space: normal;
	word-break: break-word;
	color: rgba(0, 0, 0, 0.87);
}

.logs-row-hidden-notice {
	background: transparent;
}

.logs-cell-hidden-notice {
	padding: 4px 8px;
	font-size: 0.75rem;
	font-family: 'Roboto', sans-serif;
	color: rgba(0, 0, 0, 0.35);
	text-align: center;
	font-style: italic;
	border-bottom: 1px solid rgba(0, 0, 0, 0.08);
	white-space: nowrap;
}
</style>
