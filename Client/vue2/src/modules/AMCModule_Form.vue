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

<div v-if="module.visible !== false" class="form-root">

	<template v-for="entity in module.entities">

		<!-- ── edit (text input) ─────────────────────────── -->
		<div :key="entity.uuid + '_edit'" v-if="entity.type === 'edit'" class="form-field">
			<v-text-field
				outlined dense
				v-model="entity.dataObject.value"
				:label="entity.caption"
				:disabled="entity.dataObject.disabled"
				:readonly="entity.dataObject.readonly"
				:prefix="entity.dataObject.prefix"
				:suffix="entity.dataObject.suffix"
				:rules="checkRules(entity)"
				:error-messages="serverError(entity)"
				hide-details="auto"
				class="form-input"
				@blur="uiEditBoxChange(entity)"
				@keyup.enter="uiEditBoxChange(entity)"
			/>
		</div>

		<!-- ── switch ────────────────────────────────────── -->
		<div :key="entity.uuid + '_switch'" v-if="entity.type === 'switch'" class="form-field form-field--switch">
			<span class="form-switch-label">{{ entity.caption }}</span>
			<v-switch
				dense hide-details inset
				v-model="entity.dataObject.value"
				class="form-switch"
				@change="uiToggleSwitch(entity)"
			/>
		</div>

		<!-- ── memo / textarea ───────────────────────────── -->
		<div :key="entity.uuid + '_memo'" v-if="entity.type === 'memo'" class="form-field">
			<v-textarea
				outlined dense
				:label="entity.caption"
				v-model="entity.dataObject.value"
				:disabled="entity.dataObject.disabled"
				:readonly="entity.dataObject.readonly"
				hide-details="auto"
				class="form-input"
			/>
		</div>

		<!-- ── combobox / select ─────────────────────────── -->
		<div :key="entity.uuid + '_combo'" v-if="entity.type === 'combobox'" class="form-field">
			<v-select
				outlined dense
				v-model="entity.dataObject.value"
				:label="entity.caption"
				:disabled="entity.dataObject.disabled"
				:readonly="entity.dataObject.readonly"
				:items="entity.items"
				hide-details="auto"
				class="form-input"
				@input="uiComboboxChange(entity)"
			/>
		</div>

		<!-- ── number ────────────────────────────────────── -->
		<div :key="entity.uuid + '_number'" v-if="entity.type === 'number'" class="form-field">
			<v-text-field
				outlined dense
				type="number"
				v-model="entity.dataObject.value"
				:label="entity.caption"
				:disabled="entity.dataObject.disabled"
				:readonly="entity.dataObject.readonly"
				:suffix="entity.dataObject.unit"
				:step="entity.dataObject.step"
				:min="entity.dataObject.minvalue"
				:max="entity.dataObject.maxvalue"
				:error-messages="serverError(entity)"
				hide-details="auto"
				class="form-input"
				@blur="uiEditBoxChange(entity)"
				@keyup.enter="uiEditBoxChange(entity)"
			/>
		</div>

		<!-- ── slider ────────────────────────────────────── -->
		<div :key="entity.uuid + '_slider'" v-if="entity.type === 'slider'" class="form-field form-field--slider">
			<div class="form-slider-label">
				<span>{{ entity.caption }}</span>
				<!-- editable numeric override: shares the slider's value for precise entry -->
				<span class="form-slider-value">
					<input
						type="number"
						class="form-slider-input"
						v-model.number="entity.dataObject.value"
						:min="parseFloat(entity.dataObject.minvalue)"
						:max="parseFloat(entity.dataObject.maxvalue)"
						:step="parseFloat(entity.dataObject.step) || 1"
						:disabled="entity.dataObject.disabled || entity.dataObject.readonly"
						@change="uiEditBoxChange(entity)"
						@keydown.enter="uiEditBoxChange(entity)"
					/>
					<span v-if="entity.dataObject.unit" class="form-slider-unit"> {{ entity.dataObject.unit }}</span>
				</span>
			</div>
			<v-slider
				v-model="entity.dataObject.value"
				:min="parseFloat(entity.dataObject.minvalue) || 0"
				:max="parseFloat(entity.dataObject.maxvalue) || 100"
				:step="parseFloat(entity.dataObject.step) || 1"
				:disabled="entity.dataObject.disabled"
				:readonly="entity.dataObject.readonly"
				thumb-label
				hide-details="auto"
				class="form-slider"
				@end="uiEditBoxChange(entity)"
			/>
		</div>

		<!-- ── datetime ──────────────────────────────────── -->
		<div :key="entity.uuid + '_datetime'" v-if="entity.type === 'datetime'" class="form-field">
			<v-menu
				v-model="dateMenuOpen[entity.uuid]"
				:close-on-content-click="false"
				transition="scale-transition"
				offset-y min-width="auto"
			>
				<template v-slot:activator="{ on, attrs }">
					<v-text-field
						outlined dense
						:value="entity.dataObject.value"
						:label="entity.caption"
						:disabled="entity.dataObject.disabled"
						:readonly="true"
						prepend-inner-icon="mdi-calendar-clock"
						hide-details="auto"
						class="form-input"
						v-bind="attrs"
						v-on="on"
					/>
				</template>
				<v-date-picker
					v-if="!entity.dataObject.mode || entity.dataObject.mode === 'date' || entity.dataObject.mode === 'datetime'"
					:value="entity.dataObject.value ? entity.dataObject.value.substring(0, 10) : ''"
					@input="v => { applyDatePart(entity, v); closeMenuIfDone(entity); }"
					no-title scrollable
				/>
				<v-time-picker
					v-if="entity.dataObject.mode === 'time'"
					:value="entity.dataObject.value"
					@input="v => { entity.dataObject.value = v; dateMenuOpen[entity.uuid] = false; uiEditBoxChange(entity); }"
					format="24hr"
				/>
			</v-menu>
		</div>

		<!-- ── multiselect ───────────────────────────────── -->
		<div :key="entity.uuid + '_multiselect'" v-if="entity.type === 'multiselect'" class="form-field">
			<v-select
				outlined dense multiple chips small-chips
				:value="parseMultiselectValue(entity.dataObject.value)"
				:label="entity.caption"
				:disabled="entity.dataObject.disabled"
				:readonly="entity.dataObject.readonly"
				:items="entity.items"
				hide-details="auto"
				class="form-input"
				@input="v => { entity.dataObject.value = JSON.stringify(v); uiComboboxChange(entity); }"
			/>
		</div>

		<!-- ── calculated (read-only display) ───────────── -->
		<div :key="entity.uuid + '_calculated'" v-if="entity.type === 'calculated'" class="form-field form-field--calculated">
			<span class="form-calc-label">{{ entity.caption }}</span>
			<v-chip class="form-calc-chip" outlined label>
				{{ entity.dataObject.value }}
				<span v-if="entity.dataObject.unit" class="form-calc-unit ml-1">{{ entity.dataObject.unit }}</span>
			</v-chip>
		</div>

		<!-- ── section ───────────────────────────────────── -->
		<div :key="entity.uuid + '_section'" v-if="entity.type === 'section'" class="form-section">
			<v-expansion-panels
				v-if="entity.dataObject.collapsible !== false"
				:value="entity.dataObject.defaultOpen !== false ? [0] : []"
				multiple flat
			>
				<v-expansion-panel>
					<v-expansion-panel-header class="form-section-header">
						<div class="form-section-title">
							<v-icon v-if="entity.dataObject.icon" small class="mr-2">{{ entity.dataObject.icon }}</v-icon>
							<span>{{ entity.caption }}</span>
						</div>
						<div v-if="entity.dataObject.description" class="form-section-desc">{{ entity.dataObject.description }}</div>
					</v-expansion-panel-header>
					<v-expansion-panel-content>
						<div class="form-section-children">
							<AMCFormField
								v-for="child in entity.entities"
								:key="child.uuid"
								:entity="child"
								:serverErrors="module.serverErrors"
								@change-event="onChildChangeEvent"
								@toggle-event="onChildToggleEvent"
								@combo-event="onChildComboEvent"
							/>
						</div>
					</v-expansion-panel-content>
				</v-expansion-panel>
			</v-expansion-panels>

			<div v-else class="form-section-static">
				<div class="form-section-header form-section-header--static">
					<div class="form-section-title">
						<v-icon v-if="entity.dataObject.icon" small class="mr-2">{{ entity.dataObject.icon }}</v-icon>
						<span>{{ entity.caption }}</span>
					</div>
					<div v-if="entity.dataObject.description" class="form-section-desc">{{ entity.dataObject.description }}</div>
				</div>
				<div class="form-section-children">
					<AMCFormField
						v-for="child in entity.entities"
						:key="child.uuid"
						:entity="child"
						:serverErrors="module.serverErrors"
						@change-event="onChildChangeEvent"
						@toggle-event="onChildToggleEvent"
						@combo-event="onChildComboEvent"
					/>
				</div>
			</div>
		</div>

		<!-- ── action bar ────────────────────────────────── -->
		<div :key="entity.uuid + '_actionbar'" v-if="entity.type === 'actionbar'" class="form-actionbar">
			<v-btn
				v-if="entity.dataObject.cancelevent"
				text
				class="form-actionbar-cancel"
				:disabled="module.isSubmitting"
				@click="uiActionCancel(entity)"
			>
				{{ entity.dataObject.cancelcaption || 'Cancel' }}
			</v-btn>
			<v-btn
				v-if="entity.dataObject.submitevent"
				color="primary"
				class="form-actionbar-submit"
				:disabled="!module.isFormDirty || module.isSubmitting"
				:loading="module.isSubmitting"
				@click="uiActionSubmit(entity)"
			>
				{{ entity.dataObject.submitcaption || 'Save' }}
			</v-btn>
		</div>

	</template>
