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
<div v-if="module.visible !== false" class="bl-root">

	<AMCModule_TableBase
		:columns="columns"
		:entries="module.entries"
		row-key="buildUUID"
		empty-icon="mdi-folder-open-outline"
		empty-title="No build files uploaded"
		empty-hint="Click a row to view details and select a build"
		@row-click="onRowClick"
	>
		<!-- Thumbnail -->
		<template #cell-buildThumbnail="{ item }">
			<img
				v-if="isValidUUID(item.buildThumbnail)"
				class="bl-thumb-img"
				:src="Application.getImageURL(item.buildThumbnail)"
			/>
			<div v-else class="bl-thumb-placeholder">
				<v-icon color="grey lighten-1" size="28">mdi-file-3d-outline</v-icon>
			</div>
		</template>

		<!-- Build name + execution count -->
		<template #cell-buildName="{ item }">
			<div class="bl-name-cell">
				<span class="bl-name-text">{{ item.buildName }}</span>
				<span class="bl-sub-text">
					{{ item.buildExecutionCount === 1 ? 'Printed 1 time' : 'Printed ' + item.buildExecutionCount + ' times' }}
				</span>
			</div>
		</template>

		<!-- Layers -->
		<template #cell-buildLayers="{ item }">
			<span class="bl-meta">{{ item.buildLayers }}</span>
		</template>

		<!-- Upload time + user -->
		<template #cell-buildTimestamp="{ item }">
			<div class="bl-date-cell">
				<span class="bl-date-text">{{ formatDateTime(item.buildTimestamp) }}</span>
				<span class="bl-sub-text">{{ item.buildUser }}</span>
			</div>
		</template>

		<!-- UUID -->
		<template #cell-buildUUID="{ item }">
			<span class="bl-uuid">{{ item.buildUUID }}</span>
		</template>

		<!-- Status -->
		<template #cell-buildStatus>
			<div class="bl-status-cell">
				<v-icon small color="green darken-1">mdi-check-circle</v-icon>
				<span class="bl-status-ready">Ready</span>
			</div>
		</template>

		<!-- Action buttons -->
		<template v-if="hasActions" #row-actions="{ item }">
			<v-btn
				v-for="button in module.entrybuttons"
				:key="button.uuid"
				text
				x-small
				color="primary"
				@click.stop="onButtonClick(button, item)"
			>
				<v-icon v-if="button.icon" small left>{{ button.icon }}</v-icon>
				{{ button.caption }}
			</v-btn>
		</template>
	</AMCModule_TableBase>

	<!-- Detail dialog -->
	<v-dialog v-model="showDialog" max-width="540" @click:outside="showDialog = false">
		<v-card v-if="selectedEntry">
			<v-card-title class="bl-dialog-title">
				<v-icon left color="primary">mdi-file-3d-outline</v-icon>
				Build details
				<v-spacer />
				<v-btn icon small @click="showDialog = false">
					<v-icon>mdi-close</v-icon>
				</v-btn>
			</v-card-title>
			<v-divider />

			<v-card-text class="bl-dialog-body pt-4">

				<div v-if="isValidUUID(selectedEntry.buildThumbnail)" class="bl-dialog-thumb-row mb-4">
					<v-img
						:src="Application.getImageURL(selectedEntry.buildThumbnail)"
						max-height="160"
						contain
						class="rounded"
					/>
				</div>

				<div class="bl-dialog-row">
					<span class="bl-dialog-label">File name</span>
					<span class="bl-dialog-value font-weight-medium">{{ selectedEntry.buildName }}</span>
				</div>
				<div class="bl-dialog-row">
					<span class="bl-dialog-label">Layers</span>
					<span class="bl-dialog-value">{{ selectedEntry.buildLayers }}</span>
				</div>
				<div class="bl-dialog-row">
					<span class="bl-dialog-label">Uploaded by</span>
					<span class="bl-dialog-value">{{ selectedEntry.buildUser }}</span>
				</div>
				<div class="bl-dialog-row">
					<span class="bl-dialog-label">Upload time</span>
					<span class="bl-dialog-value">{{ formatDateTime(selectedEntry.buildTimestamp) }}</span>
				</div>
				<div class="bl-dialog-row">
					<span class="bl-dialog-label">Executions</span>
					<span class="bl-dialog-value">{{ selectedEntry.buildExecutionCount }}</span>
				</div>
				<div class="bl-dialog-row bl-dialog-uuid-row">
					<span class="bl-dialog-label">UUID</span>
					<span class="bl-uuid-text">{{ selectedEntry.buildUUID }}</span>
				</div>

			</v-card-text>

			<v-divider />
			<v-card-actions>
				<v-spacer />
				<v-btn text @click="showDialog = false">Cancel</v-btn>
				<v-btn color="primary" @click="onSelectFromDialog">
					<v-icon left small>mdi-check</v-icon>
					Select for execution
				</v-btn>
			</v-card-actions>
		</v-card>
	</v-dialog>

