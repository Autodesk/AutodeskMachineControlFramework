<template>

<div v-if="(moduleitem.type=='buttongroup')" v-bind:style="moduleitem.cssstyle" > 
	<template v-for="button in moduleitem.buttons">
	
		<span :key="button.name" v-on:click.stop="uiModuleButtonClick (button);">	
			<v-btn color="primary" class="ml-1 mr-1" v-bind:style="moduleitem.buttoncssstyle" >
				<v-icon v-if="button.icon">{{ button.icon }}</v-icon>
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
					
			var formvalues = this.Application.assembleFormValues (button.eventformvalues);
			
			if (button.event != "") {		
				this.Application.triggerUIEvent (button.event, button.uuid, formvalues);
			}
			
			if (button.targetpage != "") {
				this.Application.changePage (button.targetpage);
			}
		
		}
	  },
	  
	  created () {
	  	  
	  }
	  
	};
	
</script>
