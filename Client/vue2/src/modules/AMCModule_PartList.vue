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
<div v-if="module.visible !== false" class="pl-root">

	<div v-if="loading && !details" class="pl-message">{{ loadingText }}</div>
	<div v-else-if="errorMessage" class="pl-message pl-error">{{ errorMessage }}</div>
	<div v-else-if="!details" class="pl-message">No build selected.</div>
	<template v-else>

		<!-- Build detail header -->
		<div class="pl-header">
			<img
				v-if="isValidUUID(details.thumbnail)"
				class="pl-thumb-img"
				:src="Application.getImageURL(details.thumbnail)"
			/>
			<div v-else class="pl-thumb-placeholder">
				<v-icon color="grey lighten-1" size="28">mdi-file-3d-outline</v-icon>
			</div>
			<div class="pl-detail-grid">
				<span class="pl-detail-label">Name</span>
				<span class="pl-detail-value font-weight-medium">{{ details.name || '—' }}</span>
				<span class="pl-detail-label">Layers</span>
				<span class="pl-detail-value">{{ details.layercount != null ? details.layercount : '—' }}</span>
				<span class="pl-detail-label">Layer thickness</span>
				<span class="pl-detail-value">{{ layerThickness }}</span>
				<span class="pl-detail-label">File size</span>
				<span class="pl-detail-value">{{ formatBytes(details.size) }}</span>
			</div>
		</div>

		<!-- Part list -->
		<div class="pl-section-title">Parts ({{ parts.length }})</div>
		<v-simple-table dense class="pl-table">
			<template #default>
				<thead>
					<tr>
						<th class="text-left" style="width: 40px;"></th>
						<th class="text-left">Part name</th>
						<th class="text-left" style="width: 160px;">Part number</th>
					</tr>
				</thead>
				<tbody>
					<tr v-if="parts.length === 0">
						<td colspan="3" class="pl-empty">No parts in this build.</td>
					</tr>
					<tr v-for="(part, idx) in parts" :key="part.uuid || idx">
						<td><v-icon small color="grey">mdi-cube-outline</v-icon></td>
						<td class="font-weight-medium">{{ part.name || '—' }}</td>
						<td class="pl-partnumber">{{ part.partnumber || '—' }}</td>
					</tr>
				</tbody>
			</template>
		</v-simple-table>

	</template>

</div>
</template>

<script>
import { isValidUUID } from '../../../core/modules/AMCModule_TableUtils.js';

export default {
	props: ['Application', 'module'],

	computed: {
		// The build details are fetched by the model class (AMCModule_PartList.js); this
		// component only renders what the model exposes.
		details () {
			return this.module.details || null;
		},

		loading () {
			return this.module.loading === true;
		},

		errorMessage () {
			return this.module.errorMessage || '';
		},

		loadingText () {
			return this.module.loadingtext || 'Loading build details...';
		},

		parts () {
			return (this.module.parts && this.module.parts.length) ? this.module.parts : [];
		},

		layerThickness () {
			const d = this.details;
			if (!d) return '—';
			if (d.variablelayers) return 'Variable';
			if (typeof d.globallayerthickness === 'number' && d.globallayerthickness > 0)
				return d.globallayerthickness.toFixed(3) + ' mm';
			return '—';
		},
	},

	methods: {
		isValidUUID,

		formatBytes (bytes) {
			if (!bytes || bytes <= 0) return '—';
			if (bytes >= 1048576) return (bytes / 1048576).toFixed(1) + ' MB';
			if (bytes >= 1024) return (bytes / 1024).toFixed(1) + ' KB';
			return bytes + ' B';
		},
	},
};
</script>

<style scoped>
.pl-root {
	width: 100%;
	min-height: 0;
}

.pl-message {
	font-size: 0.875rem;
	color: rgba(0, 0, 0, 0.55);
	padding: 8px 0;
}

.pl-error {
	color: #c62828;
}

.pl-header {
	display: flex;
	gap: 12px;
	margin-bottom: 12px;
}

.pl-thumb-img {
	display: block;
	width: 128px;
	height: 96px;
	object-fit: contain;
	border-radius: 4px;
	background: #f5f5f5;
	flex-shrink: 0;
}

.pl-thumb-placeholder {
	width: 128px;
	height: 96px;
	display: flex;
	align-items: center;
	justify-content: center;
	background: #f5f5f5;
	border-radius: 4px;
	flex-shrink: 0;
}

.pl-detail-grid {
	display: grid;
	grid-template-columns: auto 1fr;
	column-gap: 16px;
	row-gap: 4px;
	align-content: start;
	font-size: 0.8125rem;
}

.pl-detail-label {
	color: rgba(0, 0, 0, 0.55);
}

.pl-detail-value {
	font-size: 0.875rem;
}

.pl-section-title {
	font-size: 0.7rem;
	font-weight: 600;
	text-transform: uppercase;
	letter-spacing: 0.05em;
	color: rgba(0, 0, 0, 0.55);
	margin-bottom: 4px;
}

.pl-partnumber {
	color: rgba(0, 0, 0, 0.55);
}

.pl-empty {
	text-align: center;
	padding: 24px 0;
	color: rgba(0, 0, 0, 0.45);
	font-size: 0.875rem;
}
</style>
