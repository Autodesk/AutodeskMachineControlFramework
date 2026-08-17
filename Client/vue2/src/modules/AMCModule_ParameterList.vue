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
	<v-data-table
		:headers="module.headers"
		:items="module.entries"
		:items-per-page="module.entriesperpage || -1"
		class="plist-table"
		disable-pagination
		hide-default-footer
		width="100%"
	>
		<template v-slot:item.paramValue="{ item }">
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
		};
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
	},
};
</script>

<style scoped>
.plist-root {
	width: 100%;
	min-height: 0;
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
