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


Abstract: This is the class declaration of CUserPreferenceHandler

*/


#ifndef __LIBMCDATA_USERPREFERENCEHANDLER
#define __LIBMCDATA_USERPREFERENCEHANDLER

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

#include "amcdata_sqlhandler.hpp"

// Include custom headers here.
#include <mutex>


namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CUserPreferenceHandler 
**************************************************************************************************************************/

class CUserPreferenceHandler : public virtual IUserPreferenceHandler, public virtual CBase {

protected:
	std::mutex m_Mutex;
	AMCData::PSQLHandler m_pSQLHandler;

	// Normalizes and validates the (UserUUID, Domain, Key) tuple. Throws if domain or key are empty.
	void normalizeIdentifier(const std::string& sUserUUID, const std::string& sDomain, const std::string& sKey, std::string& sNormalizedUserUUID, std::string& sNormalizedDomain, std::string& sNormalizedKey);

public:

	CUserPreferenceHandler(AMCData::PSQLHandler pSQLHandler);

	bool HasUserPreference(const std::string & sUserUUID, const std::string & sDomain, const std::string & sKey) override;

	std::string RetrieveUserPreference(const std::string & sUserUUID, const std::string & sDomain, const std::string & sKey) override;

	void StoreUserPreference(const std::string & sUserUUID, const std::string & sDomain, const std::string & sKey, const std::string & sValue, const LibMCData_uint64 nAbsoluteTimeStamp) override;

	bool DeleteUserPreference(const std::string & sUserUUID, const std::string & sDomain, const std::string & sKey) override;

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_USERPREFERENCEHANDLER
