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

#include "amc_toolpathpart.hpp"
#include "libmc_exceptiontypes.hpp"

#include "Common/common_utils.hpp"

namespace AMC {

	CToolpathPart::CToolpathPart(Lib3MF::PModel p3MFModel, Lib3MF::PBuildItem pBuildItem, Lib3MF::PWrapper pWrapper)
		: m_p3MFModel (p3MFModel), m_pBuildItem (pBuildItem), m_pWrapper (pWrapper)
	{
		if (m_p3MFModel.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (m_pBuildItem.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (m_pWrapper.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		auto p3MFObject = m_pBuildItem->GetObjectResource();
		m_sName = p3MFObject->GetName();

		bool bHasUUID;
		m_sUUID = m_pBuildItem->GetUUID(bHasUUID);
		if (!bHasUUID)
			throw ELibMCCustomException (LIBMC_ERROR_BUILDITEMHASNOUUID, m_sName);

		m_sPartNumber = m_pBuildItem->GetPartNumber();

		m_sMeshUUID = p3MFObject->GetUUID(bHasUUID);
		if (!bHasUUID)
			throw ELibMCCustomException(LIBMC_ERROR_OBJECTHASNOUUID, m_sName);

	}

	CToolpathPart::~CToolpathPart()
	{
		m_pBuildItem = nullptr;
		m_p3MFModel = nullptr;
		m_pWrapper = nullptr;
	}

	std::string CToolpathPart::getUUID()
	{
		return m_sUUID;
	}

	std::string CToolpathPart::getMeshUUID()
	{
		return m_sMeshUUID;
	}

	std::string CToolpathPart::getName()
	{
		return m_sName;
	}

	std::string CToolpathPart::getPartNumber()
	{
		return m_sPartNumber;
	}

	Lib3MF::PModel CToolpathPart::getModel()
	{
		return m_p3MFModel;
	}

	Lib3MF::PBuildItem CToolpathPart::getBuildItem()
	{
		return m_pBuildItem;
	}

}


