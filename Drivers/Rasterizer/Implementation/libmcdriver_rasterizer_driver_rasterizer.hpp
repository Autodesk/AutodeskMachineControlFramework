/*++

Copyright (C) 2022 Autodesk Inc.

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


Abstract: This is the class declaration of CDriver_Rasterizer

*/


#ifndef __LIBMCDRIVER_RASTERIZER_DRIVER_RASTERIZER
#define __LIBMCDRIVER_RASTERIZER_DRIVER_RASTERIZER

#include "libmcdriver_rasterizer_interfaces.hpp"
#include "libmcdriver_rasterizer_rasterizer.hpp"

// Parent classes
#include "libmcdriver_rasterizer_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>

namespace LibMCDriver_Rasterizer {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_Rasterizer 
**************************************************************************************************************************/

class CDriver_Rasterizer : public virtual IDriver_Rasterizer, public virtual CDriver {
private:

	std::string m_sName;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	std::map<std::string, PRasterizerInstance> m_Instances;

protected:

public:

	CDriver_Rasterizer(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_Rasterizer();


	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_Rasterizer_uint32& nMajor, LibMCDriver_Rasterizer_uint32& nMinor, LibMCDriver_Rasterizer_uint32& nMicro, std::string& sBuild) override;

	void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;

	void QueryParameters() override;

	ISliceStack * CreateSliceStack(const LibMCDriver_Rasterizer_uint32 nLayerCount, const LibMCDriver_Rasterizer_double dLayerThickness, const LibMCDriver_Rasterizer_double dBottomZ) override;

	ILayerObject * CreateEmptyLayer() override;

	IRasterizer * RegisterInstance(const std::string & sIdentifier, const LibMCDriver_Rasterizer_uint32 nPixelSizeX, const LibMCDriver_Rasterizer_uint32 nPixelSizeY, const LibMCDriver_Rasterizer_double dDPIX, const LibMCDriver_Rasterizer_double dDPIY) override;

	void UnregisterInstance(const std::string & sIdentifier) override;

	bool HasInstance(const std::string& sIdentifier) override;

	IRasterizer * GetInstance(const std::string & sIdentifier) override;

};

} // namespace Impl
} // namespace LibMCDriver_Rasterizer

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_RASTERIZER_DRIVER_RASTERIZER
