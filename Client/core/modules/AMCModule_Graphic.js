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


class AMCApplicationItem_Graphic_SVGImage extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemJSON) 
	{
		Assert.ObjectValue (itemJSON);		
		
		super (moduleInstance, itemJSON.uuid, itemJSON.type);		
		this.registerClass ("amcItem_SVGImage");
		this.usesV2Frontend = true;
		
		this.name = Assert.IdentifierString (itemJSON.name);		
		this.imageuuid = Assert.OptionalUUIDValue (itemJSON.imageuuid);

		this.updateFromJSON (itemJSON);
				
		this.setRefreshFlag ();
		
	}
		
	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);		
		
		this.x = Assert.NumberValue (updateJSON.x);
		this.y = Assert.NumberValue (updateJSON.y);
		this.z = Assert.NumberValue (updateJSON.z);
		this.scalex = Assert.NumberValue (updateJSON.scalex);
		this.scaley = Assert.NumberValue (updateJSON.scaley);
		this.angle = Assert.NumberValue (updateJSON.angle);
		
		this.moduleInstance.callDataHasChanged ();
	}

	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return true;

		if (attrs.name !== undefined)
			this.name = attrs.name;
		if (attrs.x !== undefined) {
			let x = parseFloat (attrs.x);
			if (!isNaN (x))
				this.x = x;
		}
		if (attrs.y !== undefined) {
			let y = parseFloat (attrs.y);
			if (!isNaN (y))
				this.y = y;
		}
		if (attrs.z !== undefined) {
			let z = parseFloat (attrs.z);
			if (!isNaN (z))
				this.z = z;
		}
		if (attrs.scalex !== undefined) {
			let scalex = parseFloat (attrs.scalex);
			if (!isNaN (scalex))
				this.scalex = scalex;
		}
		if (attrs.scaley !== undefined) {
			let scaley = parseFloat (attrs.scaley);
			if (!isNaN (scaley))
				this.scaley = scaley;
		}
		if (attrs.angle !== undefined) {
			let angle = parseFloat (attrs.angle);
			if (!isNaN (angle))
				this.angle = angle;
		}

		this.moduleInstance.callDataHasChanged ();
		return true;
	}
}


export default class AMCApplicationModule_Graphic extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);				
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_Graphic");
		this.usesV2Frontend = true;
		
		this.viewminx = Assert.NumberValue (moduleJSON.viewminx);
		this.viewminy = Assert.NumberValue (moduleJSON.viewminy);
		this.viewmaxx = Assert.NumberValue (moduleJSON.viewmaxx);
		this.viewmaxy = Assert.NumberValue (moduleJSON.viewmaxy);
		this.showgrid = Assert.BoolValue (moduleJSON.showgrid);
		
		Assert.ArrayValue (moduleJSON.items);
		this.items = [];

		for (let itemJSON of moduleJSON.items) {
			
			let item = null;
			
			if (itemJSON.type === "svgimage") 
				item = new AMCApplicationItem_Graphic_SVGImage (this, itemJSON);
			
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
			if (attrs.viewminx !== undefined) {
				let value = parseFloat (attrs.viewminx);
				if (!isNaN (value))
					this.viewminx = value;
			}
			if (attrs.viewminy !== undefined) {
				let value = parseFloat (attrs.viewminy);
				if (!isNaN (value))
					this.viewminy = value;
			}
			if (attrs.viewmaxx !== undefined) {
				let value = parseFloat (attrs.viewmaxx);
				if (!isNaN (value))
					this.viewmaxx = value;
			}
			if (attrs.viewmaxy !== undefined) {
				let value = parseFloat (attrs.viewmaxy);
				if (!isNaN (value))
					this.viewmaxy = value;
			}
			if (attrs.showgrid !== undefined)
				this.showgrid = (attrs.showgrid === true || attrs.showgrid === "1" || attrs.showgrid === "true");
		}

		let v2Entry = this.page.application.getV2Entry (this.uuid);
		if (v2Entry && v2Entry.submodules) {
			for (let submodule of v2Entry.submodules) {
				let item = this.items.find(entry => entry.uuid === submodule.uuid);
				if (item && submodule.attributes) {
					item.updateFromV2Attributes (submodule.attributes);
				}
			}
		}

		return true;
	}
		
}
