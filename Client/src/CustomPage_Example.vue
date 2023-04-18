<template>

	<div style="width:100%; height:100%; display:block;">
		<div>
			Test1234: {{ customProperties.values.dummycounter }}
		</div>
	
		<Module_Content :module="uploadModule" :Application="Application" />
		<Module_LayerView :module="previewModule" :Application="Application" />		
	</div>
	
</template>

<script>

	import Module_Content from "./AMCModule_Content.vue";
	import Module_LayerView from "./AMCModule_LayerView.vue";

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