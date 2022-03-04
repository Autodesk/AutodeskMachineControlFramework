<template>

	<v-card v-if="(module.type == 'tabs')" style="width:100%; height:100%">
		
		<v-tabs  v-model="tab">
				
			<template v-for="moduleTab in module.tabs">
						
				<v-tab :key="moduleTab.uuid">
					{{ moduleTab.caption }}
				</v-tab>
							
			</template>
			
		</v-tabs>
		
		<v-tabs-items v-model="tab"  style="overflow:auto; width:100%; height:100%">
		
			<template v-for="moduleTab in module.tabs" fill-height>
						
				<v-tab-item :key="moduleTab.uuid" style="overflow:auto; width:100%; height:100%;">
						<Module_Content :key="moduleTab.name" v-if="(moduleTab.type == 'content')" :module="moduleTab" :Application="Application" />					
						<Module_GLScene :key="moduleTab.name" v-if="(moduleTab.type == 'glscene')" :module="moduleTab" :Application="Application" />							
						<Module_Graphic :key="moduleTab.name" v-if="(moduleTab.type == 'graphic')" :module="moduleTab" :Application="Application" />							
						<Module_LayerView :key="moduleTab.name" v-if="(moduleTab.type == 'layerview')" :module="moduleTab" :Application="Application" />							
				</v-tab-item>
							
			</template>	
		</v-tabs-items>

	</v-card>	

</template>

<script>

	import Module_Content from "./AMCModule_Content.vue";
	import Module_GLScene from "./AMCModule_GLScene.vue";
	import Module_Graphic from "./AMCModule_Graphic.vue";
	import Module_LayerView from "./AMCModule_LayerView.vue";

	export default {
		props: ["Application", "module"],
	  
		components: {
			Module_Content,
			Module_GLScene,
			Module_Graphic,
			Module_LayerView,
		},
		
		data: () => ({
			tab: null
		})
		
	};
	
</script>