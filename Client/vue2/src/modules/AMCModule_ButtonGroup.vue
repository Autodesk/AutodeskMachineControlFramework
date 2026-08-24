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

<div v-if="module.visible !== false" class="btngroup-root" :class="{ 'btngroup-root--toolbar': isToolbar }" :style="module.cssstyle">
	<template v-for="(button, index) in module.buttons">
		<div v-if="button.kind === 'spring'" :key="button.uuid || 'spring' + index" class="btngroup-spring"></div>
		<div v-else-if="button.kind === 'spacer'" :key="button.uuid || 'spacer' + index" class="btngroup-spacer"></div>
		<v-btn
			v-else
			:key="button.uuid || button.name"
			:disabled="button.disabled"
			:color="button.variant === 'primary' ? 'primary' : undefined"
			:outlined="button.variant !== 'primary'"
			:style="module.buttoncssstyle"
			class="btngroup-btn"
			:class="[widthClass(button), { 'btngroup-btn--iconlabel': hasIcon(button) }]"
			@click.stop="uiModuleButtonClick(button)"
		>
			<div v-if="hasIcon(button)" class="btngroup-btn__stack">
				<!-- Render the packaged SVG as a mask tinted with currentColor so the
				     icon follows the button text colour in light and dark themes. -->
				<span v-if="button.iconresource" class="btngroup-btn__img" :style="iconMaskStyle(button)" aria-hidden="true"></span>
				<v-icon v-else-if="button.icon" small>{{ button.icon }}</v-icon>
				<span v-if="button.caption" class="btngroup-btn__label">{{ button.caption }}</span>
			</div>
			<template v-else>
				{{ button.caption }}
			</template>
		</v-btn>
	</template>
</div>

</template>

<script>
export default {
	props: ['Application', 'module'],

	computed: {
		isToolbar() {
			return this.module.buttondistribution === 'toolbar';
		},
	},

	methods: {
		hasIcon(button) {
			return !!(button.icon || button.iconresource);
		},

		widthClass(button) {
			if (button.width === 'narrow')
				return 'btngroup-btn--narrow';
			if (button.width === 'fixed')
				return 'btngroup-btn--fixed';
			return '';
		},

		iconResourceURL(button) {
			return button.iconresource ? this.Application.getImageURL(button.iconresource) : '';
		},

		iconMaskStyle(button) {
			const url = this.iconResourceURL(button);
			const mask = "url('" + url + "') no-repeat center / contain";
			return {
				backgroundColor: 'currentColor',
				webkitMask: mask,
				mask: mask,
			};
		},

		uiModuleButtonClick(button) {
			const formvalues = this.Application.assembleFormValues(button.eventformvalues);
			if (button.event && button.event !== '')
				this.Application.triggerUIEvent(button.event, button.uuid, formvalues);
			if (button.targetpage && button.targetpage !== '')
				this.Application.changePage(button.targetpage);
		},
	},
};
</script>

<style scoped>
.btngroup-root {
	display: flex;
	flex-direction: row;
	flex-wrap: wrap;
	align-items: center;
	gap: 8px;
	min-height: 0;
}
.btngroup-btn {
	height: 36px !important;
	letter-spacing: 0.01em;
	text-transform: none;
	font-weight: 500;
}

/* Toolbar layout: no wrapping so spacers/springs keep their positions. */
.btngroup-root--toolbar {
	flex-wrap: nowrap;
	align-items: stretch;
}
.btngroup-spring {
	flex: 1 1 0;
	min-width: 0;
}
.btngroup-spacer {
	flex: 0 0 24px;
}
.btngroup-btn--narrow {
	min-width: 64px !important;
	padding: 0 6px !important;
}
.btngroup-btn--fixed {
	min-width: 112px !important;
}
.btngroup-btn--iconlabel {
	height: auto !important;
	min-height: 56px;
	padding: 8px 4px !important;
}
.btngroup-btn__stack {
	display: flex;
	flex-direction: column;
	align-items: center;
	gap: 4px;
}
.btngroup-btn__img {
	display: inline-block;
	height: 24px;
	width: 24px;
}
.btngroup-btn__label {
	font-size: 12px;
}
</style>
