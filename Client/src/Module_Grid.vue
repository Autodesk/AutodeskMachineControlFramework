<template>
	
	<div v-if="(module.type == 'grid')" v-bind:style="module.cssstyle">
		
			<template v-for="moduleSection in module.sections">

				<div :key="moduleSection.uuid" tile flat v-bind:style="moduleSection.cssstyle">
					<Module_Content :key="moduleSection.name" v-if="(moduleSection.type == 'content')" :module="moduleSection" :Application="Application" />
					<Module_Tabs :key="moduleSection.name" v-if="(moduleSection.type == 'tabs')" :module="moduleSection" :Application="Application" />
					<Module_GLScene :key="moduleSection.name" v-if="(moduleSection.type == 'glscene')" :module="moduleSection" :Application="Application" />
					<Module_Graphic :key="moduleSection.name" v-if="(moduleSection.type == 'graphic')" :module="moduleSection" :Application="Application" />
				</div>
						
			</template>	
		
	</div>

</template>

<script>

	import Module_Content from "./Module_Content.vue";
	import Module_Tabs from "./Module_Tabs.vue";
	import Module_GLScene from "./Module_GLScene.vue";
	import Module_Graphic from "./Module_Graphic.vue";

	export default {
		props: ["Application", "module"],
	  
		components: {
			Module_Content,
			Module_Tabs,
			Module_GLScene,
			Module_Graphic
		},
		
		data: () => ({				
			
		})
		
	};
	
</script>