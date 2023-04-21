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



export default class AMCApplicationModule_Grid extends Common.AMCApplicationModule {
	
	constructor (page, moduleJSON) 
	{		
		Assert.ObjectValue (moduleJSON);				
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name, moduleJSON.caption);		
		this.registerClass ("amcModule_Grid");
				
				
		// TODO: check input
		this.columns = Assert.ArrayValue (moduleJSON.columns);
		this.rows = Assert.ArrayValue (moduleJSON.rows);
				
		this.sections = []
		
		Assert.ArrayValue (moduleJSON.sections);
		for (let sectionJSON of moduleJSON.sections) {
			
			let section = this.page.application.createModuleInstance (this.page, sectionJSON);
			
			section.columnstart = Assert.IntegerValue (sectionJSON.columnstart);
			section.columnend = Assert.IntegerValue (sectionJSON.columnend);
			section.rowstart = Assert.IntegerValue (sectionJSON.rowstart);
			section.rowend = Assert.IntegerValue (sectionJSON.rowend);
			section.columnposition = Assert.IdentifierString (sectionJSON.columnposition);
			section.rowposition = Assert.IdentifierString (sectionJSON.rowposition);
			section.scrollbars = Assert.BoolValue (sectionJSON.scrollbars);
			
			var marginx = "margin-left: 0; margin-right: 0;";
			var marginy = "margin-top: 0; margin-bottom: 0;";

			if (section.columnposition) {
				if (section.columnposition == "centered")
					marginx = "margin-left: auto; margin-right: auto;";
				if (section.columnposition == "right")
					marginx = "margin-left: auto; margin-right: 0;"
			}

			if (section.rowposition) {
				if (section.rowposition == "centered")
					marginy = "margin-top: auto; margin-bottom: auto;";
				if (section.rowposition == "bottom")
					marginy = "margin-top: auto; margin-bottom: 0;";
			}
			
			var overflowtype;
			if (section.scrollbars) {
				overflowtype = "scroll";
			} else {
				overflowtype = "hidden";
			}

			section.cssstyle = "overflow:" + overflowtype + "; overflow-x:" + overflowtype + "; grid-area:" + section.name + ";" + marginx + marginy;
			
			
			if (section) {				
				this.sections.push (section);
				this.page.application.addModule (section);
			} else {
				throw "Module type not found: " + sectionJSON.type;
			}
			
			
		}
		
		
		this.cssstyle = "display: grid; width:100%; height:100%;";

		let columnString = "";
        let rowString = "";
        let areaString = "";

		let columnCount = this.columns.length;
		let rowCount = this.rows.length;
	
		let gridMap = new Array(rowCount);
		for (let row = 0; row < rowCount; row++) {
			gridMap[row] = new Array(columnCount);

			for (let column = 0; column < columnCount; column++) {
				var templatename = "_grid_" + this.name + "_" + column + "_" + row;
				gridMap[row][column] = templatename;
			}
		}

		for (let section of this.sections) {

			let columnstart = section.columnstart;
			let columnend = section.columnend;
			let rowstart = section.rowstart;
			let rowend = section.rowend;

			if ((columnstart <= columnend) && (rowstart <= rowend) && (columnstart > 0) && (rowstart > 0) &&
				(columnend <= columnCount) && (rowend <= rowCount)) {

				for (let row = rowstart - 1; row < rowend; row++) {
					for (let column = columnstart - 1; column < columnend; column++) {
						gridMap[row][column] = section.name;
					}
				}
			}
		}

		for (let row = 0; row < rowCount; row++) {
			let rowObject = this.rows[row];

			if (rowObject.unit === "px") {
				rowString = rowString + rowObject.height + "px ";
			} else if (rowObject.unit === "free") {
				rowString = rowString + rowObject.height + "fr ";
			} else if (rowObject.unit === "pt") {
				rowString = rowString + rowObject.height + "pt ";
			} else {
				rowString = rowString + "auto ";
			}
		}

		for (let column = 0; column < columnCount; column++) {
			let columnObject = this.columns[column];

			if (columnObject.unit === "px") {
				columnString = columnString + columnObject.width + "px ";
			} else if (columnObject.unit === "free") {
				columnString = columnString + columnObject.width + "fr ";
			} else if (columnObject.unit === "pt") {
				columnString = columnString + columnObject.width + "pt ";
			} else {
				columnString = columnString + "auto ";
			}

		}

		for (let row = 0; row < rowCount; row++) {
			areaString = areaString + "\"";
			for (let column = 0; column < columnCount; column++) {
				if (column > 0) {
					areaString = areaString + " ";
				}
				areaString = areaString + gridMap[row][column];
			}
			areaString = areaString + "\" ";
		}


		this.cssstyle = this.cssstyle + "grid-template-columns: " + columnString + ";";
		this.cssstyle = this.cssstyle + "grid-template-rows: " + rowString + ";";
		this.cssstyle = this.cssstyle + "grid-template-areas: " + areaString;

		
	}
		
}
