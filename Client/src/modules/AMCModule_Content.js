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

import AMCUploadState from "../common/AMCImplementation_UploadState.js"


class AMCApplicationItem_Content_ParameterList extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_ParameterList");
		
		this.entries = [];
		
		// TODO: check validity
		this.headers = itemJSON.headers;
		
		this.stateid = 1;
		
		this.loadingtext = "";
		this.entriesperpage = 25;
		
		this.updateFromJSON (itemJSON);
		
		this.setRefreshFlag ();
		
	}
		
	
	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		Assert.ArrayValue (updateJSON.entries);
		
		if (updateJSON.loadingtext)
			this.loadingtext = Assert.StringValue (updateJSON.loadingtext);
		if (updateJSON.entriesperpage)
			this.entriesperpage = Assert.IntegerValue (updateJSON.entriesperpage);

		let oldEntryCount = this.entries.length;
		for (let index = 0; index < oldEntryCount; index++) {
			this.entries.pop();
		}

		for (let entry of updateJSON.entries) {
			this.entries.push(entry);
		}
		
	}
	
}


class AMCApplicationItem_Content_Upload extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_Upload");
		
		this.uploadclass = Assert.IdentifierString (itemJSON.uploadclass);
		this.uploadsuccessevent = Assert.OptionalIdentifierString (itemJSON.uploadsuccessevent);
		this.uploadfailureevent = Assert.OptionalIdentifierString (itemJSON.uploadfailureevent);
		this.uploadcaption = Assert.StringValue (itemJSON.uploadcaption);
		this.uploadfilename = "";
		this.uploadisinitial = 1;
		this.uploadissaving = 0;
				
		this.state = new AMCUploadState (this.uuid);
		
		this.setRefreshFlag ();
		
	}
		
}


class AMCApplicationItem_Content_BuildList extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_BuildList");

		this.entries = [];
				
		// TODO: check validity
		this.headers = itemJSON.headers;
		this.loadingtext = "";
		this.selectevent = "";
		this.selectionvalueuuid = Common.nullUUID ();
		this.entriesperpage = 25;
		
		this.updateFromJSON (itemJSON);
		
		this.setRefreshFlag ();
								
	}
	
	
	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		Assert.ArrayValue (updateJSON.entries);
		
		if (updateJSON.loadingtext)
			this.loadingtext = Assert.StringValue (updateJSON.loadingtext);
		if (updateJSON.selectevent)
			this.selectevent = Assert.IdentifierString (updateJSON.selectevent);		
		if (updateJSON.selectionvalueuuid)
			this.selectionvalueuuid = Assert.IdentifierString (updateJSON.selectionvalueuuid);
		if (updateJSON.entriesperpage)
			this.entriesperpage = Assert.IntegerValue (updateJSON.entriesperpage);

		let oldEntryCount = this.entries.length;
		for (let index = 0; index < oldEntryCount; index++) {
			this.entries.pop();
		}

		for (let entry of updateJSON.entries) {
			this.entries.push(entry);
		}
		
	}
	
		
}


class AMCApplicationItem_Content_AlertList extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_AlertList");

		this.entries = [];
				
		// TODO: check validity
		this.headers = itemJSON.headers;
		this.loadingtext = "";
		this.selectevent = "";
		this.selectionvalueuuid = Common.nullUUID ();
		this.entriesperpage = 25;
		
		this.updateFromJSON (itemJSON);
		
		this.setRefreshFlag ();
								
	}
	
	
	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		Assert.ArrayValue (updateJSON.entries);
		
		if (updateJSON.loadingtext)
			this.loadingtext = Assert.StringValue (updateJSON.loadingtext);
		if (updateJSON.selectevent)
			this.selectevent = Assert.IdentifierString (updateJSON.selectevent);		
		if (updateJSON.selectionvalueuuid)
			this.selectionvalueuuid = Assert.IdentifierString (updateJSON.selectionvalueuuid);
		if (updateJSON.entriesperpage)
			this.entriesperpage = Assert.IntegerValue (updateJSON.entriesperpage);

		let oldEntryCount = this.entries.length;
		for (let index = 0; index < oldEntryCount; index++) {
			this.entries.pop();
		}

		for (let entry of updateJSON.entries) {
			this.entries.push(entry);
		}
		
	}
	
	

		
}


class AMCApplicationItem_Content_Paragraph extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_Paragraph");
		
		this.text = itemJSON.text;
		
	}
		
}


class AMCApplicationItem_Content_Image extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_Image");
		
		this.aspectratio = Assert.NumberValue (itemJSON.aspectratio);		
		if (itemJSON.maxwidth) 
			this.maxwidth = Assert.NumberValue (itemJSON.maxwidth);
		if (itemJSON.maxheight) 
			this.maxheight = Assert.NumberValue (itemJSON.maxheight);
		
	}
		
}


