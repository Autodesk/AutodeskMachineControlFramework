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

/**
 * Runtime schema definitions for all module types.
 * Each entry lists the required fields that must be present for a module
 * payload to be considered well-formed, plus an optional list for
 * documentation purposes.
 *
 * validateModuleJSON(json) validates a single module payload object against
 * its schema and returns { valid, errors }.
 */

const MODULE_SCHEMAS = {
	content: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'headline', 'title', 'subtitle', 'entries'],
	},
	grid: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'columns', 'rows', 'sections', 'padding'],
	},
	tabs: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'tabs'],
	},
	glscene: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'scene'],
	},
	layerview: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'items'],
	},
	graphic: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'viewminx', 'viewminy', 'viewmaxx', 'viewmaxy', 'showgrid', 'items'],
	},
	logs: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'entries'],
	},
	parameterlist: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'headers', 'entries', 'loadingtext', 'entriesperpage'],
	},
	statuslist: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'title', 'rows', 'loadingtext'],
	},
	togglepanel: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'columns', 'toggles'],
	},
	form: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'entities'],
	},
	chart: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'dataseries', 'version'],
	},
	image: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'imageresource', 'aspectratio', 'maxwidth', 'maxheight'],
	},
	paragraph: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'text', 'headline', 'muted'],
	},
	upload: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'uploadcaption', 'acceptedtypes', 'uploadclass'],
	},
	buildlist: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'headers', 'entries', 'loadingtext', 'entriesperpage', 'selectevent', 'selectionvalueuuid', 'buttonvalueuuid', 'entrybuttons'],
	},
	executionlist: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'headers', 'entries', 'loadingtext', 'entriesperpage', 'selectevent', 'selectionvalueuuid', 'buttonvalueuuid', 'entrybuttons'],
	},
	alertlist: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'headers', 'entries', 'loadingtext', 'entriesperpage', 'selectevent', 'selectionvalueuuid'],
	},
	buttongroup: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'buttons', 'cssstyle', 'buttoncssstyle', 'buttondistribution'],
	},
	configurationlist: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'headers', 'entries', 'loadingtext', 'entriesperpage', 'selectevent', 'selectionvalueuuid', 'buttonvalueuuid', 'entrybuttons', 'schema'],
	},
	videostream: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'streamresource', 'maxwidth', 'maxheight'],
	},
	separator: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'orientation', 'variant'],
	},
	statusbanner: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible', 'severity', 'title', 'description'],
	},
	custom: {
		required: ['uuid', 'type'],
		optional: ['caption', 'visible'],
	},
};

/**
 * Validate a module JSON payload against the schema for its type.
 *
 * @param {object} json - raw module payload (after v2-to-legacy normalisation)
 * @returns {{ valid: boolean, errors: string[] }}
 */
export function validateModuleJSON (json) {
	if (!json || typeof json !== 'object')
		return { valid: false, errors: ['Module payload is not an object'] };

	const type = json.type;
	if (!type)
		return { valid: false, errors: ['Module payload is missing "type" field'] };

	const schema = MODULE_SCHEMAS[type];
	if (!schema)
		return { valid: false, errors: ['Unknown module type: ' + type] };

	const errors = [];
	for (const field of schema.required) {
		if (json[field] == null)
			errors.push('Missing required field "' + field + '" in module of type "' + type + '"');
	}

	return { valid: errors.length === 0, errors };
}

export default MODULE_SCHEMAS;
