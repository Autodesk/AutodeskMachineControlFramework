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

	<div style="width:100%; height:100%; display:block;">
		<div>
			Test1234: {{ customProperties.values.dummycounter }}
			
			<v-btn v-on:click="clickedOnTest">Testbutton</v-btn>
		</div>
	
		<Module_Content :module="uploadModule" :Application="Application" />
		<Module_LayerView :module="previewModule" :Application="Application" />		
	</div>
	
</template>

<script>

	import Module_Content from "./modules/AMCModule_Content.vue";
	import Module_LayerView from "./modules/AMCModule_LayerView.vue";

	export default {
		props: ["Application", "CustomPage"],
	  
		components: {
			Module_LayerView,
			Module_Content
		},
		
		data: () => ({				
			
			uploadModule: null,
			previewModule: null,
			customProperties: null,
			
		}),
		
		methods: {
			clickedOnTest () {
			
				this.CustomPage.callEvent ("onchangeatmosphereclicked", { "new_atmosphere_value": 12.0 });
			
			}
		},
		
		
		created () {
		
			this.previewModule = this.CustomPage.findModule ("preview");
			this.uploadModule = this.CustomPage.findModule ("upload");
			this.customProperties = this.CustomPage.getPropertiesObject ();
			
		},
		
		mounted() {
			
		}
		
	};
	
</script>