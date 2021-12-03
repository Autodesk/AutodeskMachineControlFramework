<template>
	
	<div v-if="(module.type == 'grid')" fluid v-bind:style="module.cssstyle">
		
			<template v-for="moduleSection in module.sections">

				<div :key="moduleSection.uuid" tile flat v-bind:style="'overflow:auto; grid-area:' + moduleSection.name">
					<Module_Content :key="moduleSection.name" v-if="(moduleSection.type == 'content')" :module="moduleSection" :Application="Application" />					
					<Module_Tabs :key="moduleSection.name" v-if="(moduleSection.type == 'tabs')" :module="moduleSection" :Application="Application" />							
				</div>
						
			</template>	
		
	</div>

</template>

<script>

	import Module_Content from "./Module_Content.vue";
	import Module_Tabs from "./Module_Tabs.vue";

	export default {
		props: ["Application", "module"],
	  
		components: {
			Module_Content,
			Module_Tabs,
		},
		
		data: () => ({				
			
		})
		
	};
	
</script>