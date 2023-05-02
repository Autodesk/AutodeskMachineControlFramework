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


class AMCApplicationItem_Custom_Properties extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_CustomProperties");
		
		this.values = {}
				
		this.updateFromJSON (itemJSON);
		
		this.setRefreshFlag ();
											
	}

	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		
		this.values = updateJSON;
				
		this.moduleInstance.callDataHasChanged ();
	}

		
}



class AMCApplicationItem_Custom_Event extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_CustomEvent");
		
		
		this.name = Assert.StringValue (itemJSON.name);
		this.parameters = Assert.ArrayValue (itemJSON.parameters);
		this.parameterNameMap = new Map ();
		for (let parameter of this.parameters) {
			this.parameterNameMap.set (parameter.name, parameter.uuid);			
		}
				
		this.updateFromJSON (itemJSON);
		
		this.setRefreshFlag ();
											
	}

	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);		
				
	}

	prepareUIEvent (parameters)
	{
		Assert.ObjectValue (parameters);
		let resultUIEventParameter = {};
		
		for (let defaultParameter of this.parameters) {			
			resultUIEventParameter [defaultParameter.uuid] = defaultParameter.defaultvalue;			
		}

		const objectEntries = Object.entries(parameters);
					
		for (let objectEntry of objectEntries) {
			let parameterName = objectEntry[0];
			let parameterValue = objectEntry[1];					
			if (this.parameterNameMap.has (parameterName)) {
				let uuid = this.parameterNameMap.get (parameterName);
				resultUIEventParameter [uuid] = parameterValue;
			}				
		} 
		
		return resultUIEventParameter;
		
		
		
	}
		
	

		
}




export default class AMCApplicationModule_Custom extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);				
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_Custom");
		
		Assert.ArrayValue (moduleJSON.items);
		this.items = [];
		this.propertiesitem = null;
		this.eventitems = new Map();

		for (let itemJSON of moduleJSON.items) {
			
			let item = null;
			
			if (itemJSON.type === "properties") {
				item = new AMCApplicationItem_Custom_Properties (this, itemJSON);
				this.propertiesitem = item;
			}

			if (itemJSON.type === "event") {
				item = new AMCApplicationItem_Custom_Event (this, itemJSON);
				this.eventitems.set (item.name, item);
			}
			

			if (item) {
				this.items.push (item);
				this.page.addItem (item);
			} else {
				throw "Item type not found: " + itemJSON.type;
			}
			
		}			
		
		
	}
	
	
	findEvent (name)
	{
		if (this.eventitems.has (name)) 
		{
			return this.eventitems.get (name); 
		}
		
		return null;
	}
		
}

