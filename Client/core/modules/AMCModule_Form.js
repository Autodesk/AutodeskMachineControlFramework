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


function makeEntityDataObject (entity) {
	return {
		uuid:                 entity.uuid,
		value:                entity.value,
		remotevalue:          entity.value,
		disabled:             entity.disabled,
		readonly:             entity.readonly,
		changeevent:          entity.changeevent,
		validation:           entity.validation,
		validationmessage:    entity.validationmessage,
		minvalue:             entity.minvalue,
		maxvalue:             entity.maxvalue,
		minlength:            entity.minlength,
		maxlength:            entity.maxlength,
		step:                 entity.step,
		unit:                 entity.unit,
		mode:                 entity.mode,
		format:               entity.format,
		items:                entity.items,
		// actionbar-specific
		submitevent:          entity.submitevent,
		cancelevent:          entity.cancelevent,
		validateevent:        entity.validateevent,
		submitcaption:        entity.submitcaption,
		cancelcaption:        entity.cancelcaption,
		optimistic:           entity.optimistic,
		// section-specific
		description:          entity.description,
		icon:                 entity.icon,
		collapsible:          entity.collapsible,
		defaultOpen:          entity.defaultopen,
		isProgrammaticChange: false,
	};
}


export default class AMCApplicationModule_Form extends Common.AMCApplicationModule {

	constructor (page, moduleJSON)
	{
		Assert.ObjectValue (moduleJSON);
		super (page, moduleJSON.uuid, moduleJSON.type, moduleJSON.name || moduleJSON.uuid, moduleJSON.caption || "");
		this.registerClass ("amcModule_Form");

		this.usesV2Frontend = true;

		this.entities = Array.isArray(moduleJSON.entities) ? moduleJSON.entities : [];

		// Dirty-state tracking and server-side error map
		this.serverErrors = {};
		this.isSubmitting = false;

		this._registerEntities(this.entities);
	}


	_registerEntities (entities) {
		for (let entity of entities) {
			entity.dataObject = makeEntityDataObject(entity);
			this.page.application.AppContent.FormEntityMap.set (entity.uuid, entity);

			// Recursively register children inside sections
			if (entity.type === 'section' && Array.isArray(entity.entities)) {
				this._registerEntities(entity.entities);
			}
		}
	}


	_flatEntities () {
		const result = [];
		const collect = (list) => {
			for (let e of list) {
				result.push(e);
				if (e.type === 'section' && Array.isArray(e.entities))
					collect(e.entities);
			}
		};
		collect(this.entities);
		return result;
	}


	isEntityDirty (entity) {
		if (!entity || !entity.dataObject) return false;
		return entity.dataObject.value !== entity.dataObject.remotevalue;
	}


	get isFormDirty () {
		return this._flatEntities().some(e => this.isEntityDirty(e));
	}


	resetDirtyFields () {
		for (let entity of this._flatEntities()) {
			if (entity.dataObject) {
				entity.dataObject.isProgrammaticChange = true;
				entity.dataObject.value = entity.dataObject.remotevalue;
			}
		}
		this.serverErrors = {};
	}


	assembleAllFormValues () {
		const values = {};
		for (let entity of this._flatEntities()) {
			if (entity.dataObject && entity.type !== 'actionbar' && entity.type !== 'section' && entity.type !== 'calculated') {
				values[entity.uuid] = entity.dataObject.value;
			}
		}
		return values;
	}


