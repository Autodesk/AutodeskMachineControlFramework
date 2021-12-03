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
						<Module_Tabs :key="moduleTab.name" v-if="(moduleTab.type == 'tabs')" :module="moduleTab" :Application="Application" />							
						<Module_Grid :key="moduleTab.name" v-if="(moduleTab.type == 'grid')" :module="moduleTab" :Application="Application" />							
				</v-tab-item>
							
			</template>	
		</v-tabs-items>

	</v-card>	

</template>

<script>

	import Module_Content from "./Module_Content.vue";
	import Module_Tabs from "./Module_Tabs.vue";
	import Module_Grid from "./Module_Grid.vue";

	export default {
		props: ["Application", "module"],
	  
		components: {
			Module_Content,
			Module_Tabs,
			Module_Grid,
		},
		
		data: () => ({
			tab: null
		})
		
	};
	
</script>