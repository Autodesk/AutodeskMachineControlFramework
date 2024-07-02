/*++

Copyright (C) 2021 Autodesk Inc.

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

 */


import * as Assert from "../common/AMCAsserts.js";
import * as Common from "../common/AMCCommon.js"


export default class AMCApplicationItem_Content_Form extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_Form");
				
		Assert.ArrayValue (itemJSON.entities);		
		// TODO: parse input
		this.entities = itemJSON.entities;
				
		for (let entity of this.entities) {

			entity.dataObject = {
				uuid: entity.uuid,
				value: entity.value,
				remotevalue: entity.value,
				disabled: entity.disabled,
				readonly: entity.readonly,
				changeevent: entity.changeevent,
				validation: entity.validation,
				validationmessage: entity.validationmessage,
				minvalue: entity.minvalue,
				maxvalue: entity.maxvalue,
				isProgrammaticChange: false
				
			};
			
			this.moduleInstance.page.application.AppContent.FormEntityMap.set (entity.uuid, entity);
		}
				
		this.setRefreshFlag ();
										
	}
	
	
	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		
		if (updateJSON.entities) {
			Assert.ArrayValue (updateJSON.entities);
			
			for (let entityJSON of updateJSON.entities) {
				if (this.moduleInstance.page.application.AppContent.FormEntityMap.has (entityJSON.uuid)) {
					let entity = this.moduleInstance.page.application.AppContent.FormEntityMap.get (entityJSON.uuid);					
					let dataObject = entity.dataObject;
					
					Assert.ObjectValue (dataObject);
					
					if (dataObject.remotevalue != entityJSON.value) {
						dataObject.value = entityJSON.value;
						dataObject.isProgrammaticChange = true;
					} else {
						dataObject.isProgrammaticChange = false;
					}
					dataObject.remotevalue = entityJSON.value;
					dataObject.prefix = entityJSON.prefix;
					dataObject.suffix = entityJSON.suffix;
					dataObject.readonly = entityJSON.readonly;
					dataObject.disabled = entityJSON.disabled;
					dataObject.changeevent = entityJSON.changeevent;
					
				}
				
			} 
		}
		
				
	}
	
		
}