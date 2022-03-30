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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
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

#ifndef __LIBE1701
#define __LIBE1701


namespace libE1701 {

	typedef unsigned char e1701ID;
	typedef int e1701Error;
	typedef int e1701Version;

	typedef e1701Version(*E1701_get_library_version)();

	typedef e1701ID(*E1701_set_connection)(const char *address);

	typedef void(*E1701_set_password)(const e1701ID connectionID, const char* password);

	typedef void(*E1701_get_version)(const e1701ID connectionID, unsigned short * hwVersion, unsigned short * fwVersion);

	typedef e1701Error(*E1701_load_correction)(const e1701ID connectionID, const char* filename, uint8_t tableNum);

	typedef e1701Error(*E1701_switch_correction)(const e1701ID connectionID, uint8_t tableNum);

	typedef e1701Error(*E1701_execute)(const e1701ID connectionID);

	typedef void(*E1701_close)(const e1701ID connectionID);

	typedef e1701Error(*E1701_jump_abs)(const e1701ID connectionID, const int x, const int y, const int z);

	typedef e1701Error(*E1701_mark_abs)(const e1701ID connectionID, const int x, const int y, const int z);

	typedef e1701Error(*E1701_set_speeds)(const e1701ID connectionID, const double jumpspeed, const double markspeed);

	typedef e1701Error(*E1701_execute)(const e1701ID connectionID);
	
	typedef e1701Error(*E1701_stop_execution)(const e1701ID connectionID);

	typedef e1701Error(*E1701_get_card_state2)(const e1701ID connectionID, unsigned int * state);

	typedef struct {
		E1701_get_library_version m_get_library_version;
		E1701_set_connection m_set_connection;
		E1701_set_password m_set_password;
		E1701_get_version m_get_version;
		E1701_load_correction m_load_correction;
		E1701_switch_correction m_switch_correction;
		E1701_execute m_execute;
		E1701_close m_close;
		E1701_jump_abs m_jump_abs;
		E1701_mark_abs m_mark_abs;
		E1701_set_speeds m_set_speeds;
		E1701_stop_execution m_stop_execution;
		E1701_get_card_state2 m_get_card_state2;

	} sLibE1701;

}


#endif // __LIBE1701