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


import * as Assert from "./AMCAsserts.js";
import * as Common from "./AMCCommon.js"


class AMCApplicationItem_LayerView_Platform extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_Platform");
		
		this.displayed_layer = 0;
		this.displayed_build = Common.nullUUID ();
		
		this.updateFromJSON (itemJSON);
		
		this.setRefreshFlag ();
											
	}

	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);		
		
		this.currentlayer = Assert.IntegerValue (updateJSON.currentlayer);
		this.builduuid = Assert.OptionalUUIDValue (updateJSON.builduuid);
		this.baseimageresource = Assert.OptionalUUIDValue (updateJSON.baseimageresource);
		this.sizex = Assert.NumberValue (updateJSON.sizex);
		this.sizey = Assert.NumberValue (updateJSON.sizey);
		this.originx = Assert.NumberValue (updateJSON.originx);
		this.originy = Assert.NumberValue (updateJSON.originy);
		
		this.moduleInstance.callDataHasChanged ();
	}

		
}




export default class AMCApplicationModule_LayerView extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);				
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_LayerView");
		
		Assert.ArrayValue (moduleJSON.items);
		this.items = [];
		this.platform = null;

		for (let itemJSON of moduleJSON.items) {
			
			let item = null;
			
			if (itemJSON.type === "platform") {
				item = new AMCApplicationItem_LayerView_Platform (this, itemJSON);
				this.platform = item;
			}
			
			if (item) {
				this.items.push (item);
				this.page.addItem (item);
			} else {
				throw "Item type not found: " + itemJSON.type;
			}
			
		}			
		
		
	}
		
}

