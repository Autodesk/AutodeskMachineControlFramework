<template>
	
	<v-container v-if="(module.type == 'horizontalsplit')" >

			<v-row dense no-gutters class="fill-height">

			<template v-for="moduleSection in module.sections">
									
					<v-col dense no-gutters :key="moduleSection.uuid" style="overflow: auto" class="fill-height">
							<Module_Content :key="moduleSection.name" v-if="(moduleSection.type == 'content')" :module="moduleSection" :Application="Application" />					
							<Module_Tabs :key="moduleSection.name" v-if="(moduleSection.type == 'tabs')" :module="moduleSection" :Application="Application" />							
							<Module_VerticalSplit :key="moduleSection.name" v-if="(moduleSection.type == 'verticalsplit')" :module="moduleSection" :Application="Application" />							
							<Module_HorizontalSplit :key="moduleSection.name" v-if="(moduleSection.type == 'horizontalsplit')" :module="moduleSection" :Application="Application" />							
					</v-col>
					
			</template>	

			</v-row>	

	</v-container>

</template>

<script>

	import Module_Content from "./Module_Content.vue";
	import Module_Tabs from "./Module_Tabs.vue";
	import Module_VerticalSplit from "./Module_VerticalSplit.vue";
	import Module_HorizontalSplit from "./Module_HorizontalSplit.vue";

	export default {
		props: ["Application", "module"],
	  
		components: {
			Module_Content,
			Module_Tabs,
			Module_VerticalSplit,
			Module_HorizontalSplit
		},
		
		data: () => ({
			tab: null
		})
		
	};
	
</script>