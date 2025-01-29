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


Abstract: This is the class declaration of CDataTableScatterPlotOptions

*/


#ifndef __LIBMCENV_DATATABLESCATTERPLOTOPTIONS
#define __LIBMCENV_DATATABLESCATTERPLOTOPTIONS

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDataTableScatterPlotOptions 
**************************************************************************************************************************/

class CDataTableScatterPlotOptions : public virtual IDataTableScatterPlotOptions, public virtual CBase {
private:

	std::string m_sXAxisColumnIdentifier;
	double m_dXAxisScaleFactor;
	double m_dXAxisOffset;

	std::string m_sYAxisColumnIdentifier;
	double m_dYAxisScaleFactor;
	double m_dYAxisOffset;

public:

	CDataTableScatterPlotOptions();

	virtual ~CDataTableScatterPlotOptions();

	void SetXAxisColumn(const std::string & sColumnIdentifier, const LibMCEnv_double dScaleFactor, const LibMCEnv_double dOffsetFactor) override;

	std::string GetXAxisColumn() override;

	LibMCEnv_double GetXAxisScaling() override;

	LibMCEnv_double GetXAxisOffset() override;

	void SetYAxisColumn(const std::string & sColumnIdentifier, const LibMCEnv_double dScaleFactor, const LibMCEnv_double dOffsetFactor) override;

	std::string GetYAxisColumn() override;

	LibMCEnv_double GetYAxisScaling() override;

	LibMCEnv_double GetYAxisOffset() override;

	void AddDataChannel(const std::string & sChannelIdentifier, const std::string & sColumnIdentifier, const LibMCEnv_double dScaleFactor, const LibMCEnv_double dOffsetFactor, const LibMCEnv_uint32 nColor) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_DATATABLESCATTERPLOTOPTIONS
