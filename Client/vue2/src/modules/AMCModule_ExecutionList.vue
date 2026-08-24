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
<div v-if="module.visible !== false" class="el-root">

	<AMCModule_TableBase
		:columns="visibleColumns"
		:entries="module.entries"
		row-key="executionUUID"
		empty-icon="mdi-history"
		:empty-title="module.loadingtext || 'No executions'"
		empty-hint="Build executions will appear here"
		@row-click="onRowClick"
	>
		<!-- Thumbnail -->
		<template v-if="hasThumbnailColumn" #cell-executionThumbnail="{ item }">
			<img
				v-if="isValidUUID(item.executionThumbnail)"
				:src="Application.getImageURL(item.executionThumbnail)"
				class="el-thumb-img"
				:alt="item.executionName"
			/>
			<div v-else class="el-thumb-placeholder">
				<v-icon color="grey lighten-2" size="28">mdi-cube-outline</v-icon>
			</div>
		</template>

		<!-- Name + description -->
		<template #cell-executionName="{ item }">
			<div class="el-name-text">{{ item.executionName }}</div>
			<div v-if="item.executionDescription" class="el-sub-text">{{ item.executionDescription }}</div>
			<div v-if="item.executionLayerCount" class="el-sub-text">{{ item.executionLayerCount }} layers</div>
		</template>

		<!-- Timestamp block -->
		<template #cell-executionStartTimestamp="{ item }">
			<div v-if="item.executionStartTimestamp" class="el-time-row">
				<span class="el-time-label">Start</span>
				<span class="el-time-value">{{ formatDateTime(item.executionStartTimestamp) }}</span>
			</div>
			<div v-if="item.executionEndTimestamp" class="el-time-row">
				<span class="el-time-label">End</span>
				<span class="el-time-value">{{ formatDateTime(item.executionEndTimestamp) }}</span>
			</div>
			<div v-if="item.executionDuration != null" class="el-time-row">
				<span class="el-time-label">Dur</span>
				<span class="el-time-value el-duration">{{ formatDuration(item.executionDuration) }}</span>
			</div>
		</template>

		<!-- Status badge -->
		<template #cell-executionStatus="{ item }">
			<span v-if="item.executionStatus" class="el-status-badge" :class="statusClass(item.executionStatus)">
				{{ item.executionStatus }}
			</span>
		</template>

		<!-- Action buttons -->
		<template v-if="hasButtons" #row-actions="{ item }">
			<button
				v-for="button in module.entrybuttons"
				:key="button.uuid"
				class="el-action-btn"
				@click.stop="onButtonClick(button, item)"
			>
				<v-icon v-if="button.icon" small>{{ button.icon }}</v-icon>
				<span v-if="button.caption">{{ button.caption }}</span>
			</button>
		</template>
	</AMCModule_TableBase>

	<!-- Detail dialog -->
	<v-dialog v-if="selectedItem" v-model="dialogOpen" max-width="520" @click:outside="closeDialog">
		<v-card class="el-dialog-card">
			<div class="el-dialog-title">Execution Details</div>

			<div v-if="isValidUUID(selectedItem.executionThumbnail)" class="el-dialog-thumb-row">
				<img
					:src="Application.getImageURL(selectedItem.executionThumbnail)"
					class="el-dialog-thumb-img"
					:alt="selectedItem.executionName"
				/>
			</div>

			<div class="el-dialog-body">
				<div class="el-dialog-row">
					<span class="el-dialog-label">Name</span>
					<span class="el-dialog-value">{{ selectedItem.executionName }}</span>
				</div>
				<div v-if="selectedItem.executionDescription" class="el-dialog-row">
					<span class="el-dialog-label">Description</span>
					<span class="el-dialog-value">{{ selectedItem.executionDescription }}</span>
				</div>
				<div v-if="selectedItem.executionStatus" class="el-dialog-row">
					<span class="el-dialog-label">Status</span>
					<span class="el-dialog-value">
						<span class="el-status-badge" :class="statusClass(selectedItem.executionStatus)">
							{{ selectedItem.executionStatus }}
						</span>
					</span>
				</div>
				<div v-if="selectedItem.executionLayerCount" class="el-dialog-row">
					<span class="el-dialog-label">Layers</span>
					<span class="el-dialog-value">{{ selectedItem.executionLayerCount }}</span>
				</div>
				<div v-if="selectedItem.executionStartTimestamp" class="el-dialog-row">
					<span class="el-dialog-label">Started</span>
					<span class="el-dialog-value">{{ formatDateTime(selectedItem.executionStartTimestamp) }}</span>
				</div>
				<div v-if="selectedItem.executionEndTimestamp" class="el-dialog-row">
					<span class="el-dialog-label">Finished</span>
					<span class="el-dialog-value">{{ formatDateTime(selectedItem.executionEndTimestamp) }}</span>
				</div>
				<div v-if="selectedItem.executionDuration != null" class="el-dialog-row">
					<span class="el-dialog-label">Duration</span>
					<span class="el-dialog-value">{{ formatDuration(selectedItem.executionDuration) }}</span>
				</div>
				<div v-if="selectedItem.executionUUID" class="el-dialog-row">
					<span class="el-dialog-label">UUID</span>
					<span class="el-dialog-value el-dialog-uuid">{{ selectedItem.executionUUID }}</span>
				</div>
			</div>

			<div v-if="hasButtons" class="el-dialog-actions">
				<button
					v-for="button in module.entrybuttons"
					:key="button.uuid"
					class="el-dialog-action-btn"
					@click="onButtonClick(button, selectedItem); closeDialog()"
				>
					<v-icon v-if="button.icon" small>{{ button.icon }}</v-icon>
					{{ button.caption }}
				</button>
			</div>

			<div class="el-dialog-close">
				<button class="el-close-btn" @click="closeDialog">Close</button>
			</div>
		</v-card>
	</v-dialog>

