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

<!--
  AMCModule_TableBase.vue
  Shared base table component used by AlertList, BuildList, ExecutionList and
  optionally by ParameterList / ConfigurationList for the scrollable
  sticky-header layout.

  Props:
    columns      - Array<{ key, label, width, align? }>  header definitions
    entries      - Array<any>                             row data
    rowKey       - String                                 unique key field name in each entry
    emptyIcon    - String                                 mdi icon for empty state
    emptyTitle   - String                                 primary empty-state text
    emptyHint    - String                                 secondary empty-state text (optional)

  Slots:
    cell-<key>({ item })   – scoped slot for custom column rendering
    row-actions({ item })  – scoped slot for per-row action buttons column

  Events:
    row-click(item)        – emitted when a data row is clicked
-->

<template>
<div class="tb-root">

	<div class="tb-area">

		<!-- Sticky header -->
		<div class="tb-header">
			<table class="tb-table">
				<colgroup>
					<col
						v-for="col in columns"
						:key="'hcol-' + col.key"
						:style="col.width ? 'width:' + col.width : ''"
					/>
					<col v-if="hasRowActions" style="width:1px" />
				</colgroup>
				<thead>
					<tr>
						<th
							v-for="col in columns"
							:key="'th-' + col.key"
							:style="col.align ? 'text-align:' + col.align : ''"
						>{{ col.label }}</th>
						<th v-if="hasRowActions"></th>
					</tr>
				</thead>
			</table>
		</div>

		<!-- Scrollable body -->
		<div class="tb-scroll">
			<table class="tb-table">
				<colgroup>
					<col
						v-for="col in columns"
						:key="'bcol-' + col.key"
						:style="col.width ? 'width:' + col.width : ''"
					/>
					<col v-if="hasRowActions" style="width:1px" />
				</colgroup>
				<tbody>

					<!-- Empty state -->
					<tr v-if="!entries || entries.length === 0">
						<td :colspan="hasRowActions ? columns.length + 1 : columns.length" class="tb-cell-empty">
							<div class="tb-empty-state">
								<v-icon color="grey lighten-1" size="36">{{ emptyIcon || 'mdi-table-off' }}</v-icon>
								<span class="tb-empty-title">{{ emptyTitle || 'No entries' }}</span>
								<span v-if="emptyHint" class="tb-empty-hint">{{ emptyHint }}</span>
							</div>
						</td>
					</tr>

					<!-- Data rows -->
					<tr
						v-for="item in entries"
						:key="item[rowKey]"
						class="tb-row"
						@click="$emit('row-click', item)"
					>
						<td
							v-for="col in columns"
							:key="'cell-' + col.key"
							class="tb-cell"
							:style="col.align ? 'text-align:' + col.align : ''"
						>
							<slot :name="'cell-' + col.key" :item="item">
								{{ item[col.key] }}
							</slot>
						</td>

						<!-- Row actions slot -->
						<td v-if="hasRowActions" class="tb-cell tb-cell-actions" @click.stop>
							<slot name="row-actions" :item="item" />
						</td>
					</tr>

				</tbody>
			</table>
		</div>

	</div>

</div>
</template>

<script>
export default {
	name: 'AMCModule_TableBase',

	props: {
		columns:      { type: Array,  required: true },
		entries:      { type: Array,  default: () => [] },
		rowKey:       { type: String, default: 'uuid' },
		emptyIcon:    { type: String, default: 'mdi-table-off' },
		emptyTitle:   { type: String, default: 'No entries' },
		emptyHint:    { type: String, default: '' },
	},

	computed: {
		hasRowActions () {
			return !!this.$slots['row-actions'] || !!this.$scopedSlots['row-actions'];
		},
	},
};
</script>

<style scoped>
/* ── Root ────────────────────────────────────────────────── */
.tb-root {
	width: 100%;
	min-height: 0;
	display: flex;
	flex-direction: column;
}

.tb-area {
	width: 100%;
	display: flex;
	flex-direction: column;
}

/* ── Header ──────────────────────────────────────────────── */
.tb-header {
	border: 1px solid rgba(0, 0, 0, 0.1);
	border-bottom: none;
	border-radius: 6px 6px 0 0;
	overflow: hidden;
}

/* ── Scrollable body ─────────────────────────────────────── */
.tb-scroll {
	overflow-x: hidden;
	border: 1px solid rgba(0, 0, 0, 0.1);
	border-top: none;
	border-radius: 0 0 6px 6px;
}

/* ── Shared table ────────────────────────────────────────── */
.tb-table {
	width: 100%;
	border-collapse: collapse;
	table-layout: fixed;
}

.tb-table thead th {
	background: #fafafa;
	text-align: left;
	padding: 6px 12px;
	font-size: 0.75rem;
	font-weight: 600;
	color: rgba(0, 0, 0, 0.55);
	text-transform: none;
	letter-spacing: 0;
	border-bottom: 1px solid rgba(0, 0, 0, 0.1);
	white-space: nowrap;
}

/* ── Body rows ───────────────────────────────────────────── */
.tb-row {
	cursor: pointer;
}

.tb-row:hover {
	background: rgba(0, 0, 0, 0.03);
}

.tb-cell {
	padding: 7px 12px;
	font-size: 0.8125rem;
	border-bottom: 1px solid rgba(0, 0, 0, 0.06);
	vertical-align: middle;
	overflow: hidden;
	text-overflow: ellipsis;
	white-space: nowrap;
}

.tb-row:last-child .tb-cell {
	border-bottom: none;
}

/* ── Actions ─────────────────────────────────────────────── */
.tb-cell-actions {
	white-space: nowrap;
	padding-right: 8px;
}

/* ── Empty state ─────────────────────────────────────────── */
.tb-cell-empty {
	padding: 0;
	border-bottom: none;
}

.tb-empty-state {
	display: flex;
	flex-direction: column;
	align-items: center;
	gap: 6px;
	padding: 40px 16px;
}

.tb-empty-title {
	font-size: 0.875rem;
	color: rgba(0, 0, 0, 0.45);
	font-style: italic;
}

.tb-empty-hint {
	font-size: 0.8125rem;
	color: rgba(0, 0, 0, 0.3);
}
</style>
