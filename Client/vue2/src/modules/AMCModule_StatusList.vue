<!--

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

!-->

<template>

<div v-if="module.visible !== false" class="amcf-status-list">
	<div v-if="module.title" class="amcf-status-list__title">{{ module.title }}</div>
	<div v-if="(module.rows || []).length === 0" class="amcf-status-list__empty">
		{{ module.loadingtext || 'No status available.' }}
	</div>
	<div
		v-for="(row, index) in (module.rows || [])"
		:key="index"
		class="amcf-status-list__row"
	>
		<span class="amcf-status-list__label">{{ row.label }}</span>
		<span
			v-if="renderRow(row).pill"
			class="amcf-status-list__pill"
			:class="'amcf-status-list__pill--' + renderRow(row).severity"
		>{{ renderRow(row).text }}</span>
		<span v-else class="amcf-status-list__value">{{ renderRow(row).text }}</span>
	</div>
</div>

</template>

<script>
export default {
	props: ['Application', 'module'],

	methods: {
		isTruthy (value) {
			const v = (value === undefined || value === null ? '' : value).toString().trim().toLowerCase();
			return !(v === '' || v === '0' || v === 'false' || v === 'off' || v === 'no');
		},

		renderRow (row) {
			const value = (row.value === undefined || row.value === null ? '' : row.value).toString();
			const display = (row.display || 'text').toString();

			if (display === 'number') {
				const unit = row.unit ? (' ' + row.unit) : '';
				return { pill: false, text: value + unit, severity: 'neutral' };
			}
			if (display === 'text') {
				return { pill: false, text: value, severity: 'neutral' };
			}
			if (display === 'map') {
				const maps = row.maps || [];
				const match = maps.find((m) => (m.value === undefined || m.value === null ? '' : m.value).toString() === value);
				if (match)
					return { pill: true, text: match.text || value, severity: match.severity || 'neutral' };
				return { pill: true, text: value, severity: 'neutral' };
			}

			const on = this.isTruthy(value);
			if (display === 'yesno')
				return { pill: true, text: on ? 'Yes' : 'No', severity: on ? 'success' : 'neutral' };
			if (display === 'openclosed')
				return { pill: true, text: on ? 'Opened' : 'Closed', severity: on ? 'success' : 'neutral' };
			if (display === 'leftright')
				return { pill: true, text: on ? 'Right' : 'Left', severity: 'neutral' };
			return { pill: true, text: on ? 'On' : 'Off', severity: on ? 'success' : 'neutral' };
		}
	}
};
</script>

<style scoped>
.amcf-status-list {
	display: flex;
	flex-direction: column;
	width: 100%;
	min-width: 0;
	font-family: var(--amcf-font-family, "Segoe UI", sans-serif);
}

.amcf-status-list__title {
	font-size: 12.5px;
	font-weight: 600;
	color: var(--amcf-color-text-secondary, #9aa2ab);
	margin-bottom: 4px;
}

.amcf-status-list__empty {
	font-size: 14px;
	color: var(--amcf-color-text-secondary, #9aa2ab);
	padding: 8px 0;
}

.amcf-status-list__row {
	display: flex;
	align-items: center;
	gap: 8px;
	padding: 5px 0;
	border-bottom: 1px solid var(--amcf-color-border, rgba(140, 150, 165, 0.18));
}
.amcf-status-list__row:last-child {
	border-bottom: none;
}

.amcf-status-list__label {
	flex: 1;
	font-size: 13px;
	color: var(--amcf-color-text-secondary, #9aa2ab);
}

.amcf-status-list__value {
	font-size: 13px;
	font-variant-numeric: tabular-nums;
}

.amcf-status-list__pill {
	display: inline-block;
	padding: 1px 9px;
	border-radius: 999px;
	font-size: 11.5px;
	font-weight: 600;
}
.amcf-status-list__pill--success { background: rgba(67, 160, 71, 0.14); color: #2E7D32; }
.amcf-status-list__pill--warning { background: rgba(249, 168, 37, 0.16); color: #E65100; }
.amcf-status-list__pill--error   { background: rgba(200, 16, 46, 0.12);  color: #C8102E; }
.amcf-status-list__pill--info    { background: rgba(41, 121, 255, 0.12); color: #1565C0; }
.amcf-status-list__pill--neutral { background: rgba(140, 150, 165, 0.18); color: #607D8B; }
</style>
