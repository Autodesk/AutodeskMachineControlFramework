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
	
	<div v-if="(module.type == 'grid')" v-bind:style="module.cssstyle">
		
			<template v-for="moduleSection in module.sections">

				<div :key="moduleSection.uuid" tile flat v-bind:style="moduleSection.cssstyle">
					<Module_Content :key="moduleSection.name" v-if="(moduleSection.type == 'content')" :module="moduleSection" :Application="Application" />
					<Module_Tabs :key="moduleSection.name" v-if="(moduleSection.type == 'tabs')" :module="moduleSection" :Application="Application" />
					<Module_GLScene :key="moduleSection.name" v-if="(moduleSection.type == 'glscene')" :module="moduleSection" :Application="Application" />
					<Module_Graphic :key="moduleSection.name" v-if="(moduleSection.type == 'graphic')" :module="moduleSection" :Application="Application" />
					<Module_LayerView :key="moduleSection.name" v-if="(moduleSection.type == 'layerview')" :module="moduleSection" :Application="Application" />
					<Module_Logs :key="moduleSection.name" v-if="(moduleSection.type == 'logs')" :module="moduleSection" :Application="Application" />
				</div>
						
			</template>	
		
	</div>

</template>

<script>

	import Module_Content from "../modules/AMCModule_Content.vue";
	import Module_Tabs from "../modules/AMCModule_Tabs.vue";
	import Module_GLScene from "../modules/AMCModule_GLScene.vue";
	import Module_Graphic from "../modules/AMCModule_Graphic.vue";
	import Module_Logs from "../modules/AMCModule_Logs.vue";
	import Module_LayerView from "../modules/AMCModule_LayerView.vue";

	export default {
		props: ["Application", "module"],
	  
		components: {
			Module_Content,
			Module_Logs,
			Module_Tabs,
			Module_GLScene,
			Module_Graphic,
			Module_LayerView
		},
		
		data: () => ({				
			
		})
		
	};
	
</script>