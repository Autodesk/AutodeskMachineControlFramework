<template>

<v-card v-if="(module.type == 'tabs')"  width="99%" >
	
	<v-tabs  v-model="tab">
			
		<template v-for="moduleTab in module.tabs">
					
			<v-tab :key="moduleTab.uuid">
				{{ moduleTab.caption }}
			</v-tab>
						
		</template>
		
	</v-tabs>
	
    <v-tabs-items v-model="tab">
	
		<template v-for="moduleTab in module.tabs">
					
			<v-tab-item :key="moduleTab.uuid">
				<Module_Content :key="moduleTab.name" v-if="(moduleTab.type == 'content')" :module="moduleTab" :Application="Application" />					
				<Module_Tabs :key="moduleTab.name" v-if="(moduleTab.type == 'tabs')" :module="moduleTab" :Application="Application" />	
			</v-tab-item>
						
		</template>	
	</v-tabs-items>

</v-card>	

</template>

<script>

	import Module_Content from "./Module_Content.vue";
	import Module_Tabs from "./Module_Tabs.vue";

	export default {
		props: ["Application", "module"],
	  
		components: {
			Module_Content,
			Module_Tabs
		},
		
		data: () => ({
			tab: null
		})
		
	};
	
</script>