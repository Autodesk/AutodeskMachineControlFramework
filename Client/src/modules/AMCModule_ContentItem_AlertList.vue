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

<div v-if="(moduleitem.type=='alertlist')">  	

	<v-data-table
		:headers="moduleitem.headers"
		:items="moduleitem.entries"
		:items-per-page="moduleitem.entriesperpage"
		class="elevation-1"
		disable-pagination
		hide-default-footer
		search 
		width="100%"
		loadingText="moduleitem.loadingtext"
		@click:row="uiModuleAlertListClick"
		>													
										
	</v-data-table>		

</div>

</template>

<script>

	export default {
	  props: ["Application", "moduleitem"],
	  
	  methods: {	
	  
		uiModuleAlertListClick: function (item) {
			if (item) {
				if (this.moduleitem.selectevent && this.moduleitem.selectionvalueuuid) {
					var eventValues = {}
					eventValues[this.moduleitem.selectionvalueuuid] = item.buildUUID;
			
					this.Application.triggerUIEvent (this.moduleitem.selectevent, this.moduleitem.uuid, eventValues);
				}
			}
		},
				
	  }
	};
	
</script>