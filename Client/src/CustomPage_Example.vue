<template>

	<div style="width:100%; height:100%; display:block;">
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
			myValue: 0.0
			
		}),
		
		methods: {
		
			/*onDataUpdate ()
			{
				this.myValue = this.CustomPage.getParameter("test12");	
			
			} */
			   
		},
		
		
		created () {
		
			this.previewModule = this.CustomPage.findModule ("preview");
			this.uploadModule = this.CustomPage.findModule ("upload");
			
		},
		
		mounted() {
		
			//Page.setOnDataUpdate (this.onDataUpdate);
			
			
			
			
		}
		
	};
	
</script>