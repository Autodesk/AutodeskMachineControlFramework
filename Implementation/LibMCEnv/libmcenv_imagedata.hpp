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


Abstract: This is the class declaration of CImageData

*/


#ifndef __LIBMCENV_IMAGEDATA
#define __LIBMCENV_IMAGEDATA

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
 Class declaration of CImageData 
**************************************************************************************************************************/

class CImageData : public virtual IImageData, public virtual CBase {
private:

	uint32_t m_nPixelCountX;
	uint32_t m_nPixelCountY;
	double m_dDPIValueX;
	double m_dDPIValueY;
	double m_dPositionX;
	double m_dPositionY;
	
	eImagePixelFormat m_PixelFormat;

	std::unique_ptr<std::vector <uint8_t>> m_PixelData;

	// Depreciated cache for encoding PNG images via EncodePNG ()
	std::unique_ptr<IPNGImageData> m_EncodedPNGDataCache;

	size_t getBytesPerPixel();

protected:

public:

	static CImageData* createFromPNG(const uint8_t * pBuffer, uint64_t nBufferSize, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat);
	static CImageData* createEmpty(const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat);

	CImageData(std::vector<uint8_t> * pPixelData, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat, bool bDoClear);
	virtual ~CImageData();


	LibMCEnv::eImagePixelFormat GetPixelFormat() override;

	void ChangePixelFormat(const LibMCEnv::eImagePixelFormat ePixelFormat) override;

	void GetDPI(LibMCEnv_double & dDPIValueX, LibMCEnv_double & dDPIValueY) override;

	void SetDPI(const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY) override;

	void GetSizeInMM(LibMCEnv_double & dSizeX, LibMCEnv_double & dSizeY) override;

	void GetSizeInPixels(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY) override;

	void ResizeImage(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY) override;

	void LoadPNG(const LibMCEnv_uint64 nPNGDataBufferSize, const LibMCEnv_uint8* pPNGDataBuffer) override;

	IPNGImageData* CreatePNGImage(IPNGImageStoreOptions* pPNGStorageOptions) override;

	void EncodePNG() override;

	void GetEncodedPNGData(LibMCEnv_uint64 nPNGDataBufferSize, LibMCEnv_uint64* pPNGDataNeededCount, LibMCEnv_uint8 * pPNGDataBuffer) override;

	void ClearEncodedPNGData() override;

	void Clear(const LibMCEnv_uint32 nValue) override;

	LibMCEnv_uint32 GetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY) override;

	void SetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY, const LibMCEnv_uint32 nValue) override;

	void GetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, LibMCEnv_uint64 nValueBufferSize, LibMCEnv_uint64* pValueNeededCount, LibMCEnv_uint8 * pValueBuffer) override;

	void SetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, const LibMCEnv_uint64 nValueBufferSize, const LibMCEnv_uint8 * pValueBuffer) override;

	std::vector <uint8_t> & getPixelData ();
};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_IMAGEDATA
