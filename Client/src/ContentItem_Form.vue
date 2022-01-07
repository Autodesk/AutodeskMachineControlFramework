<template>

<div v-if="(moduleitem.type=='form')">  	
	
	<v-container dense>
				
		<template v-for="entity in moduleitem.entities">
			
			<v-row dense no-gutters :key="entity.name" v-if="(entity.type=='edit')">
				<v-col cols="12"><v-text-field outlined dense v-model="entity.dataObject.value" :label="entity.caption" :disabled="entity.dataObject.disabled" :readonly="entity.dataObject.readonly" :prefix="entity.prefix" :suffix="entity.suffix"/></v-col>
			</v-row>

			<v-row dense no-gutters :key="entity.name" v-if="(entity.type=='switch')" align="center">
				<v-col cols="3"><v-label>{{ entity.caption }}:</v-label></v-col>
				<v-col cols="9"><v-switch @change="uiToggleSwitch (entity);" v-model="entity.dataObject.value"  :disabled="entity.dataObject.disabled" :readonly="entity.dataObject.readonly" /></v-col>
			</v-row>

			<v-row dense no-gutters :key="entity.name" v-if="(entity.type=='memo')">
				<v-col cols="12"><v-textarea outlined :label="entity.caption" v-model="entity.dataObject.value"  :disabled="entity.dataObject.disabled" :readonly="entity.dataObject.readonly" /></v-col>
			</v-row>

			<v-row dense no-gutters :key="entity.name" v-if="(entity.type=='combobox')" align="center">
				<v-col cols="12"><v-select outlined v-model="entity.dataObject.value" :label="entity.caption" :disabled="entity.dataObject.disabled" :readonly="entity.dataObject.readonly" /></v-col>
			</v-row>
							
		</template>
	
	</v-container>
	
</div>

</template>

<script>

	export default {
	  props: ["Application", "moduleitem"],

	  methods: {	
		uiToggleSwitch: function (switchentity) {
		
			if (switchentity.dataObject.value != switchentity.dataObject.remotevalue) {
				if (switchentity.changeevent != "") {
				
					console.log ("change event!");
				
					var formvalues = this.Application.assembleFormValues ([ switchentity.uuid ]);
					this.Application.triggerUIEvent (switchentity.changeevent, switchentity.uuid, formvalues);
				}
			}
		}
	  
	  }
	};
	
</script>

