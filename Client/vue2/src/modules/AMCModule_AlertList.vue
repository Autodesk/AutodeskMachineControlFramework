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
<div v-if="module.visible !== false" class="al-root">

	<AMCModule_TableBase
		:columns="columns"
		:entries="module.entries"
		row-key="alertuuid"
		empty-icon="mdi-bell-off-outline"
		empty-title="No alerts"
		@row-click="onRowClick"
	>
		<!-- Timestamp -->
		<template #cell-alerttimestamp="{ item }">
			{{ formatTime(item.alerttimestamp) }}
		</template>

		<!-- Caption + identifier -->
		<template #cell-alertcaption="{ item }">
			<div class="al-caption-cell">
				<span class="al-caption-text">{{ item.alertcaption }}</span>
				<span v-if="item.alertidentifier" class="al-sub-text">{{ item.alertidentifier }}</span>
			</div>
		</template>

		<!-- Context -->
		<template #cell-alertcontext="{ item }">
			<span class="al-context">{{ item.alertcontext }}</span>
		</template>

		<!-- Severity badge -->
		<template #cell-severity="{ item }">
			<span
				v-if="item.severity || item.alertlevel"
				class="al-badge"
				:class="severityClass(item.severity || item.alertlevel)"
			>{{ item.severity || item.alertlevel }}</span>
		</template>

		<!-- Active indicator -->
		<template #cell-alertactive="{ item }">
			<v-icon v-if="item.alertactive" small color="red darken-1">mdi-alert-circle</v-icon>
			<v-icon v-else small color="grey lighten-1">mdi-check-circle-outline</v-icon>
		</template>
	</AMCModule_TableBase>

</div>
</template>

<script>
import AMCModule_TableBase from './AMCModule_TableBase.vue';
import { formatTime, SEVERITY_CLASSES, triggerSelectEvent } from '../../../core/modules/AMCModule_TableUtils.js';

export default {
	props: ['Application', 'module'],

	components: { AMCModule_TableBase },

	data () {
		return {
			columns: [
				{ key: 'alerttimestamp', label: 'Time',    width: '110px' },
				{ key: 'alertcaption',   label: 'Alert',   width: ''      },
				{ key: 'alertcontext',   label: 'Context', width: '200px' },
				{ key: 'severity',       label: 'Level',   width: '110px' },
				{ key: 'alertactive',    label: 'Active',  width: '70px', align: 'center' },
			],
		};
	},

	methods: {
		formatTime,

		severityClass (level) {
			return 'al-badge--' + (SEVERITY_CLASSES[(level || '').toLowerCase()] || 'badge--slate').replace('badge--', '');
		},

		onRowClick (item) {
			triggerSelectEvent(this.Application, this.module, item.alertuuid);
		},
	},
};
</script>

<style scoped>
.al-root {
	width: 100%;
	min-height: 0;
}

/* ── Caption cell ────────────────────────────────────────── */
.al-caption-cell {
	display: flex;
	flex-direction: column;
	gap: 2px;
}

.al-caption-text {
	font-weight: 500;
	font-size: 0.875rem;
	color: rgba(0, 0, 0, 0.87);
}

.al-sub-text {
	font-size: 0.75rem;
	color: rgba(0, 0, 0, 0.45);
	font-style: italic;
}

/* ── Context ─────────────────────────────────────────────── */
.al-context {
	font-size: 0.8125rem;
	color: rgba(0, 0, 0, 0.55);
}

/* ── Severity badge ──────────────────────────────────────── */
.al-badge {
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

.al-badge--red    { color: #dc2626; }
.al-badge--yellow { color: #ca8a04; }
.al-badge--blue   { color: #2563eb; }
.al-badge--green  { color: #16a34a; }
.al-badge--slate  { color: rgba(0, 0, 0, 0.45); }
</style>
