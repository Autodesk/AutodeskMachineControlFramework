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

	<div class="content-root">

		<!-- ── Card styles: elevated / outlined / tinted ─────────────────── -->
		<v-card
			v-if="isCard"
			class="content-card"
			:outlined="module.cardstyle === 'outlined'"
			:elevation="module.cardstyle === 'elevated' ? module.elevation : 0"
			:color="cardBackground || undefined"
		>
			<v-card-title v-if="module.title" class="content-card-title">
				{{ module.title }}
			</v-card-title>
			<v-card-subtitle v-if="module.subtitle" class="content-card-subtitle">
				{{ module.subtitle }}
			</v-card-subtitle>

			<v-card-text class="content-card-body">
				<div class="content-children" :style="childrenStyle">
					<Module_Factory
						v-for="childModule in module.modules"
						:key="childModule.uuid"
						:module="childModule"
						:Application="Application"
					/>
				</div>
			</v-card-text>
		</v-card>

		<!-- ── Plain (cardstyle="none") — backward-compatible divs ───────── -->
		<template v-else>
			<div v-if="module.title"    class="content-title">{{ module.title }}</div>
			<div v-if="module.subtitle" class="content-subtitle">{{ module.subtitle }}</div>

			<div
				v-if="module.modules && module.modules.length > 0"
				class="content-children"
				:style="childrenStyle"
			>
				<Module_Factory
					v-for="childModule in module.modules"
					:key="childModule.uuid"
					:module="childModule"
					:Application="Application"
				/>
			</div>
		</template>

	</div>

</template>

<script>
import Module_Factory from './AMCModule_Factory.vue';

export default {
	name: 'Module_Content',
	props: ['Application', 'module'],
	components: { Module_Factory },

	computed: {
		isCard () {
			const s = this.module.cardstyle;
			return s === 'elevated' || s === 'outlined' || s === 'tinted';
		},

		cardBackground () {
			if (this.module.cardstyle === 'tinted')
				return this.module.cardcolor || 'var(--amcf-color-surface-raised, #F5F6F8)';
			return '';
		},

		childrenStyle () {
			const gap = (this.module.spacing || 0) + 'px';
			return { gap };
		},
	},
};
</script>

<style scoped>
/*
 * The root always fills its grid cell / flex parent completely.
 * display:flex + flex-direction:column lets the card or the children
 * wrapper stretch via flex:1.
 */
.content-root {
	display: flex;
	flex-direction: column;
	width: 100%;
	height: 100%;
}

/* ── Card variant ─────────────────────────────────────────────────── */
.content-card {
	flex: 1 1 0;
	min-height: 0;
	display: flex;
	flex-direction: column;
}

/*
 * Vuetify injects its own padding on v-card__text; we override it so the
 * inner body can stretch to fill the card height.
 */
.content-card ::v-deep .v-card__text {
	flex: 1 1 0;
	min-height: 0;
	padding: 8px 12px 12px;
}

.content-card-title {
	padding-bottom: 0;
	font-size: var(--amcf-typo-h2-size, 1rem);
	font-weight: var(--amcf-typo-h2-weight, 700);
	line-height: 1.3;
	color: var(--amcf-color-text, #1E1E1E);
}

.content-card-subtitle {
	padding-top: 2px;
	font-size: var(--amcf-typo-subtext-size, 0.8125rem);
	color: var(--amcf-color-text-muted, #5F6368);
}

.content-card-body {
	flex: 1 1 0;
	min-height: 0;
	overflow: auto;
}

/* ── Plain (none) variant ─────────────────────────────────────────── */
.content-title {
	font-size: var(--amcf-typo-h2-size, 1.125rem);
	font-weight: var(--amcf-typo-h2-weight, 600);
	color: var(--amcf-color-text, #1E1E1E);
	padding: 4px 0 2px;
}

.content-subtitle {
	font-size: 0.9375rem;
	color: var(--amcf-color-text-muted, #5F6368);
	padding-bottom: 8px;
}

/* ── Shared children wrapper ──────────────────────────────────────── */
.content-children {
	flex: 1 1 0;
	min-height: 0;
	display: flex;
	flex-direction: column;
	width: 100%;
	height: 100%;
}
</style>
