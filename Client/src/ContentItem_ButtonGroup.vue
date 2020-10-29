<template>

<div v-if="(moduleitem.type=='buttongroup')">  	
	<v-spacer></v-spacer>
	
	<template v-for="button in moduleitem.buttons">
	
		<v-btn color="primary" :key="button.name" v-on:click.stop="uiModuleButtonClick (button);">
			{{ button.caption }}
		</v-btn>
	
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
