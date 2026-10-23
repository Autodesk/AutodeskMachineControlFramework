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
<div class="amcff-wrap">

	<!-- edit -->
	<v-text-field v-if="entity.type === 'edit'"
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
		@blur="$emit('change-event', { entity })"
		@keyup.enter="$emit('change-event', { entity })"
	/>

	<!-- switch -->
	<div v-else-if="entity.type === 'switch'" class="amcff-switch">
		<span class="amcff-switch-label">{{ entity.caption }}</span>
		<v-switch dense hide-details inset v-model="entity.dataObject.value"
			@change="$emit('toggle-event', { entity })" />
	</div>

	<!-- memo -->
	<v-textarea v-else-if="entity.type === 'memo'"
		outlined dense
		:label="entity.caption"
		v-model="entity.dataObject.value"
		:disabled="entity.dataObject.disabled"
		:readonly="entity.dataObject.readonly"
		hide-details="auto"
		class="form-input"
	/>

	<!-- combobox -->
	<v-select v-else-if="entity.type === 'combobox'"
		outlined dense
		v-model="entity.dataObject.value"
		:label="entity.caption"
		:disabled="entity.dataObject.disabled"
		:readonly="entity.dataObject.readonly"
		:items="entity.items"
		hide-details="auto"
		class="form-input"
		@input="$emit('combo-event', { entity })"
	/>

	<!-- number -->
	<v-text-field v-else-if="entity.type === 'number'"
		outlined dense type="number"
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
		@blur="$emit('change-event', { entity })"
		@keyup.enter="$emit('change-event', { entity })"
	/>

	<!-- slider -->
	<div v-else-if="entity.type === 'slider'" class="amcff-slider-wrap">
		<div class="amcff-slider-label">
			<span>{{ entity.caption }}</span>
			<span>{{ entity.dataObject.value }}<span v-if="entity.dataObject.unit"> {{ entity.dataObject.unit }}</span></span>
		</div>
		<v-slider
			v-model="entity.dataObject.value"
			:min="parseFloat(entity.dataObject.minvalue) || 0"
			:max="parseFloat(entity.dataObject.maxvalue) || 100"
			:step="parseFloat(entity.dataObject.step) || 1"
			:disabled="entity.dataObject.disabled"
			thumb-label hide-details="auto"
			@end="$emit('change-event', { entity })"
		/>
		<div class="amcff-slider-bounds">
			<span>{{ parseFloat(entity.dataObject.minvalue) || 0 }}<span v-if="entity.dataObject.unit"> {{ entity.dataObject.unit }}</span></span>
			<span>{{ parseFloat(entity.dataObject.maxvalue) || 100 }}<span v-if="entity.dataObject.unit"> {{ entity.dataObject.unit }}</span></span>
		</div>
	</div>

	<!-- multiselect -->
	<v-select v-else-if="entity.type === 'multiselect'"
		outlined dense multiple chips small-chips
		:value="parseMultiselect(entity.dataObject.value)"
		:label="entity.caption"
		:disabled="entity.dataObject.disabled"
		:items="entity.items"
		hide-details="auto"
		class="form-input"
		@input="v => { entity.dataObject.value = JSON.stringify(v); $emit('combo-event', { entity }); }"
	/>

	<!-- calculated -->
	<div v-else-if="entity.type === 'calculated'" class="amcff-calc">
		<span class="amcff-calc-label">{{ entity.caption }}</span>
		<v-chip outlined label>
			{{ entity.dataObject.value }}
			<span v-if="entity.dataObject.unit" class="ml-1" style="font-size:0.8rem;color:rgba(0,0,0,.54)">{{ entity.dataObject.unit }}</span>
		</v-chip>
	</div>

</div>
</template>

<script>
export default {
	name: 'AMCFormField',
	props: ['entity', 'serverErrors'],
	methods: {
		serverError (e) {
			if (!this.serverErrors || !e) return [];
			const m = this.serverErrors[e.uuid];
			return m ? [m] : [];
		},
		parseMultiselect (v) {
			if (!v) return [];
			try { return JSON.parse(v); } catch (_) { return []; }
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
	},
};
</script>

<style scoped>
.amcff-wrap { width: 100%; }

.form-input >>> .v-input__control > .v-input__slot { min-height: 36px !important; }
.form-input >>> .v-text-field__slot label,
.form-input >>> .v-select__slot label { font-size: 0.875rem; }
.form-input >>> .v-text-field__slot input,
.form-input >>> .v-select__slot .v-select__selection { font-size: 0.875rem; }
.form-input >>> fieldset { border-radius: 6px; }

.amcff-switch {
	display: flex; flex-direction: row;
	align-items: center; justify-content: space-between;
	min-height: 40px; padding: 0 2px;
}
.amcff-switch-label { font-size: 0.875rem; color: rgba(0,0,0,.87); }

.amcff-slider-wrap { padding: 4px 2px; }
.amcff-slider-label {
	display: flex; justify-content: space-between;
	font-size: 0.875rem; color: rgba(0,0,0,.87); margin-bottom: 2px;
}
.amcff-slider-bounds {
	display: flex; justify-content: space-between;
	font-size: 0.75rem; color: rgba(0,0,0,.55); margin-top: -6px;
}

.amcff-calc {
	display: flex; align-items: center; justify-content: space-between;
	padding: 6px 2px; min-height: 40px;
}
.amcff-calc-label { font-size: 0.875rem; color: rgba(0,0,0,.7); }
</style>
