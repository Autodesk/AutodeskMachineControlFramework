<template>
	
	<v-container v-if="(module.type == 'verticalsplit')" style="display:block; height:100%; width:100%">
		
			<template v-for="moduleSection in module.sections">
						
				<v-row dense no-gutters :key="moduleSection.uuid" style="width:100%; height:50%;">   
				
					<v-col dense no-gutters style="width: 100%; height: 100%; overflow:auto; ">
						<Module_Content :key="moduleSection.name" v-if="(moduleSection.type == 'content')" :module="moduleSection" :Application="Application" />					
						<Module_Tabs :key="moduleSection.name" v-if="(moduleSection.type == 'tabs')" :module="moduleSection" :Application="Application" />							
						<Module_VerticalSplit :key="moduleSection.name" v-if="(moduleSection.type == 'verticalsplit')" :module="moduleSection" :Application="Application" />							
						<Module_HorizontalSplit :key="moduleSection.name" v-if="(moduleSection.type == 'horizontalsplit')" :module="moduleSection" :Application="Application" />							
					</v-col>

				</v-row>	

			</template>	
		
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
		})
		
	};
	
</script>