class AMCApplicationItem_Content_Chart extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_Chart");
		
		this.dataseries = Common.nullUUID ();
		this.version = 0;
		this.loadeddataseries = Common.nullUUID ();
		this.loadedversion = 0;
		
		this.onChartDataUpdated = null;
		this.chartData = [];
		
		this.updateFromJSON (itemJSON);
		
		this.setRefreshFlag ();
	}

	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		
		this.dataseries = Assert.UUIDValue (updateJSON.dataseries);
		if (updateJSON.version) {
			this.version = Assert.IntegerValue (updateJSON.version);
		} else {
			this.version = 0;
		}
		
		if ((this.loadeddataseries != this.dataseries) || (this.loadedversion != this.version)) {
			this.loadeddataseries = this.dataseries;
			this.loadedversion = this.version;
			this.refreshChartData ();
		}
				
	}
	
	
	refreshChartData ()
	{
		let application = this.getApplication ();
		let normalizedUUID = this.dataseries;
	
		application.axiosGetArrayBufferRequest("/ui/chart/" + normalizedUUID)
				.then(responseData => {
					var floatView = new Float32Array(responseData.data);
					let dataLength = floatView.length;
					let pointCount = dataLength / 2;
										
					this.chartData = [];
					for (let index = 0; index < pointCount; index++) {
						let xvalue = floatView[index * 2];
						let yvalue = floatView[index * 2 + 1];
						this.chartData.push ([xvalue, yvalue]);
					}
					
					if (this.onChartDataUpdated)
						this.onChartDataUpdated ();
										
				})
				.catch(err => {
					this.loadeddataseries = Common.nullUUID ();
					
					if (err.response) {
						console.log (err.response);
					} else {
						console.log ("fatal error while retrieving chart data " + normalizedUUID);
					}
				});			
	
	}

}


class AMCApplicationItem_Content_Form extends Common.AMCApplicationItem {
	
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


class AMCApplicationItem_Content_ButtonGroup extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{		
		Assert.ObjectValue (itemJSON);		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_ButtonGroup");
		
		Assert.ArrayValue (itemJSON.buttons);		
		// TODO: parse input
		this.buttons = itemJSON.buttons;
		
		this.buttoncssstyle = ""; 
		this.cssstyle = "";
		
		if (itemJSON.buttondistribution == "rightaligned") {
			this.cssstyle = this.cssstyle + "text-align: right;"; 
		}

		if (itemJSON.buttondistribution == "leftaligned") {
			this.cssstyle = this.cssstyle + "text-align: left;"; 
		}

		if (itemJSON.buttondistribution == "centered") {
			this.cssstyle = this.cssstyle + "text-align: centered;"; 
		}

		if (itemJSON.buttondistribution == "equal") {
			
			let buttoncount = this.buttons.length;
			if (buttoncount > 0) {
				this.buttoncssstyle = this.buttoncssstyle + "min-width: calc(" + (100.0 / buttoncount) + "% - 10px);"; 
			}
			
			this.cssstyle = this.cssstyle + "text-align: center;";	
		}
		
		this.setRefreshFlag ();
		
				
	}
	
	
	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		
		if (updateJSON.buttons) {
			// TODO: parse input	
			this.buttons = updateJSON.buttons;
		}
		
	}
		
}




export default class AMCApplicationModule_Content extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);		
		Assert.ArrayValue (moduleJSON.items);
		
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_Content");
		
		this.headline = Assert.StringValue (moduleJSON.headline);
		this.title = Assert.StringValue (moduleJSON.title);
		this.subtitle = Assert.StringValue (moduleJSON.subtitle);
		
		this.items = [];
		
		
		for (let itemJSON of moduleJSON.items) {
			
			let item = null;
			
			if (itemJSON.type === "parameterlist") 
				item = new AMCApplicationItem_Content_ParameterList (this, itemJSON);

			if (itemJSON.type === "upload") 
				item = new AMCApplicationItem_Content_Upload (this, itemJSON);
			
			if (itemJSON.type === "buildlist") 
				item = new AMCApplicationItem_Content_BuildList (this, itemJSON);
			
			if (itemJSON.type === "alertlist") 
				item = new AMCApplicationItem_Content_AlertList (this, itemJSON);

			if (itemJSON.type === "buttongroup") 
				item = new AMCApplicationItem_Content_ButtonGroup (this, itemJSON);
			
			if (itemJSON.type === "paragraph") 
				item = new AMCApplicationItem_Content_Paragraph (this, itemJSON);
			
			if (itemJSON.type === "image") 
				item = new AMCApplicationItem_Content_Image (this, itemJSON);

			if (itemJSON.type === "chart") 
				item = new AMCApplicationItem_Content_Chart (this, itemJSON);

			if (itemJSON.type === "form") 
				item = new AMCApplicationItem_Content_Form (this, itemJSON);
			
			if (item) {
				this.items.push (item);
				this.page.addItem (item);
			} else {
				throw "Item type not found: " + itemJSON.type;
			}
			
		}			
				
	}
	
	
}