</div>
</template>

<script>
import AMCModule_TableBase from './AMCModule_TableBase.vue';
import { formatDateTime, formatDuration, STATUS_CLASSES, isValidUUID, triggerSelectEvent, triggerButtonEvent } from '../../../core/modules/AMCModule_TableUtils.js';

export default {
	props: ['Application', 'module'],

	components: { AMCModule_TableBase },

	data () {
		return {
			dialogOpen: false,
			selectedItem: null,
		};
	},

	computed: {
		hasThumbnailColumn () {
			return this.module.entries && this.module.entries.some(e => isValidUUID(e.executionThumbnail));
		},

		hasButtons () {
			return this.module.entrybuttons && this.module.entrybuttons.length > 0;
		},

		visibleColumns () {
			const cols = [];
			if (this.hasThumbnailColumn)
				cols.push({ key: 'executionThumbnail',      label: '',         width: '112px' });
			cols.push({ key: 'executionName',            label: 'Execution', width: ''      });
			cols.push({ key: 'executionStartTimestamp',  label: 'Time',      width: '180px' });
			cols.push({ key: 'executionStatus',          label: 'Status',    width: '110px' });
			return cols;
		},
	},

	methods: {
		formatDateTime,
		formatDuration,
		isValidUUID,

		statusClass (status) {
			const key = (status || '').toLowerCase();
			const cls = STATUS_CLASSES[key] || 'badge--slate';
			return 'el-status--' + cls.replace('badge--', '');
		},

		onRowClick (item) {
			this.selectedItem = item;
			this.dialogOpen = true;
			triggerSelectEvent(this.Application, this.module, item.executionUUID);
		},

		onButtonClick (button, item) {
			triggerButtonEvent(this.Application, this.module, button, item.executionUUID);
		},

		closeDialog () {
			this.dialogOpen = false;
			this.selectedItem = null;
		},
	},
};
</script>

<style scoped>
.el-root {
	width: 100%;
	min-height: 0;
	display: flex;
	flex-direction: column;
}

/* ── Name cell ───────────────────────────────────────────── */
.el-name-text {
	font-weight: 500;
	font-size: 0.875rem;
	color: rgba(0, 0, 0, 0.87);
	white-space: nowrap;
	overflow: hidden;
	text-overflow: ellipsis;
}

.el-sub-text {
	font-size: 0.75rem;
	color: rgba(0, 0, 0, 0.45);
	margin-top: 2px;
	white-space: nowrap;
	overflow: hidden;
	text-overflow: ellipsis;
}

/* ── Thumbnail ───────────────────────────────────────────── */
.el-thumb-img {
	width: 96px;
	height: 72px;
	object-fit: contain;
	display: block;
	border-radius: 4px;
	background: #f5f5f5;
}

.el-thumb-placeholder {
	width: 96px;
	height: 72px;
	display: flex;
	align-items: center;
	justify-content: center;
	background: #f5f5f5;
	border-radius: 4px;
}

/* ── Time block ──────────────────────────────────────────── */
.el-time-row {
	display: flex;
	gap: 6px;
	align-items: baseline;
	line-height: 1.6;
}