</div>

</template>

<script>
import AMCFormField from './AMCFormField.vue';

export default {
	name: 'AMCModule_Form',
	components: { AMCFormField },
	props: ['Application', 'module'],

	data () {
		return {
			dateMenuOpen: {},
		};
	},

	methods: {

		serverError (entity) {
			if (!entity || !this.module.serverErrors) return [];
			const msg = this.module.serverErrors[entity.uuid];
			return msg ? [msg] : [];
		},

		parseMultiselectValue (v) {
			if (!v) return [];
			try { return JSON.parse(v); }
			catch (_) { return []; }
		},

		applyDatePart (entity, datePart) {
			if (!entity.dataObject.mode || entity.dataObject.mode === 'date') {
				entity.dataObject.value = datePart;
				this.dateMenuOpen[entity.uuid] = false;
				this.uiEditBoxChange(entity);
			} else {
				const timePart = (entity.dataObject.value || '').substring(11) || '00:00';
				entity.dataObject.value = datePart + 'T' + timePart;
			}
		},

		closeMenuIfDone (entity) {
			if (!entity.dataObject.mode || entity.dataObject.mode === 'date') {
				this.dateMenuOpen[entity.uuid] = false;
			}
		},

		uiToggleSwitch (switchentity) {
			if (!switchentity.dataObject) return;
			if (!switchentity.dataObject.isProgrammaticChange) {
				if (switchentity.dataObject.changeevent && switchentity.dataObject.changeevent !== '') {
					const formvalues = this.Application.assembleFormValues([switchentity.uuid]);
					this.Application.triggerUIEvent(switchentity.dataObject.changeevent, switchentity.uuid, formvalues, () => {
						switchentity.dataObject.isProgrammaticChange = true;
						switchentity.dataObject.value = switchentity.dataObject.remotevalue;
					});
				}
			} else {
				switchentity.dataObject.isProgrammaticChange = false;
			}
		},

		uiComboboxChange (comboboxEntity) {
			if (!comboboxEntity.dataObject) return;
			if (!comboboxEntity.dataObject.isProgrammaticChange) {
				if (comboboxEntity.dataObject.changeevent && comboboxEntity.dataObject.changeevent !== '') {
					const formvalues = this.Application.assembleFormValues([comboboxEntity.uuid]);
					this.Application.triggerUIEvent(comboboxEntity.dataObject.changeevent, comboboxEntity.uuid, formvalues, () => {
						comboboxEntity.dataObject.isProgrammaticChange = true;
						comboboxEntity.dataObject.value = comboboxEntity.dataObject.remotevalue;
					});
				}
			} else {
				comboboxEntity.dataObject.isProgrammaticChange = false;
			}
		},

		uiEditBoxChange (editentity) {
			if (!editentity || !editentity.dataObject) return;
			if (!editentity.dataObject.changeevent || editentity.dataObject.changeevent === '') return;
			if (editentity.dataObject.readonly || editentity.dataObject.disabled) return;
			const formvalues = this.Application.assembleFormValues([editentity.uuid]);
			this.Application.triggerUIEvent(editentity.dataObject.changeevent, editentity.uuid, formvalues);
		},

		onChildChangeEvent (payload) {
			const { entity } = payload;
			this.uiEditBoxChange(entity);
		},

		onChildToggleEvent (payload) {
			const { entity } = payload;
			this.uiToggleSwitch(entity);
		},

		onChildComboEvent (payload) {
			const { entity } = payload;
			this.uiComboboxChange(entity);
		},

		checkRules (editentity) {
			const rules = [];
			if (!editentity || !editentity.dataObject) return rules;
			if (editentity.dataObject.validation === 'double') {
				rules.push(v => !!v || editentity.dataObject.validationmessage);
				rules.push(v => !isNaN(v) || editentity.dataObject.validationmessage);
				rules.push(v => (v >= editentity.dataObject.minvalue && v <= editentity.dataObject.maxvalue) || editentity.dataObject.validationmessage);
			}
			if (editentity.dataObject.validation === 'string') {
				rules.push(v => (v !== null && v !== undefined && v !== '') || editentity.dataObject.validationmessage);
				if (editentity.dataObject.minlength != null || editentity.dataObject.maxlength != null) {
					rules.push(v => {
						const len = (v || '').length;
						const minOk = editentity.dataObject.minlength == null || len >= editentity.dataObject.minlength;
						const maxOk = editentity.dataObject.maxlength == null || len <= editentity.dataObject.maxlength;
						return (minOk && maxOk) || editentity.dataObject.validationmessage;
					});
				}
			}
			return rules;
		},

		async uiActionSubmit (actionbarEntity) {
			if (!actionbarEntity.dataObject) return;
			this.module.isSubmitting = true;
			this.module.serverErrors = {};

			try {
				const validateEvent = actionbarEntity.dataObject.validateevent;
				if (validateEvent && validateEvent !== '') {
					const formvalues = this.module.assembleAllFormValues();
					const result = await this.Application.validateFormValues(validateEvent, this.module.uuid, formvalues);
					if (!result.valid) {
						this.module.serverErrors = result.errors || {};
						return;
					}
				}

				const submitEvent = actionbarEntity.dataObject.submitevent;
				if (submitEvent && submitEvent !== '') {
					const formvalues = this.module.assembleAllFormValues();
					await this.Application.triggerUIEvent(submitEvent, this.module.uuid, formvalues);
					if (actionbarEntity.dataObject.optimistic) {
						for (let entity of this.module._flatEntities()) {
							if (entity.dataObject)
								entity.dataObject.remotevalue = entity.dataObject.value;
						}
					}
				}
			}
			finally {
				this.module.isSubmitting = false;
			}
		},

		uiActionCancel (actionbarEntity) {
			if (!actionbarEntity.dataObject) return;
			this.module.resetDirtyFields();
			const cancelEvent = actionbarEntity.dataObject.cancelevent;
			if (cancelEvent && cancelEvent !== '') {
				this.Application.triggerUIEvent(cancelEvent, this.module.uuid, {});
			}
		},
	},
};
</script>