	updateFromJSON (updateJSON)
	{
		Assert.ObjectValue (updateJSON);

		this.setVisible(updateJSON.visible);

		if (updateJSON.entities) {
			Assert.ArrayValue (updateJSON.entities);

			for (let entityJSON of updateJSON.entities) {
				if (!this.page.application.AppContent.FormEntityMap.has (entityJSON.uuid)) {
					let newEntity = {
						uuid:              entityJSON.uuid,
						name:              entityJSON.name || entityJSON.uuid,
						type:              entityJSON.type || "edit",
						caption:           entityJSON.caption || "",
						value:             entityJSON.value,
						disabled:          !!entityJSON.disabled,
						readonly:          !!entityJSON.readonly,
						changeevent:       entityJSON.changeevent || "",
						validation:        entityJSON.validation || "",
						validationmessage: entityJSON.validationmessage || "",
						minvalue:          entityJSON.minvalue,
						maxvalue:          entityJSON.maxvalue,
						minlength:         entityJSON.minlength,
						maxlength:         entityJSON.maxlength,
						step:              entityJSON.step,
						unit:              entityJSON.unit,
						mode:              entityJSON.mode,
						format:            entityJSON.format,
						items:             entityJSON.items,
					};
					newEntity.dataObject = makeEntityDataObject(newEntity);
					this.entities.push(newEntity);
					this.page.application.AppContent.FormEntityMap.set(newEntity.uuid, newEntity);
				}

				let entity = this.page.application.AppContent.FormEntityMap.get (entityJSON.uuid);
				let dataObject = entity.dataObject;

				Assert.ObjectValue (dataObject);

				if (dataObject.remotevalue !== entityJSON.value) {
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


	updateFromV2Attributes (attrs)
	{
		let v2Entry = this.page.application.getV2Entry(this.uuid);
		if (!v2Entry)
			return true;

		if (v2Entry.attributes) {
			if (v2Entry.attributes.visible !== undefined)
				this.setVisible(v2Entry.attributes.visible === true || v2Entry.attributes.visible === "1" || v2Entry.attributes.visible === "true");
			if (v2Entry.attributes.caption !== undefined)
				this.caption = v2Entry.attributes.caption;
		}

		// ContentLeaf wraps the Form item as submodules[0].
		// The individual form entities are one level deeper: submodules[0].submodules.
		let entitySubmodules = (v2Entry.submodules && v2Entry.submodules.length > 0 && v2Entry.submodules[0].submodules)
			? v2Entry.submodules[0].submodules
			: [];

		if (entitySubmodules.length > 0) {
			let app = this.page.application;

			for (let sub of entitySubmodules) {
				if (!app.AppContent.FormEntityMap.has(sub.uuid))
					continue;

				let entity = app.AppContent.FormEntityMap.get(sub.uuid);
				let dataObject = entity.dataObject;
				let a = sub.attributes || {};

				if (a.value !== undefined) {
					if (dataObject.remotevalue !== a.value) {
						dataObject.value = a.value;
						dataObject.isProgrammaticChange = true;
					} else {
						dataObject.isProgrammaticChange = false;
					}
					dataObject.remotevalue = a.value;
				}

				if (a.caption !== undefined)
					entity.caption = a.caption;
				if (a.disabled !== undefined)
					dataObject.disabled = (a.disabled === true || a.disabled === "1" || a.disabled === "true");
				if (a.readonly !== undefined)
					dataObject.readonly = (a.readonly === true || a.readonly === "1" || a.readonly === "true");
				if (a.prefix !== undefined)
					dataObject.prefix = a.prefix;
				if (a.suffix !== undefined)
					dataObject.suffix = a.suffix;
				if (a.changeevent !== undefined)
					dataObject.changeevent = a.changeevent;
				if (a.validation !== undefined)
					dataObject.validation = a.validation;
				if (a.validationmessage !== undefined)
					dataObject.validationmessage = a.validationmessage;
				if (a.minvalue !== undefined)
					dataObject.minvalue = a.minvalue;
				if (a.maxvalue !== undefined)
					dataObject.maxvalue = a.maxvalue;
				if (a.minlength !== undefined)
					dataObject.minlength = a.minlength;
				if (a.maxlength !== undefined)
					dataObject.maxlength = a.maxlength;
				if (a.step !== undefined)
					dataObject.step = a.step;
				if (a.unit !== undefined)
					dataObject.unit = a.unit;
				if (a.mode !== undefined)
					dataObject.mode = a.mode;
			if (a.format !== undefined)
				dataObject.format = a.format;

			if ((entity.type === "combobox" || entity.type === "multiselect") && sub.submodules) {
				entity.items = sub.submodules.map(item => {
					let ia = item.attributes || {};
					return { value: String(ia.value !== undefined ? ia.value : ""), text: ia.text || String(ia.value !== undefined ? ia.value : "") };
				});
				dataObject.items = entity.items;

				// Item values are normalized to strings above; coerce the current
				// combobox value to a string too so the selected item resolves on
				// first render (a numeric default like value="1" would otherwise
				// fail the strict comparison against the string item value).
				if (entity.type === "combobox" && dataObject.value !== undefined && dataObject.value !== null)
					dataObject.value = String(dataObject.value);
			}
		}
	}

	return true;
	}


	setVisible (flag) {
		this.visible = !!flag;
	}

}