.el-time-label {
	font-size: 0.7rem;
	font-weight: 600;
	color: rgba(0, 0, 0, 0.38);
	text-transform: uppercase;
	letter-spacing: 0.03em;
	min-width: 28px;
	flex-shrink: 0;
}

.el-time-value {
	font-family: 'Roboto Mono', monospace;
	font-size: 0.75rem;
	color: rgba(0, 0, 0, 0.6);
	white-space: nowrap;
}

.el-duration {
	color: rgba(0, 0, 0, 0.87);
	font-weight: 500;
}

/* ── Status badge ────────────────────────────────────────── */
.el-status-badge {
	display: inline-flex;
	align-items: center;
	padding: 1px 8px;
	border-radius: 9999px;
	font-size: 0.7rem;
	font-weight: 600;
	border: 2px solid currentColor;
	text-transform: capitalize;
	white-space: nowrap;
}

.el-status--blue   { color: #2563eb; }
.el-status--green  { color: #16a34a; }
.el-status--red    { color: #dc2626; }
.el-status--yellow { color: #ca8a04; }
.el-status--slate  { color: rgba(0, 0, 0, 0.45); }
.el-status--purple { color: #7c3aed; }

/* ── Action button ───────────────────────────────────────── */
.el-action-btn {
	display: inline-flex;
	align-items: center;
	gap: 4px;
	padding: 3px 10px;
	border: 1px solid rgba(0, 0, 0, 0.15);
	border-radius: 4px;
	background: transparent;
	cursor: pointer;
	font-size: 0.8125rem;
	font-weight: 500;
	color: rgba(0, 0, 0, 0.7);
	margin-right: 6px;
	transition: background 0.15s;
}

.el-action-btn:hover {
	background: rgba(0, 0, 0, 0.05);
	border-color: rgba(0, 0, 0, 0.3);
}

/* ── Dialog ──────────────────────────────────────────────── */
.el-dialog-card {
	border-radius: 8px !important;
	box-shadow: 0 4px 24px rgba(0, 0, 0, 0.12) !important;
	overflow: hidden;
}

.el-dialog-title {
	font-size: 1rem;
	font-weight: 600;
	color: rgba(0, 0, 0, 0.87);
	padding: 20px 24px 0;
}

.el-dialog-thumb-row {
	padding: 16px 24px 0;
	display: flex;
	justify-content: center;
}

.el-dialog-thumb-img {
	max-width: 100%;
	max-height: 200px;
	object-fit: contain;
	border-radius: 6px;
	background: #f5f5f5;
}

.el-dialog-body {
	padding: 16px 24px;
	display: flex;
	flex-direction: column;
	gap: 10px;
}

.el-dialog-row {
	display: flex;
	gap: 12px;
	align-items: flex-start;
}

.el-dialog-label {
	font-size: 0.75rem;
	font-weight: 600;
	color: rgba(0, 0, 0, 0.45);
	text-transform: uppercase;
	letter-spacing: 0.03em;
	min-width: 72px;
	flex-shrink: 0;
	padding-top: 2px;
}

.el-dialog-value {
	font-size: 0.875rem;
	color: rgba(0, 0, 0, 0.87);
}

.el-dialog-uuid {
	font-family: 'Roboto Mono', monospace;
	font-size: 0.75rem;
	color: rgba(0, 0, 0, 0.45);
	word-break: break-all;
}

.el-dialog-actions {
	padding: 0 24px 8px;
	display: flex;
	gap: 8px;
	flex-wrap: wrap;
}

.el-dialog-action-btn {
	display: inline-flex;
	align-items: center;
	gap: 5px;
	padding: 6px 14px;
	border: 1px solid rgba(0, 0, 0, 0.15);
	border-radius: 4px;
	background: transparent;
	cursor: pointer;
	font-size: 0.875rem;
	font-weight: 500;
	color: rgba(0, 0, 0, 0.7);
	transition: background 0.15s;
}

.el-dialog-action-btn:hover {
	background: rgba(0, 0, 0, 0.05);
	border-color: rgba(0, 0, 0, 0.3);
}

.el-dialog-close {
	padding: 8px 24px 20px;
	display: flex;
	justify-content: flex-end;
}

.el-close-btn {
	padding: 6px 16px;
	border-radius: 4px;
	border: 1px solid rgba(0, 0, 0, 0.15);
	background: transparent;
	cursor: pointer;
	font-size: 0.875rem;
	font-weight: 500;
	color: rgba(0, 0, 0, 0.6);
	transition: background 0.15s;
}

.el-close-btn:hover {
	background: rgba(0, 0, 0, 0.05);
}
</style>
