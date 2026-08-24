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

<div v-if="module.visible !== false" class="amcf-para">
	<component :is="tagName" :class="variantClass">
		{{ module.text }}
	</component>
</div>

</template>

<script>
export default {
	props: ['Application', 'module'],

	computed: {
		variant () {
			return this.module.variant || 'body';
		},

		tagName () {
			switch (this.variant) {
				case 'h1': return 'h1';
				case 'h2': return 'h2';
				case 'label': return 'span';
				case 'subtext': return 'small';
				case 'code': return 'code';
				default: return 'p';
			}
		},

		variantClass () {
			return 'amcf-para__text amcf-para--' + this.variant;
		}
	}
};
</script>

<style scoped>
.amcf-para {
	width: 100%;
	min-height: 0;
}

.amcf-para__text {
	margin: 0;
	white-space: pre-wrap;
	font-family: var(--amcf-font-family, "Segoe UI", sans-serif);
}

.amcf-para--h1 {
	font-size: var(--amcf-typo-h1-size, 24px);
	font-weight: var(--amcf-typo-h1-weight, 700);
	line-height: 1.3;
	color: var(--amcf-color-text, rgba(0, 0, 0, 0.87));
}

.amcf-para--h2 {
	font-size: var(--amcf-typo-h2-size, 18px);
	font-weight: var(--amcf-typo-h2-weight, 600);
	line-height: 1.4;
	color: var(--amcf-color-text, rgba(0, 0, 0, 0.87));
}

.amcf-para--body {
	font-size: var(--amcf-typo-body-size, 14px);
	font-weight: var(--amcf-typo-body-weight, 400);
	line-height: 1.6;
	color: var(--amcf-color-text-muted, rgba(0, 0, 0, 0.87));
}

.amcf-para--label {
	display: block;
	font-size: var(--amcf-typo-label-size, 12px);
	font-weight: var(--amcf-typo-label-weight, 600);
	line-height: 1.5;
	color: var(--amcf-color-text-dim, rgba(0, 0, 0, 0.55));
}

.amcf-para--subtext {
	display: block;
	font-size: var(--amcf-typo-subtext-size, 12px);
	font-weight: var(--amcf-typo-subtext-weight, 400);
	line-height: 1.5;
	color: var(--amcf-color-text-dim, rgba(0, 0, 0, 0.55));
}

.amcf-para--code {
	display: block;
	font-family: var(--amcf-font-family-mono, Consolas, monospace);
	font-size: var(--amcf-typo-code-size, 12px);
	font-weight: 400;
	line-height: 1.5;
	color: var(--amcf-color-text-muted, rgba(0, 0, 0, 0.87));
	background: var(--amcf-color-surface-inset, rgba(0, 0, 0, 0.04));
	border: 1px solid var(--amcf-color-border, rgba(0, 0, 0, 0.12));
	border-radius: var(--amcf-radius-sm, 4px);
	padding: var(--amcf-space-2, 8px) var(--amcf-space-3, 12px);
}
</style>
