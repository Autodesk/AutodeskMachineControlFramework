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


import AMCApplicationItem_Content_AlertList from "./AMCModule_ContentItem_AlertList.js"
import AMCApplicationItem_Content_BuildList from "./AMCModule_ContentItem_BuildList.js"
import AMCApplicationItem_Content_ButtonGroup from "./AMCModule_ContentItem_ButtonGroup.js"
import AMCApplicationItem_Content_Chart from "./AMCModule_ContentItem_Chart.js"
import AMCApplicationItem_Content_ExecutionList from "./AMCModule_ContentItem_ExecutionList.js"
import AMCApplicationItem_Content_Form from "./AMCModule_ContentItem_Form.js"
import AMCApplicationItem_Content_Image from "./AMCModule_ContentItem_Image.js"
import AMCApplicationItem_Content_Paragraph from "./AMCModule_ContentItem_Paragraph.js"
import AMCApplicationItem_Content_ParameterList from "./AMCModule_ContentItem_ParameterList.js"
import AMCApplicationItem_Content_Upload from "./AMCModule_ContentItem_Upload.js"



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
			
			if (itemJSON.type === "executionlist") 
				item = new AMCApplicationItem_Content_ExecutionList (this, itemJSON);
			
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

