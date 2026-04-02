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

	<v-card v-if="(module.type == 'tabs') && module.visible !== false" class="tabs-root">

		<!-- Tab header bar -->
		<v-tabs v-model="tab" class="tabs-bar">
			<template v-for="moduleTab in module.tabs">
				<v-tab v-if="moduleTab.visible" :key="moduleTab.uuid">
					{{ moduleTab.caption }}
				</v-tab>
			</template>
		</v-tabs>

		<!-- Tab content fills the remaining height via flex:1 -->
		<v-tabs-items v-model="tab" class="tabs-content">
			<template v-for="moduleTab in module.tabs">
				<v-tab-item v-if="moduleTab.visible" :key="moduleTab.uuid" class="tabs-item">
					<Module_Factory :module="moduleTab" :Application="Application" />
				</v-tab-item>
			</template>
		</v-tabs-items>

	</v-card>

</template>

<script>
export default {
	props: ['Application', 'module'],

	data: () => ({
		tab: null,
	}),
};
</script>

<style scoped>
/* Root card is a flex column so the tab bar and content area share height */
.tabs-root {
	width: 100%;
	height: 100%;
	display: flex !important;
	flex-direction: column;
}

/* Tab bar: shrinks to its natural height */
.tabs-bar {
	flex: 0 0 auto;
}

/* Tab content: takes all remaining height; Vuetify sets this to block by default */
.tabs-content {
	flex: 1 1 0 !important;
	min-height: 0;
	overflow: auto;
	width: 100%;
}

/*
 * Vuetify sets height:auto on .v-window__container after the slide/fade
 * transition ends, collapsing tab content to its natural height.
 * Forcing height:100% here keeps it flush with the flex-allocated space
 * both during and after the animation.
 */
.tabs-content ::v-deep .v-window__container {
	height: 100%;
}

/* Individual tab item fills its container */
.tabs-item {
	width: 100%;
	height: 100%;
}
</style>
