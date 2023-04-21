/*++

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

 */

import * as Assert from "./AMCAsserts.js";
import AMCApplicationPage from "./AMCPage.js"

export default class AMCApplicationCustomPage extends AMCApplicationPage {
	
	constructor (application, pageJSON) 
	{
		Assert.ObjectInstance (application, "amcApplication");
		Assert.ObjectValue (pageJSON);
		Assert.IdentifierString (pageJSON.name);
		Assert.IdentifierString (pageJSON.component);
		Assert.ArrayValue (pageJSON.modules);
		
		super (application, pageJSON);
		this.registerClass ("amcCustomPage");
			
		this.application = application;
		this.name = pageJSON.name;
		this.component = pageJSON.component;
		
		this.modules = [];
		this.moduleMap = new Map ();
		this.customModule = null;
					
		for (let moduleDefinitionJSON of pageJSON.modules) {
			
			let moduleInstance = this.application.createModuleInstance (this, moduleDefinitionJSON);					
			
			if (moduleInstance) {				
				this.modules.push (moduleInstance);
				this.moduleMap.set (moduleInstance.name, moduleInstance);
				this.application.addModule (moduleInstance);												
				
				if (moduleInstance.type == "custom")
					this.customModule = moduleInstance;
					
			} else {
				throw "Module type not found: " + moduleDefinitionJSON.type;
			}
						
		}
		
		
	}
	
	findModule (name)
	{
		Assert.IdentifierString (name);
		if (this.moduleMap.has (name))
			return this.moduleMap.get (name);
		
		return null;
	}
	
	getProperty (name)
	{
		Assert.IdentifierString (name);
		
		return "";
	}
	
	getPropertiesObject ()
	{		
		if (this.customModule) {
			if (this.customModule.propertiesitem)
				return this.customModule.propertiesitem;
		}
	
		// Fallback: Return empty object
		return {
			values: { }
		};
	}

	callEvent (eventName, parameters) 
	{
		Assert.IdentifierString (eventName);
		Assert.IdentifierString (parameters);
		
	}

}
