<template>

<div v-if="(moduleitem.type=='upload')">  	
	<v-file-input accept=".3mf" show-size full-width v-model="moduleitem.state.chosenFile" v-bind:label="moduleitem.uploadcaption" v-bind:messages="moduleitem.state.messages" @change="uiUploadStart (moduleitem)"></v-file-input>
</div>

</template>

<script>

	export default {
	  props: ["Application", "moduleitem"],

	  methods: {	
	  
			uiUploadStart: function (item) {
														
					if (item) {
						if (item.state) {
							if (item.state.chosenFile) {
								item.state.generateUploadID ();
								this.Application.performJobUpload (item.state, item.uploadsuccessevent, item.uploadfailureevent);
							} else {		
								item.state.cancelUpload ();
							}
								
						}					
					}
					
					
						item.state.uploadid = item.state.idcounter; 
						
						
					
				
			},	
			
	  }
	  
	};

</script>