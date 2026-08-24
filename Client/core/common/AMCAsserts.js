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


export function IdentifierString (value) // Not empty, Alphanumeric with _ - characters
{
	if (!value)
		throw "invalid identifier string";
	
	return value;
}

export function OptionalIdentifierString (value) // Allows empty identifier too!
{
	if (!value)
		return "";
	
	return IdentifierString (value);
} 


export function StringValue (value) // Arbitrary string
{
	if (!value)
		return "";
	
	return value.toString ();
}

export function BoolValue (value) // Boolean Value
{
	if (!value)
		return false;
	
	return true;
}

export function UUIDValue (value)
{
	if (!value)
		throw "invalid uuid string";
	
	return value.toString ();
}

export function SHA256Value (value)
{
	if (!value)
		throw "invalid sha256 string";
	
	return value.toString ();
}


export function OptionalUUIDValue (value)
{
	if (!value)
		return "00000000-0000-0000-0000-000000000000";
	
	return value.toString ();
}


export function ObjectValue (value)
{
	if (!value)
		throw "invalid json object";
	
	return value;
}

export function ArrayValue (value)
{
	
	if (!Array.isArray (value))
		throw "invalid array object";
	
	return value;
}

export function NumberValue (value)
{
	if (isNaN (value))
		throw "invalid number value";
	
	return value;
}

export function IntegerValue (value)
{
	if (isNaN (value))
		throw "invalid number value";
	
	return Math.round (value);
} 

export function OptionalIntegerValue (value)
{
	if (!value)
		return 0;
	
	return IntegerValue (value);
}  


export function ObjectInstance (objectInstance, className)
{
	let classID = IdentifierString (className);
	
	if (!objectInstance)
		throw "object instance is null (" + classID + ")";
	
	if (!objectInstance.isClass)
		throw "object instance is not amc class (" + classID + ")";
	
	if (!objectInstance.isClass (className))
		throw "invalid object instance (" + classID + ")";
	
	return objectInstance;
}
