<template>

	<v-card width="99%" v-if="(module.type == 'content')">
		<v-card-text>
			<div v-if="module.title != ''"> {{ module.headline }}</div>
			<p v-if="module.title != ''" class="display-1 text--primary">
				{{ module.title }}
			</p>
			<p v-if="module.subtitle != ''" >{{ module.subtitle }}</p>

			<div v-if="module.items.length > 0" class="text--primary">
			
				<template v-for="moduleitem in module.items">
					
					<ContentItem_Paragraph :key="moduleitem.uuid" v-if="(moduleitem.type=='paragraph')" :moduleitem="moduleitem" :Application="Application" />
					
					<ContentItem_LayerView :key="moduleitem.uuid" v-if="(moduleitem.type=='layerview')" :moduleitem="moduleitem" :Application="Application" />
					
					<ContentItem_Image :key="moduleitem.uuid" v-if="(moduleitem.type=='image')" :moduleitem="moduleitem" :Application="Application" />
					
					<ContentItem_Upload :key="moduleitem.uuid" v-if="(moduleitem.type=='upload')" :moduleitem="moduleitem" :Application="Application" />
																
					<ContentItem_BuildList :key="moduleitem.uuid" v-if="(moduleitem.type=='buildlist')" :moduleitem="moduleitem" :Application="Application" />										
																																			
					<ContentItem_ParameterList :key="moduleitem.uuid" v-if="(moduleitem.type=='parameterlist')" :moduleitem="moduleitem" :Application="Application" />
									
					<ContentItem_ButtonGroup :key="moduleitem.uuid" v-if="(moduleitem.type=='buttongroup')" :moduleitem="moduleitem" :Application="Application" />

					<ContentItem_Form :key="moduleitem.uuid" v-if="(moduleitem.type=='form')" :moduleitem="moduleitem" :Application="Application" />
						
				</template>
					
			</div>
			
												
		</v-card-text>


	</v-card>

</template>

<script>

	import ContentItem_LayerView from './ContentItem_LayerView.vue';
	import ContentItem_Paragraph from './ContentItem_Paragraph.vue';
	import ContentItem_Image from './ContentItem_Image.vue';
	import ContentItem_Upload from './ContentItem_Upload.vue';
	import ContentItem_BuildList from './ContentItem_BuildList.vue';
	import ContentItem_ParameterList from './ContentItem_ParameterList.vue';
	import ContentItem_ButtonGroup from './ContentItem_ButtonGroup.vue';
	import ContentItem_Form from './ContentItem_Form.vue';


	export default {
		props: ["Application", "module"],
	  
		components: {
			ContentItem_LayerView,
			ContentItem_Paragraph,
			ContentItem_Image,
			ContentItem_Upload,
			ContentItem_BuildList,
			ContentItem_ParameterList,
			ContentItem_ButtonGroup,
			ContentItem_Form
		}		
		
	};
	
</script>