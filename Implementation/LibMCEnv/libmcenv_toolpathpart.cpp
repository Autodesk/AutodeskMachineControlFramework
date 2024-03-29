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


#include "libmcenv_toolpathpart.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CToolpathPart
**************************************************************************************************************************/
CToolpathPart::CToolpathPart(AMC::PToolpathPart pPart)
	: m_pPart (pPart)
{
	if (pPart.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CToolpathPart::~CToolpathPart()
{

}

std::string CToolpathPart::GetName()
{
	return m_pPart->getName();
}

std::string CToolpathPart::GetUUID()
{
	return m_pPart->getUUID();
}

std::string CToolpathPart::GetMeshUUID()
{
	return m_pPart->getMeshUUID();
}

LibMCEnv::sToolpathPartTransform CToolpathPart::GetTransform()
{
	LibMCEnv::sToolpathPartTransform transform;
	// TODO

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			transform.m_Matrix[i][j] = (i == j) ? 1.0 : 0.0;

	transform.m_Translation[0] = 0.0;
	transform.m_Translation[1] = 0.0;
	transform.m_Translation[2] = 0.0;

	return transform;

}