</div>
</template>

<script>
import AMCModule_TableBase from './AMCModule_TableBase.vue';
import { formatDateTime, isValidUUID, triggerSelectEvent, triggerButtonEvent } from '../../../core/modules/AMCModule_TableUtils.js';

export default {
	props: ['Application', 'module'],

	components: { AMCModule_TableBase },

	data () {
		return {
			showDialog: false,
			selectedEntry: null,
			columns: [
				{ key: 'buildThumbnail', label: '',        width: '96px'  },
				{ key: 'buildName',      label: 'Build',   width: ''      },
				{ key: 'buildLayers',    label: 'Layers',  width: '80px'  },
				{ key: 'buildTimestamp', label: 'Uploaded',width: '160px' },
				{ key: 'buildUUID',      label: 'UUID',    width: '200px' },
				{ key: 'buildStatus',    label: 'Status',  width: '110px' },
			],
		};
	},

	computed: {
		hasActions () {
			return this.module.entrybuttons && this.module.entrybuttons.length > 0;
		},
	},

	methods: {
		formatDateTime,
		isValidUUID,

		onRowClick (item) {
			this.selectedEntry = item;
			this.showDialog = true;
		},

		onSelectFromDialog () {
			this.showDialog = false;
			triggerSelectEvent(this.Application, this.module, this.selectedEntry && this.selectedEntry.buildUUID);
		},

		onButtonClick (button, item) {
			triggerButtonEvent(this.Application, this.module, button, item.buildUUID);
		},
	},
};
</script>

<style scoped>
/* ── Root ────────────────────────────────────────────────── */
.bl-root {
	width: 100%;
	min-height: 0;
}

/* ── Name cell ───────────────────────────────────────────── */
.bl-name-cell {
	display: flex;
	flex-direction: column;
	gap: 2px;
}

.bl-name-text {
	font-weight: 500;
	font-size: 0.875rem;
	white-space: nowrap;
	overflow: hidden;
	text-overflow: ellipsis;
}

.bl-sub-text {
	font-size: 0.75rem;
	color: rgba(0, 0, 0, 0.45);
	font-style: italic;
}

/* ── Thumbnail ───────────────────────────────────────────── */
.bl-thumb-img {
	display: block;
	width: 72px;
	height: 52px;
	object-fit: contain;
	border-radius: 4px;
	background: #f5f5f5;
}

.bl-thumb-placeholder {
	width: 72px;
	height: 52px;
	display: flex;
	align-items: center;
	justify-content: center;
	background: #f5f5f5;
	border-radius: 4px;
}

/* ── Meta / layers ───────────────────────────────────────── */
.bl-meta {
	font-size: 0.8125rem;
	color: rgba(0, 0, 0, 0.55);
}

/* ── Date cell ───────────────────────────────────────────── */
.bl-date-cell {
	display: flex;
	flex-direction: column;
	gap: 2px;
}

.bl-date-text {
	font-size: 0.8125rem;
}

/* ── UUID ────────────────────────────────────────────────── */
.bl-uuid {
	font-size: 0.7rem;
	font-family: 'Roboto Mono', monospace;
	color: rgba(0, 0, 0, 0.38);
	letter-spacing: 0.01em;
}

/* ── Status ──────────────────────────────────────────────── */
.bl-status-cell {
	display: flex;
	align-items: center;
	gap: 4px;
}

.bl-status-ready {
	font-size: 0.8125rem;
	color: #2e7d32;
	font-weight: 500;
}

/* ── Dialog ──────────────────────────────────────────────── */
.bl-dialog-title {
	font-size: 1rem !important;
	padding: 14px 20px !important;
}

.bl-dialog-body {
	padding: 16px 20px !important;
}

.bl-dialog-thumb-row {
	border-radius: 6px;
	overflow: hidden;
	background: #f5f5f5;
}

.bl-dialog-row {
	display: flex;
	align-items: baseline;
	gap: 12px;
	padding: 5px 0;
	border-bottom: 1px solid rgba(0, 0, 0, 0.06);
}

.bl-dialog-row:last-child {
	border-bottom: none;
}

.bl-dialog-label {
	font-size: 0.8125rem;
	font-weight: 500;
	color: rgba(0, 0, 0, 0.55);
	min-width: 140px;
	flex-shrink: 0;
}

.bl-dialog-value {
	font-size: 0.875rem;
}

.bl-uuid-text {
	font-size: 0.7rem;
	font-family: 'Roboto Mono', monospace;
	color: rgba(0, 0, 0, 0.38);
	letter-spacing: 0.01em;
	word-break: break-all;
}
</style>
