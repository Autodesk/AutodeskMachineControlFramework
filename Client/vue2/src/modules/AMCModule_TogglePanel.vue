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

<div v-if="module.visible !== false" class="amcf-togglepanel" :style="gridStyle">
	<div
		v-for="(toggle, index) in (module.toggles || [])"
		:key="toggle.uuid || index"
		class="amcf-togglepanel__item"
	>
		<div class="amcf-togglepanel__labels">
			<span class="amcf-togglepanel__label">{{ toggle.label }}</span>
			<span class="amcf-togglepanel__state">{{ stateLabel(toggle) }}</span>
		</div>
		<v-switch
			:input-value="isTruthy(toggle.value)"
			dense hide-details inset
			class="amcf-togglepanel__switch"
			@change="onToggle(toggle)"
		/>
	</div>
</div>

</template>

<script>
export default {
	props: ['Application', 'module'],

	computed: {
		gridStyle () {
			const columns = this.module.columns || 3;
			return { 'grid-template-columns': 'repeat(' + columns + ', minmax(0, 1fr))' };
		},
	},

	methods: {
		isTruthy (value) {
			const v = (value === undefined || value === null ? '' : value).toString().trim().toLowerCase();
			return !(v === '' || v === '0' || v === 'false' || v === 'off' || v === 'no');
		},

		stateLabel (toggle) {
			const on = this.isTruthy(toggle.value);
			switch (toggle.mode) {
				case 'yesno': return on ? 'Yes' : 'No';
				case 'openclosed': return on ? 'Opened' : 'Closed';
				case 'leftright': return on ? 'Right' : 'Left';
				default: return on ? 'On' : 'Off';
			}
		},

		onToggle (toggle) {
			if (toggle.event && toggle.event !== '')
				this.Application.triggerUIEvent(toggle.event, toggle.uuid, {});
		},
	},
};
</script>

<style scoped>
.amcf-togglepanel {
	display: grid;
	gap: 8px;
	width: 100%;
}
.amcf-togglepanel__item {
	display: flex;
	align-items: center;
	justify-content: space-between;
	gap: 8px;
	border: 1px solid var(--amcf-color-border, rgba(140, 150, 165, 0.25));
	border-radius: 8px;
	padding: 6px 12px;
}
.amcf-togglepanel__labels {
	display: flex;
	flex-direction: column;
	min-width: 0;
}
.amcf-togglepanel__label {
	font-size: 13px;
	white-space: nowrap;
	overflow: hidden;
	text-overflow: ellipsis;
}
.amcf-togglepanel__state {
	font-size: 11.5px;
	color: var(--amcf-color-text-secondary, #9aa2ab);
}
.amcf-togglepanel__switch {
	margin: 0;
	padding: 0;
	flex-shrink: 0;
}
</style>
