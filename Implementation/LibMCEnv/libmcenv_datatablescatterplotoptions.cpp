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


Abstract: This is a stub class definition of CDataTableScatterPlotOptions

*/

#include "libmcenv_datatablescatterplotoptions.hpp"
#include "libmcenv_interfaceexception.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CDataTableScatterPlotOptions 
**************************************************************************************************************************/

CDataTableScatterPlotOptions::CDataTableScatterPlotOptions()
	: m_dXAxisScaleFactor (1.0), m_dXAxisOffset (0.0), 
	  m_dYAxisScaleFactor (1.0), m_dYAxisOffset (0.0)
{

}

CDataTableScatterPlotOptions::~CDataTableScatterPlotOptions()
{

}

void CDataTableScatterPlotOptions::SetXAxisColumn(const std::string & sColumnIdentifier, const LibMCEnv_double dScaleFactor, const LibMCEnv_double dOffset)
{
	m_sXAxisColumnIdentifier = sColumnIdentifier;
	m_dXAxisScaleFactor = dScaleFactor;
	m_dXAxisOffset = dOffset;
}

std::string CDataTableScatterPlotOptions::GetXAxisColumn()
{
	return m_sXAxisColumnIdentifier;
}

LibMCEnv_double CDataTableScatterPlotOptions::GetXAxisScaling()
{
	return m_dXAxisScaleFactor;
}

LibMCEnv_double CDataTableScatterPlotOptions::GetXAxisOffset()
{
	return m_dXAxisOffset;
}

void CDataTableScatterPlotOptions::SetYAxisColumn(const std::string & sColumnIdentifier, const LibMCEnv_double dScaleFactor, const LibMCEnv_double dOffset)
{
	m_sYAxisColumnIdentifier = sColumnIdentifier;
	m_dYAxisScaleFactor = dScaleFactor;
	m_dYAxisOffset = dOffset;
}

std::string CDataTableScatterPlotOptions::GetYAxisColumn()
{
	return m_sYAxisColumnIdentifier;
}

LibMCEnv_double CDataTableScatterPlotOptions::GetYAxisScaling()
{
	return m_dYAxisScaleFactor;
}

LibMCEnv_double CDataTableScatterPlotOptions::GetYAxisOffset()
{
	return m_dYAxisOffset;
}

void CDataTableScatterPlotOptions::AddDataChannel(const std::string & sChannelIdentifier, const std::string & sColumnIdentifier, const LibMCEnv_double dScaleFactor, const LibMCEnv_double dOffsetFactor, const LibMCEnv_uint32 nColor)
{
	
}

