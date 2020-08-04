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

#include "amc_toolpathentity.hpp"
#include "libmc_interfaceexception.hpp"

namespace AMC {

	CToolpathEntity::CToolpathEntity(LibMCData::PStorageStream pStorageStream, Lib3MF::PWrapper p3MFWrapper)
		: m_ReferenceCount (0), m_pStorageStream (pStorageStream)
	{
		if (pStorageStream.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (p3MFWrapper.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		void* pReadCallback = nullptr;
		void* pSeekCallback = nullptr;
		void* pUserData = nullptr;

		// It is important to keep the storage stream in the same context as the 3MF Model - as the callbacks would be invalidated otherwise!
		m_pStorageStream->GetCallbacks(pReadCallback, pSeekCallback, pUserData);
		uint64_t nStreamSize = m_pStorageStream->GetSize();

		m_p3MFModel = p3MFWrapper->CreateModel();
		m_p3MFReader = m_p3MFModel->QueryReader("3mf");
		m_p3MFReader->AddRelationToRead("http://schemas.microsoft.com/3dmanufacturing/2019/05/toolpath");
		m_p3MFReader->ReadFromCallback((Lib3MF::ReadCallback) pReadCallback, nStreamSize, (Lib3MF::SeekCallback) pSeekCallback, pUserData);

		auto pToolpathIterator = m_p3MFModel->GetToolpaths();
		if (!pToolpathIterator->MoveNext())
			throw ELibMCInterfaceException(LIBMC_ERROR_TOOLPATHENTITYINVALIDFILE);

		m_pToolpath = pToolpathIterator->GetCurrentToolpath();


	}

	CToolpathEntity::~CToolpathEntity()
	{

	}

	void CToolpathEntity::IncRef()
	{
		if (m_ReferenceCount >= AMC_TOOLPATH_MAXREFCOUNT)
			throw ELibMCInterfaceException(LIBMC_ERROR_TOOLPATHENTITYREFERENCEERROR);

		m_ReferenceCount++;

	}

	bool CToolpathEntity::DecRef()
	{
		if (m_ReferenceCount == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_TOOLPATHENTITYREFERENCEERROR);

		m_ReferenceCount--;
		return (m_ReferenceCount == 0);
	}

	uint32_t CToolpathEntity::getLayerCount()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);		
		return m_pToolpath->GetLayerCount ();
	}

	PToolpathLayerData CToolpathEntity::readLayer(uint32_t nLayerIndex)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		double dUnits = m_pToolpath->GetUnits();

		auto p3MFLayerData = m_pToolpath->ReadLayerData(nLayerIndex);
		return std::make_shared<CToolpathLayerData> (p3MFLayerData, dUnits);
	}


}