<style scoped>
.form-root {
	display: flex;
	flex-direction: column;
	gap: 4px;
	width: 100%;
	min-height: 0;
}

.form-field {
	width: 100%;
}

.form-field--switch {
	display: flex;
	flex-direction: row;
	align-items: center;
	justify-content: space-between;
	min-height: 40px;
	padding: 0 2px;
}

.form-switch-label {
	font-size: 0.875rem;
	color: rgba(0, 0, 0, 0.87);
}

.form-switch {
	flex: 0 0 auto;
	margin: 0 !important;
}

/* ── number / edit shared input ────────────────────────────────────────── */
.form-input >>> .v-input__control > .v-input__slot {
	min-height: 36px !important;
}

.form-input >>> .v-text-field__slot label,
.form-input >>> .v-select__slot label {
	font-size: 0.875rem;
}

.form-input >>> .v-text-field__slot input,
.form-input >>> .v-select__slot .v-select__selection {
	font-size: 0.875rem;
}

.form-input >>> fieldset {
	border-radius: 6px;
}

/* ── slider ─────────────────────────────────────────────────────────────── */
.form-field--slider {
	padding: 4px 2px;
}

.form-slider-label {
	display: flex;
	justify-content: space-between;
	font-size: 0.875rem;
	color: rgba(0, 0, 0, 0.87);
	margin-bottom: 2px;
}

