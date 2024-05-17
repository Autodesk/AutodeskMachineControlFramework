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

<div v-if="(moduleitem.type=='buildlist')">  	
<v-container>
    <!-- Data table for build jobs -->
    <v-data-table
      :headers="moduleitem.headers"
      :items="moduleitem.entries"
      :items-per-page="moduleitem.entriesperpage"
      class="elevation-1"
	  search
	  loadingText="moduleitem.loadingtext"
      :hide-default-footer="false"
    >
      <template v-slot:[`item.buildThumbnail`]="{ item }">        
		 <v-img v-if="(item.buildThumbnail != '00000000-0000-0000-0000-000000000000') && item.buildThumbnail" :height="moduleitem.thumbnailheight" :width="moduleitem.thumbnailwidth" :aspect-ratio="moduleitem.thumbnailaspectratio" contain :src="Application.getImageURL (item.buildThumbnail)" v-on:click.stop="uiModuleBuildListDetailsClick (item)" />	   	  
      </template>
      <template v-slot:[`item.buildName`]="{ item }">
		<v-card class="ma-2" v-on:click.stop="uiModuleBuildListDetailsClick (item)">
          <v-card-title>{{ item.buildName }}</v-card-title>
          <v-card-subtitle>{{ item.buildLayers }} layers, printed {{ item.buildPrintCount }} times
          </v-card-subtitle>
          <v-card-text>
            
          </v-card-text>
          <v-card-actions>
		  
			<template v-for="button in moduleitem.entrybuttons">
	
				<span :key="button.uuid" v-on:click.stop="uiModuleBuildListHistoryClick (button, item);">	
					<v-btn text color="primary" >
						<v-icon v-if="button.icon">{{ button.icon }}</v-icon>
						{{ button.caption }}
					</v-btn>		
				
				</span>	
			</template>		  
			
          </v-card-actions>
        </v-card>
        
      </template>
      <template v-slot:[`item.buildTimestamp`]="{ item }">
			<div>
				<p class="text-center">{{ formatDateTime (item.buildTimestamp) }}<br>uploaded by {{ item.buildUser }}</p>
				<p class="grey-lighten-4 opacity-80 font-weight-light text-center">{{ item.buildUUID }}</p> 				
			</div>
      </template>
    </v-data-table>
  </v-container>
</div>

</template>

<script>

	export default {
	  props: ["Application", "moduleitem"],
	  
	  methods: {	
	  
		formatDateTime(timeString) {
			const date = new Date(timeString);
			return new Intl.DateTimeFormat('en-US', {
				year: 'numeric',
				month: 'short',
				day: 'numeric',
				hour: '2-digit',
				minute: '2-digit',
				second: '2-digit',
				hour12: false
			}).format(date);
		},

	  
		uiModuleBuildListClick: function (item) {
			if (item) {
				if (this.moduleitem.selectevent && this.moduleitem.selectionvalueuuid) {
				
										
					var eventValues = {}
					eventValues[this.moduleitem.selectionvalueuuid] = item.buildUUID;
			
					this.Application.triggerUIEvent (this.moduleitem.selectevent, this.moduleitem.uuid, eventValues);
				}
			}
		},
				
		uiModuleBuildListDetailsClick: function (item) {
			if (item) {
				if (this.moduleitem.selectevent && this.moduleitem.selectionvalueuuid) {					
					
					var eventValues = {}
					eventValues[this.moduleitem.selectionvalueuuid] = item.buildUUID;
			
					this.Application.triggerUIEvent (this.moduleitem.selectevent, this.moduleitem.uuid, eventValues);
				}
			}
		},

		uiModuleBuildListHistoryClick: function (button, item) {
			if (item && button) {
				if (button.selectevent && this.moduleitem.selectionvalueuuid && this.moduleitem.buttonvalueuuid) {
				
					var eventValues = {}							
					eventValues[this.moduleitem.selectionvalueuuid] = item.buildUUID;
					eventValues[this.moduleitem.buttonvalueuuid] = button.uuid;
					
					//alert ("Item UUID: " + item.buildUUID);
					//alert ("Button UUID: " + button.uuid);
						
					this.Application.triggerUIEvent (button.selectevent, this.moduleitem.uuid, eventValues);
				}
			}
		},

	  }
	};
	
</script>