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

<div v-if="(moduleitem.type=='form')">  	
	
	<v-container dense>				
		<template v-for="entity in moduleitem.entities">			
			<v-row dense no-gutters :key="entity.name" v-if="(entity.type=='edit')">
				<v-col cols="12"><v-text-field outlined dense v-model="entity.dataObject.value" :label="entity.caption" :disabled="entity.dataObject.disabled" :readonly="entity.dataObject.readonly" :prefix="entity.prefix" :suffix="entity.suffix" :rules="checkRules (entity)"/></v-col>
			</v-row>

			<v-row dense no-gutters :key="entity.name" v-if="(entity.type=='switch')">
				<v-col cols="10"><v-subheader>{{ entity.caption }}</v-subheader></v-col>
				<v-col cols="2"><v-switch dense hide-details persistent-hint inset v-model="entity.dataObject.value" @change="uiToggleSwitch (entity)" /></v-col>
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
		
			if (switchentity.dataObject) {
		
				if (switchentity.dataObject.value != switchentity.dataObject.remotevalue) {
					if (switchentity.changeevent != "") {
					
						console.log ("change event!");
					
						var formvalues = this.Application.assembleFormValues ([ switchentity.uuid ]);
						this.Application.triggerUIEvent (switchentity.changeevent, switchentity.uuid, formvalues);
					}
				}
			}
		},
		
		checkRules (editentity) {
		
		
			const rules = [];		
			
			if (editentity) {
			
				if (editentity.validation === "double") {
			
					rules.push((value) => {
						return !!value || editentity.validationmessage;
					});
													
					rules.push((value) => {
						return !isNaN(value) || editentity.validationmessage;
					});
								
					rules.push((value) => {
						return ((value >= editentity.minvalue) && (value <= editentity.maxvalue)) || editentity.validationmessage;
					});
										
				}
				
				if (editentity.validation === "string") {
			
					rules.push((value) => {
						return ((value.length >= editentity.minlength) && (value.length >= editentity.maxlength)) || editentity.validationmessage;
					});
																							
				}
				
			}
			
			return rules;
		}
	  
	  }
	};
	
</script>

