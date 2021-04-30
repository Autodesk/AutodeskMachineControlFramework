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
					item.state.idcounter = item.state.idcounter + 1;
					if (item.state.chosenFile) {
					
						if (item.state.uploadid === 0) {
							item.state.messages = ["Uploading..."];					
					
						} else {
							item.state.messages = ["Replacing upload..."];
						}
						item.state.uploadid = item.state.idcounter; 
						
						this.Application.performJobUpload (item.uuid, item.state, item.state.uploadid, item.state.chosenFile, item.uploadsuccesspage);
						
					} else {
					
						if (!(item.state.uploadid === 0)) {
							item.state.uploadid = 0; 
							item.state.messages = ["Canceled upload..."];
						}
					
					}
					
				
			},	
			
	  }
	  
	};

</script>