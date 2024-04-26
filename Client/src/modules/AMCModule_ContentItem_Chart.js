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


export default 

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