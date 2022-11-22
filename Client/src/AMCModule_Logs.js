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
import { DateTime } from "luxon";

class AMCApplicationModule_LogItem extends Common.AMCApplicationItem {
	
	constructor (moduleInstance, itemUUID) 
	{
		super (moduleInstance, itemUUID, "logitem");		
		this.registerClass ("amcItem_LogItem");
		this.stateid = 1;
		
		this.updateFromJSON ({});
		
		this.setRefreshFlag ();
		
	}
		
	
	updateFromJSON (updateJSON)
	{
		if (updateJSON.logentries) {
			for (let logentry of updateJSON.logentries) {
				
				let timeStampInput = logentry.timestamp.replace ("Z UTC", "").trim();
				let timeStampObject = DateTime.fromISO(timeStampInput, {zone: 'utc'});
				let timeStampStr = timeStampObject.toFormat('HH:mm:ss.SSS');
				
				this.moduleInstance.DisplayItems.unshift ({
					logIndex: logentry.id,
					logSubsystem: logentry.subsystem,
					logTime: timeStampStr,
					logText: logentry.message
				});
				
				if (logentry.id >= this.stateid) {
					this.stateid = logentry.id + 1;
				}
				
			}
		}
	}
	
}


export default class AMCApplicationModule_Logs extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);				
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_Logs");
		
		this.currentReceiveIndex = 0;
		
		this.items = [];
				
		this.logItem = new AMCApplicationModule_LogItem (this, moduleJSON.uuid);
		
		this.items.push (this.logItem);
		this.page.addItem (this.logItem);
		
		this.DisplayItems = [
		];
		
						
	}
		
		
	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		Assert.ArrayValue (updateJSON.entries);
		
	}
}
