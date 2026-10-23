/*++

Copyright (C) 2026 Autodesk Inc.

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


export function compareEntriesBy (keyName, descending)
{
	return (entryA, entryB) => {
		let valueA = entryA[keyName];
		let valueB = entryB[keyName];
		let order = (valueA < valueB) ? -1 : ((valueA > valueB) ? 1 : 0);
		return descending ? -order : order;
	};
}


// Keeps the entries of a database backed list in sync with the head id the server pushes.
// The first load and the first load after a failure fetch the whole list. Every later load
// only fetches the rows that changed since the head id of the previous load.
//
// requestList (sinceHeadID) must return a promise of { headID, delta, rows, removedKeys }.
// sinceHeadID is null for a full fetch. rows must be in the same shape as entries.
export default class AMCListDeltaSync {

	constructor (entries, keyName, compareEntries, requestList)
	{
		if (!Array.isArray (entries))
			throw "invalid list entries";
		if (typeof requestList !== "function")
			throw "invalid list request function";

		this.entries = entries;
		this.keyName = keyName;
		this.compareEntries = compareEntries;
		this.requestList = requestList;

		this.loadedHeadID = null;
		this.targetHeadID = -1;
		this.requestInFlight = false;
	}


	notifyHeadID (headID)
	{
		if (!Number.isSafeInteger (headID) || (headID <= this.targetHeadID))
			return;

		this.targetHeadID = headID;
		this.loadIfNeeded ();
	}


	loadIfNeeded ()
	{
		if (this.requestInFlight)
			return;
		if ((this.loadedHeadID !== null) && (this.loadedHeadID >= this.targetHeadID))
			return;

		let sinceHeadID = this.loadedHeadID;
		this.requestInFlight = true;

		this.requestList (sinceHeadID)
		.then (result => {
			if (!result || !Number.isSafeInteger (result.headID) || !Array.isArray (result.rows))
				throw "invalid list response";

			if ((sinceHeadID !== null) && (result.delta === true)) {
				this.applyDelta (result.rows, result.removedKeys);
			} else {
				this.replaceEntries (result.rows);
			}

			this.requestInFlight = false;
			this.loadedHeadID = result.headID;

			// A head the server does not report would otherwise request the list forever.
			if (this.targetHeadID > result.headID)
				this.targetHeadID = result.headID;

			this.loadIfNeeded ();
		})
		.catch (() => {
			this.requestInFlight = false;
			this.loadedHeadID = null;
		});
	}


	applyDelta (rows, removedKeys)
	{
		let entriesByKey = new Map ();
		for (let entry of this.entries)
			entriesByKey.set (entry[this.keyName], entry);

		if (Array.isArray (removedKeys)) {
			for (let key of removedKeys)
				entriesByKey.delete (key);
		}

		for (let row of rows)
			entriesByKey.set (row[this.keyName], row);

		let mergedEntries = Array.from (entriesByKey.values ());
		if (this.compareEntries)
			mergedEntries.sort (this.compareEntries);

		this.replaceEntries (mergedEntries);
	}


	replaceEntries (newEntries)
	{
		this.entries.splice (0, this.entries.length);
		for (let entry of newEntries)
			this.entries.push (entry);
	}

}
