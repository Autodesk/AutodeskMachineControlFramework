<template>

<div v-if="(moduleitem.type=='buttongroup')" class="text-right">  	
	<v-spacer></v-spacer>
		
	<template v-for="button in moduleitem.buttons">
	
		<span :key="button.name" v-on:click.stop="uiModuleButtonClick (button);" class="pa-2">	
			<v-btn color="primary">
				{{ button.caption }}
			</v-btn>			
		</span>
		
		
	
	</template>
	
</div>

</template>

<script>

	export default {
	  props: ["Application", "moduleitem"],

	  methods: {	
		uiModuleButtonClick: function (button) {
					
			var contextuuid = this.Application.AppState.activeObject;
			
			if (button.event != "") {		
				this.Application.triggerUIEvent (button.event, button.uuid, contextuuid);
			}
			
			if (button.targetpage != "") {
				this.Application.changePage (button.targetpage);
			}
		
		}
	  }
	};
	
</script>
