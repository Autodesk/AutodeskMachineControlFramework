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


export default class AMCApplicationModule_Workflow extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_Workflow");

		this.usesV2Frontend = true;

		this.workflowData = moduleJSON.workflowdata || null;
		this.catalogData = moduleJSON.catalogdata || null;
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);
		if (updateJSON.workflowdata !== undefined)
			this.workflowData = updateJSON.workflowdata;
		if (updateJSON.catalogdata !== undefined)
			this.catalogData = updateJSON.catalogdata;
		if (updateJSON.visible !== undefined)
			this.visible = Assert.BoolValue (updateJSON.visible);
	}


	updateFromV2Attributes (attrs)
	{
		if (!attrs)
			return true;
		if (attrs.workflowdata !== undefined)
			this.workflowData = attrs.workflowdata;
		if (attrs.catalogdata !== undefined)
			this.catalogData = attrs.catalogdata;
		if (attrs.catalog !== undefined)
			this.catalogData = this._normalizeCatalog(attrs.catalog);
		if (attrs.caption !== undefined)
			this.caption = attrs.caption;
		if (attrs.visible !== undefined)
			this.visible = (attrs.visible === "1" || attrs.visible === true || attrs.visible === "true");
		return true;
	}

	_normalizeCatalog (rawCatalog)
	{
		if (!Array.isArray(rawCatalog))
			return null;
		return rawCatalog.map(entry => {
			const normalized = {
				id: entry.id,
				label: entry.label || entry.id,
				category: entry.category || "functionblock",
				color: entry.color || "blue"
			};
			if (entry.params && Array.isArray(entry.params)) {
				normalized.params = entry.params.map(p => {
					const param = {
						name: p.name,
						type: p.type || "string"
					};
					if (p.unit) param.unit = p.unit;
					if (p.default !== undefined && p.default !== "") {
						if (p.type === "int") param.default = parseInt(p.default, 10);
						else if (p.type === "float") param.default = parseFloat(p.default);
						else if (p.type === "bool") param.default = (p.default === "true" || p.default === "1");
						else param.default = p.default;
					}
					return param;
				});
			}
			return normalized;
		});
	}

	getDefaultWorkflow ()
	{
		return {
			uuid: "mock-workflow-001",
			name: "Example Production",
			sequence: [
				{ id: "s1", stepRef: "exposure", params: {} },
				{ id: "s2", stepRef: "delay", params: { duration: 3000 } },
				{ id: "s3", stepRef: "beschichter_lr", params: {} },
				{
					id: "g1",
					type: "parallel",
					branches: [
						[
							{ id: "s4a", stepRef: "dosierung_r", params: {} },
							{ id: "s5a", stepRef: "delay", params: { duration: 2000 } }
						],
						[
							{ id: "s4b", stepRef: "heatup_layer", params: {} },
							{ id: "s5b", stepRef: "hold_temp", params: {} }
						]
					]
				},
				{ id: "s6", stepRef: "exposure", params: {} }
			]
		};
	}

	getDefaultCatalog ()
	{
		return [
			{ id: "exposure",       label: "Exposure",        category: "functionblock", color: "blue" },
			{ id: "delay",          label: "Delay",           category: "functionblock", color: "blue",
			  params: [{ name: "duration", type: "int", unit: "ms", default: 1000 }] },
			{ id: "beschichter_lr", label: "Beschichter L→R", category: "template",      color: "amber" },
			{ id: "beschichter_rl", label: "Beschichter R→L", category: "template",      color: "amber" },
			{ id: "dosierung_r",    label: "Dosierung R",     category: "template",      color: "amber" },
			{ id: "dosierung_l",    label: "Dosierung L",     category: "template",      color: "amber" },
			{ id: "heatup_layer",   label: "Heatup Layer",    category: "template",      color: "amber" },
			{ id: "hold_temp",      label: "Hold Temperature",category: "template",      color: "amber" }
		];
	}

}
