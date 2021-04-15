<template>

<div v-if="(moduleitem.type=='form')">  	
	
	<v-container dense>
				
		<template v-for="entity in moduleitem.entities">
			
			<v-row :key="entity.name" v-if="(entity.type=='edit')">
				<v-col cols="3"><v-label>{{ entity.caption }}:</v-label></v-col>
				<v-col cols="9"><v-text-field outlined single-line dense /></v-col>
			</v-row>

			<v-row :key="entity.name" v-if="(entity.type=='switch')" align="center">
				<v-col cols="3"><v-label>{{ entity.caption }}:</v-label></v-col>
				<v-col cols="9"><v-switch /></v-col>
			</v-row>

			<v-row :key="entity.name" v-if="(entity.type=='memo')">
				<v-col cols="3"><v-label>{{ entity.caption }}:</v-label></v-col>
				<v-col cols="9"><v-textarea outlined /></v-col>
			</v-row>

			<v-row :key="entity.name" v-if="(entity.type=='combobox')" align="center">
				<v-col cols="3"><v-label>{{ entity.caption }}:</v-label></v-col>
				<v-col cols="9"><v-select outlined /></v-col>
			</v-row>
							
		</template>
	
	</v-container>
	
</div>

</template>

<script>

	export default {
	  props: ["Application", "moduleitem"],

	  methods: {	
	  }
	};
	
</script>

