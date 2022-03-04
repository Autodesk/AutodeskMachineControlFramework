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


Abstract: This is the class declaration of CPersistencyHandler

*/


#ifndef __LIBMCDATA_PERSISTENCYHANDLER
#define __LIBMCDATA_PERSISTENCYHANDLER

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
 Class declaration of CPersistencyHandler 
**************************************************************************************************************************/

class CPersistencyHandler : public virtual IPersistencyHandler, public virtual CBase {

protected:
	std::mutex m_Mutex;
	AMCData::PSQLHandler m_pSQLHandler;

	std::string retrievePersistentParameter(const std::string& sUUID, const LibMCData::eParameterDataType eDataType);


public:

	CPersistencyHandler(AMCData::PSQLHandler pSQLHandler);

	bool HasPersistentParameter(const std::string & sUUID) override;

	void GetPersistentParameterDetails(const std::string & sUUID, std::string & sName, LibMCData::eParameterDataType & eDataType) override;

	bool DeletePersistentParameter(const std::string & sUUID) override;

	void StorePersistentParameter(const std::string & sUUID, const std::string & sName, const LibMCData::eParameterDataType eDataType, const std::string & sValue) override;

	void StorePersistentStringParameter(const std::string & sUUID, const std::string & sName, const std::string & sValue) override;

	void StorePersistentUUIDParameter(const std::string & sUUID, const std::string & sName, const std::string & sValue) override;

	void StorePersistentDoubleParameter(const std::string & sUUID, const std::string & sName, const LibMCData_double dValue) override;

	void StorePersistentIntegerParameter(const std::string & sUUID, const std::string & sName, const LibMCData_int64 nValue) override;

	void StorePersistentBoolParameter(const std::string & sUUID, const std::string & sName, const bool bValue) override;

	std::string RetrievePersistentStringParameter(const std::string & sUUID) override;

	std::string RetrievePersistentUUIDParameter(const std::string & sUUID) override;

	LibMCData_double RetrievePersistentDoubleParameter(const std::string & sUUID) override;

	LibMCData_int64 RetrievePersistentIntegerParameter(const std::string & sUUID) override;

	bool RetrievePersistentBoolParameter(const std::string & sUUID) override;

	static std::string convertDataTypeToString(const LibMCData::eParameterDataType eDataType);
	static LibMCData::eParameterDataType convertStringToDataType (const std::string & sValue);

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_PERSISTENCYHANDLER