.form-slider-value {
	font-weight: 500;
	display: inline-flex;
	align-items: center;
	gap: 2px;
}

.form-slider-input {
	width: 4rem;
	text-align: right;
	font-variant-numeric: tabular-nums;
	font-weight: 500;
	border: 1px solid var(--amcf-color-border, rgba(140, 150, 165, 0.35));
	border-radius: 4px;
	padding: 1px 4px;
	background: transparent;
	color: inherit;
}

.form-slider-unit {
	color: rgba(0, 0, 0, 0.54);
	font-size: 0.8rem;
}

.form-slider {
	margin-top: 0 !important;
	padding-top: 0 !important;
}

/* ── calculated ─────────────────────────────────────────────────────────── */
.form-field--calculated {
	display: flex;
	align-items: center;
	justify-content: space-between;
	padding: 6px 2px;
	min-height: 40px;
}

.form-calc-label {
	font-size: 0.875rem;
	color: rgba(0, 0, 0, 0.7);
}

.form-calc-chip {
	font-size: 0.875rem;
	font-weight: 500;
	height: 28px;
}

.form-calc-unit {
	font-size: 0.8rem;
	color: rgba(0, 0, 0, 0.54);
}

/* ── section ────────────────────────────────────────────────────────────── */
.form-section {
	width: 100%;
	margin: 4px 0;
}

.form-section-header {
	padding: 8px 2px;
}

.form-section-header--static {
	border-bottom: 1px solid rgba(0, 0, 0, 0.12);
	margin-bottom: 8px;
}

.form-section-title {
	display: flex;
	align-items: center;
	font-size: 0.875rem;
	font-weight: 600;
	color: rgba(0, 0, 0, 0.87);
}

.form-section-desc {
	font-size: 0.8rem;
	color: rgba(0, 0, 0, 0.54);
	margin-top: 2px;
}

.form-section-children {
	display: flex;
	flex-direction: column;
	gap: 4px;
}

/* ── action bar ─────────────────────────────────────────────────────────── */
.form-actionbar {
	display: flex;
	flex-direction: row;
	justify-content: flex-end;
	gap: 8px;
	padding: 8px 0 2px;
	border-top: 1px solid rgba(0, 0, 0, 0.08);
	margin-top: 4px;
}

.form-actionbar-submit {
	min-width: 90px;
}

.form-actionbar-cancel {
	min-width: 80px;
}
</style>
