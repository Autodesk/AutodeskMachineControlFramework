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

<div v-if="module.visible !== false" class="btngroup-root" :style="module.cssstyle">
	<v-btn
		v-for="button in module.buttons"
		:key="button.uuid || button.name"
		:disabled="button.disabled"
		:color="button.color || 'primary'"
		:style="module.buttoncssstyle"
		class="btngroup-btn"
		@click.stop="uiModuleButtonClick(button)"
	>
		<v-icon v-if="button.icon" small left>{{ button.icon }}</v-icon>
		{{ button.caption }}
	</v-btn>
</div>

</template>

<script>
export default {
	props: ['Application', 'module'],

	methods: {
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
</style>
