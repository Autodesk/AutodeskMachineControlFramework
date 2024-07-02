<!--

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the Autodesk Inc. nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

!-->

<template>

<div v-if="(moduleitem.type=='upload')">  	
	<v-file-input accept=".3mf,.3mftoolpath" show-size full-width v-model="moduleitem.state.chosenFile" v-bind:label="moduleitem.uploadcaption" v-bind:messages="moduleitem.state.messages" @change="uiUploadStart (moduleitem)"></v-file-input>
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