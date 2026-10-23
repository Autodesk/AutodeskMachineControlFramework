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

<div v-if="module.visible !== false" :class="rootClass">
	<hr v-if="isHorizontal" :class="lineClass" />
	<span v-else :class="lineClass"></span>
</div>

</template>

<script>
export default {
	props: ['Application', 'module'],

	computed: {
		isHorizontal () {
			return this.module.orientation !== 'vertical';
		},

		rootClass () {
			return [
				'amcf-separator',
				this.isHorizontal ? 'amcf-separator--horizontal' : 'amcf-separator--vertical'
			];
		},

		lineClass () {
			return [
				'amcf-separator__line',
				this.module.variant === 'dark' ? 'amcf-separator__line--dark' : 'amcf-separator__line--light'
			];
		}
	}
};
</script>

<style scoped>
.amcf-separator {
	display: flex;
	align-items: center;
	flex-shrink: 0;
}

.amcf-separator--horizontal {
	width: 100%;
	padding: var(--amcf-space-2, 8px) 0;
}

.amcf-separator--vertical {
	height: 100%;
	padding: 0 var(--amcf-space-2, 8px);
	align-self: stretch;
}

.amcf-separator__line {
	border: none;
	margin: 0;
	padding: 0;
}

hr.amcf-separator__line {
	width: 100%;
	border-top-width: 1px;
	border-top-style: solid;
}

span.amcf-separator__line {
	display: inline-block;
	width: 1px;
	min-height: 24px;
	height: 100%;
}

.amcf-separator__line--light {
	border-top-color: var(--amcf-color-border, #C8CCD0);
	background-color: var(--amcf-color-border, #C8CCD0);
}

.amcf-separator__line--dark {
	border-top-color: var(--amcf-color-border-strong, #9BA3AB);
	background-color: var(--amcf-color-border-strong, #9BA3AB);
}
</style>
