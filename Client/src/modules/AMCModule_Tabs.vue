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

	<v-card v-if="(module.type == 'tabs')" style="width:100%; height:100%">
		
		<v-tabs  v-model="tab">
				
			<template v-for="moduleTab in module.tabs">
						
				<v-tab :key="moduleTab.uuid">
					{{ moduleTab.caption }}
				</v-tab>
							
			</template>
			
		</v-tabs>
		
		<v-tabs-items v-model="tab"  style="overflow:auto; width:100%; height:95%;">
		
			<template v-for="moduleTab in module.tabs">
						
				<v-tab-item :key="moduleTab.uuid" style="overflow:auto; width:100%; height:100%;">
						<Module_Content :key="moduleTab.name" v-if="(moduleTab.type == 'content')" :module="moduleTab" :Application="Application" />					
						<Module_GLScene :key="moduleTab.name" v-if="(moduleTab.type == 'glscene')" :module="moduleTab" :Application="Application" />							
						<Module_Graphic :key="moduleTab.name" v-if="(moduleTab.type == 'graphic')" :module="moduleTab" :Application="Application" />							
						<Module_LayerView :key="moduleTab.name" v-if="(moduleTab.type == 'layerview')" :module="moduleTab" :Application="Application" />							
						<Module_Logs :key="moduleTab.name" v-if="(moduleTab.type == 'logs')" :module="moduleTab" :Application="Application" />							
				</v-tab-item>
							
			</template>	
		</v-tabs-items>

	</v-card>	

</template>

<script>

	import Module_Content from "../modules/AMCModule_Content.vue";
	import Module_GLScene from "../modules/AMCModule_GLScene.vue";
	import Module_Graphic from "../modules/AMCModule_Graphic.vue";
	import Module_LayerView from "../modules/AMCModule_LayerView.vue";
	import Module_Logs from "../modules/AMCModule_Logs.vue";

	export default {
		props: ["Application", "module"],
	  
		components: {
			Module_Content,
			Module_GLScene,
			Module_Graphic,
			Module_LayerView,
			Module_Logs,
		},
		
		data: () => ({
			tab: null
		})
		
	};
	
</script>