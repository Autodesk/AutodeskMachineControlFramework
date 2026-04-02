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
 * Shared table utility helpers for list modules (AlertList, BuildList,
 * ExecutionList, ParameterList, ConfigurationList).
 */


const NULL_UUID = '00000000-0000-0000-0000-000000000000';

/** Badge class maps */
export const SEVERITY_CLASSES = {
	error:    'badge--red',
	critical: 'badge--red',
	fatal:    'badge--red',
	warning:  'badge--yellow',
	warn:     'badge--yellow',
	info:     'badge--blue',
	debug:    'badge--slate',
	ok:       'badge--green',
	success:  'badge--green',
};

export const STATUS_CLASSES = {
	running:    'badge--blue',
	active:     'badge--blue',
	finished:   'badge--green',
	completed:  'badge--green',
	succeeded:  'badge--green',
	failed:     'badge--red',
	error:      'badge--red',
	aborted:    'badge--yellow',
	cancelled:  'badge--yellow',
	pending:    'badge--slate',
	queued:     'badge--slate',
	paused:     'badge--purple',
};

/**
 * Format an ISO timestamp to a short date+time string.
 * @param {string} isoString
 * @returns {string}
 */
export function formatTime (isoString) {
	if (!isoString) return '';
	const date = new Date(isoString);
	if (isNaN(date.getTime())) return isoString;
	return new Intl.DateTimeFormat('en-US', {
		month:  'short',
		day:    'numeric',
		hour:   '2-digit',
		minute: '2-digit',
		second: '2-digit',
		hour12: false,
	}).format(date);
}

/**
 * Format an ISO timestamp to a full date+time string including year.
 * @param {string} isoString
 * @returns {string}
 */
export function formatDateTime (isoString) {
	if (!isoString) return '';
	const date = new Date(isoString);
	if (isNaN(date.getTime())) return isoString;
	return new Intl.DateTimeFormat('en-US', {
		year:   'numeric',
		month:  'short',
		day:    'numeric',
		hour:   '2-digit',
		minute: '2-digit',
		second: '2-digit',
		hour12: false,
	}).format(date);
}

/**
 * Format a duration given in seconds (or null) to hh:mm:ss.
 * @param {number|null} durationInSeconds
 * @returns {string}
 */
export function formatDuration (durationInSeconds) {
	if (durationInSeconds == null) return '';
	const s = Math.round(durationInSeconds);
	const h = Math.floor(s / 3600);
	const m = Math.floor((s % 3600) / 60);
	const sec = s % 60;
	return [h, m, sec].map(n => String(n).padStart(2, '0')).join(':');
}

/**
 * Return true when the given UUID is not the null UUID and not empty.
 * @param {string} uuid
 * @returns {boolean}
 */
export function isValidUUID (uuid) {
	return !!uuid && uuid !== NULL_UUID;
}

/**
 * Trigger a row-selection UI event.
 * @param {object} app         - AMCApplication instance
 * @param {object} module      - module with selectevent / selectionvalueuuid
 * @param {string} itemUUID    - UUID of the selected item
 */
export function triggerSelectEvent (app, module, itemUUID) {
	if (!app || !module || !module.selectevent) return;
	const formvalues = {};
	if (module.selectionvalueuuid && itemUUID)
		formvalues[module.selectionvalueuuid] = itemUUID;
	app.triggerUIEvent(module.selectevent, module.uuid, formvalues);
}

/**
 * Trigger an entry-button UI event.
 * @param {object} app      - AMCApplication instance
 * @param {object} module   - module with selectionvalueuuid / buttonvalueuuid
 * @param {object} button   - entry button definition with selectevent / uuid
 * @param {string} itemUUID - UUID of the row item
 */
export function triggerButtonEvent (app, module, button, itemUUID) {
	if (!app || !module || !button || !button.selectevent) return;
	const formvalues = {};
	if (module.selectionvalueuuid && itemUUID)
		formvalues[module.selectionvalueuuid] = itemUUID;
	if (module.buttonvalueuuid && button.uuid)
		formvalues[module.buttonvalueuuid] = button.uuid;
	app.triggerUIEvent(button.selectevent, module.uuid, formvalues);
}
