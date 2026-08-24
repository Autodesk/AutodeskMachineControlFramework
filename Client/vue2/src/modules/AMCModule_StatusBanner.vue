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

<div v-if="module.visible !== false" :class="bannerClass" role="status" :aria-label="ariaLabel">
	<span class="amcf-status-banner__dot"></span>
	<v-icon v-if="iconName" small class="amcf-status-banner__icon">{{ iconName }}</v-icon>
	<span class="amcf-status-banner__content">
		<strong v-if="module.title">{{ module.title }}</strong>
		<span v-if="module.description"> &mdash; {{ module.description }}</span>
	</span>
</div>

</template>

<script>
export default {
	props: ['Application', 'module'],

	computed: {
		severity () {
			const s = (this.module.severity || 'ok').toLowerCase();
			if (['ok', 'warning', 'error', 'archive', 'info'].indexOf(s) >= 0)
				return s;
			return 'ok';
		},

		bannerClass () {
			return [
				'amcf-status-banner',
				'amcf-status-banner--' + this.severity
			];
		},

		iconName () {
			switch (this.severity) {
				case 'ok':      return 'mdi-check-circle-outline';
				case 'warning': return 'mdi-alert-outline';
				case 'error':   return 'mdi-alert-circle-outline';
				case 'info':    return 'mdi-information-outline';
				case 'archive': return 'mdi-archive-outline';
				default:        return 'mdi-information-outline';
			}
		},

		ariaLabel () {
			return 'System Status: ' + this.severity + (this.module.title ? ' - ' + this.module.title : '');
		}
	}
};
</script>

<style scoped>
.amcf-status-banner {
	display: flex;
	align-items: center;
	gap: var(--amcf-space-3, 12px);
	padding: var(--amcf-space-3, 12px) var(--amcf-space-4, 16px);
	border-radius: var(--amcf-radius-lg, 8px);
	font-size: var(--amcf-typo-body-size, 14px);
	font-weight: 500;
	font-family: var(--amcf-font-family, "Segoe UI", sans-serif);
	width: 100%;
	box-sizing: border-box;
}

.amcf-status-banner__dot {
	width: 8px;
	height: 8px;
	border-radius: 50%;
	flex-shrink: 0;
}

.amcf-status-banner__icon {
	flex-shrink: 0;
}

.amcf-status-banner__content {
	flex: 1;
	min-width: 0;
}

/* OK */
.amcf-status-banner--ok {
	background: var(--amcf-color-ok-bg, rgba(67, 160, 71, 0.08));
	border: 1px solid var(--amcf-color-ok-border, rgba(67, 160, 71, 0.22));
	color: var(--amcf-color-ok, #2E7D32);
}
.amcf-status-banner--ok .amcf-status-banner__dot {
	background: var(--amcf-color-ok, #43A047);
}

/* Warning */
.amcf-status-banner--warning {
	background: var(--amcf-color-warn-bg, rgba(249, 168, 37, 0.08));
	border: 1px solid var(--amcf-color-warn-border, rgba(249, 168, 37, 0.22));
	color: #E65100;
}
.amcf-status-banner--warning .amcf-status-banner__dot {
	background: var(--amcf-color-warn, #F9A825);
}

/* Error */
.amcf-status-banner--error {
	background: var(--amcf-color-error-bg, rgba(200, 16, 46, 0.06));
	border: 1px solid var(--amcf-color-error-border, rgba(200, 16, 46, 0.18));
	color: var(--amcf-color-error, #C8102E);
}
.amcf-status-banner--error .amcf-status-banner__dot {
	background: var(--amcf-color-error, #C8102E);
}

/* Info */
.amcf-status-banner--info {
	background: var(--amcf-color-info-bg, rgba(41, 121, 255, 0.06));
	border: 1px solid rgba(41, 121, 255, 0.18);
	color: #1565C0;
}
.amcf-status-banner--info .amcf-status-banner__dot {
	background: var(--amcf-color-info, #2979FF);
}

/* Archive */
.amcf-status-banner--archive {
	background: rgba(144, 164, 174, 0.08);
	border: 1px solid rgba(144, 164, 174, 0.22);
	color: var(--amcf-color-archive, #607D8B);
}
.amcf-status-banner--archive .amcf-status-banner__dot {
	background: var(--amcf-color-archive, #90A4AE);
}
</style>
