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


class AMCApplicationItem_LayerView_Platform extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_Platform");
		this.usesV2Frontend = true;
		
		this.displayed_layer = 0;
		this.displayed_build = Common.nullUUID ();

		this.colors = {
			background: "#ffffff", grid: "#e0e0e0",
			contour: "#00aa88", hatch: "#cc88cc", travel: "#aaaaaa"
		};
		this.darkcolors = {
			background: "#1a1a2e", grid: "#333344",
			contour: "#33ddaa", hatch: "#dd99dd", travel: "#555566"
		};

		this.dark_baseimageresource = "";
		
		this.updateFromJSON (itemJSON);
		
		this.setRefreshFlag ();
											
	}

	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);		
		
		this.currentlayer = Assert.IntegerValue (updateJSON.currentlayer);
		this.layercount = Assert.OptionalIntegerValue (updateJSON.layercount);
		this.builduuid = Assert.OptionalUUIDValue (updateJSON.builduuid);
		this.scatterplotuuid = Assert.OptionalUUIDValue (updateJSON.scatterplotuuid);
		this.baseimageresource = Assert.OptionalUUIDValue (updateJSON.baseimageresource);
		this.sizex = Assert.NumberValue (updateJSON.sizex);
		this.sizey = Assert.NumberValue (updateJSON.sizey);
		this.originx = Assert.NumberValue (updateJSON.originx);
		this.originy = Assert.NumberValue (updateJSON.originy);
		this.labelvisible = Assert.IntegerValue (updateJSON.labelvisible);
		this.labelcaption = Assert.StringValue (updateJSON.labelcaption);
		this.labelicon = Assert.StringValue (updateJSON.labelicon);
		this.sliderchangeevent = Assert.StringValue (updateJSON.sliderchangeevent);
		this.sliderfixed = Assert.IntegerValue (updateJSON.sliderfixed);
		
		this.moduleInstance.callDataHasChanged ();
	}

	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return true;

		if (attrs.currentlayer !== undefined) {
			let currentlayer = parseInt (attrs.currentlayer);
			if (!isNaN (currentlayer))
				this.currentlayer = currentlayer;
		}
		if (attrs.layercount !== undefined) {
			let layercount = parseInt (attrs.layercount);
			if (!isNaN (layercount))
				this.layercount = layercount;
		}
		if (attrs.builduuid !== undefined)
			this.builduuid = attrs.builduuid;
		if (attrs.scatterplotuuid !== undefined)
			this.scatterplotuuid = attrs.scatterplotuuid;
		if (attrs.baseimageresource !== undefined)
			this.baseimageresource = attrs.baseimageresource;
		if (attrs.dark_baseimageresource !== undefined)
			this.dark_baseimageresource = attrs.dark_baseimageresource;
		if (attrs.sizex !== undefined) {
			let sizex = parseFloat (attrs.sizex);
			if (!isNaN (sizex))
				this.sizex = sizex;
		}
		if (attrs.sizey !== undefined) {
			let sizey = parseFloat (attrs.sizey);
			if (!isNaN (sizey))
				this.sizey = sizey;
		}
		if (attrs.originx !== undefined) {
			let originx = parseFloat (attrs.originx);
			if (!isNaN (originx))
				this.originx = originx;
		}
		if (attrs.originy !== undefined) {
			let originy = parseFloat (attrs.originy);
			if (!isNaN (originy))
				this.originy = originy;
		}
		if (attrs.labelvisible !== undefined)
			this.labelvisible = (attrs.labelvisible === true || attrs.labelvisible === "1" || attrs.labelvisible === "true");
		if (attrs.labelcaption !== undefined)
			this.labelcaption = attrs.labelcaption;
		if (attrs.labelicon !== undefined)
			this.labelicon = attrs.labelicon;
		if (attrs.sliderchangeevent !== undefined)
			this.sliderchangeevent = attrs.sliderchangeevent;
		if (attrs.sliderfixed !== undefined)
			this.sliderfixed = (attrs.sliderfixed === true || attrs.sliderfixed === "1" || attrs.sliderfixed === "true");

		const colorKeys = ["background", "grid", "contour", "hatch", "travel"];
		for (let k of colorKeys) {
			if (attrs["color_" + k] !== undefined)
				this.colors[k] = attrs["color_" + k];
			if (attrs["darkcolor_" + k] !== undefined)
				this.darkcolors[k] = attrs["darkcolor_" + k];
		}

		this.moduleInstance.callDataHasChanged ();
		return true;
	}

		
}




export default class AMCApplicationModule_LayerView extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);				
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_LayerView");
		this.usesV2Frontend = true;
		
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

	updateFromV2Attributes (attrs)
	{
		if (attrs) {
			if (attrs.caption !== undefined)
				this.caption = attrs.caption;
			if (attrs.visible !== undefined)
				this.visible = (attrs.visible === true || attrs.visible === "1" || attrs.visible === "true");
		}

		// Pass merged module-level attrs (which include all platform data) to the platform item.
		// Do NOT overwrite frontendLookup[platform.uuid] — we want the platform to fall through
		// to the legacy /ui/contentitem/ polling path so it receives the dynamically computed
		// layercount value that the v2 attribute system cannot easily express.
		if (this.platform)
			this.platform.updateFromV2Attributes (attrs);

		return true;
	}
		
}
