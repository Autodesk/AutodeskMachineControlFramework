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
#define IMAGEDATA_MAXPNGSIZE (1024UL * 1024UL * 1024UL)
#define IMAGEDATA_MAXJPEGSIZE (1024UL * 1024UL * 1024UL)

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

	size_t getBytesPerPixel();

	
	void writeToRawMemoryEx_BlackWhite1bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLinePixelOffset);
	void writeToRawMemoryEx_GreyScale2bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLinePixelOffset);
	void writeToRawMemoryEx_GreyScale4bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLinePixelOffset);
	void writeToRawMemoryEx_GreyScale8bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLinePixelOffset);
	void writeToRawMemoryEx_RGB16bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineByteOffset);
	void writeToRawMemoryEx_RGB24bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineByteOffset);
	void writeToRawMemoryEx_RGBA32bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, uint8_t* pTarget, uint32_t nYLineByteOffset);

	void readFromRawMemoryEx_GreyScale8bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, const uint8_t* pSource, uint32_t nYLineOffset);
	void readFromRawMemoryEx_RGB16bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, const uint8_t* pSource, uint32_t nYLineOffset);
	void readFromRawMemoryEx_RGB24bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, const uint8_t* pSource, uint32_t nYLineOffset);
	void readFromRawMemoryEx_RGBA32bit(uint32_t nStartX, uint32_t nStartY, uint32_t nCountX, uint32_t nCountY, const uint8_t* pSource, uint32_t nYLineOffset);

	void convertFromYUY2_GreyScale8bit(const uint8_t* pSource);
	void convertFromYUY2_RGB16bit(const uint8_t* pSource);
	void convertFromYUY2_RGB24bit(const uint8_t* pSource);
	void convertFromYUY2_RGBA32bit(const uint8_t* pSource);

	uint32_t encodeBlackWhitePalettePNG(std::vector<uint8_t>& pngBuffer, std::vector<uint8_t>& imageBuffer, uint32_t imageSizeX, uint32_t imageSizeY);

protected:

public:

	static CImageData* createFromPNG(const uint8_t * pBuffer, uint64_t nBufferSize, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat);

	static CImageData* createFromJPEG(const uint8_t* pBuffer, uint64_t nBufferSize, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat);

	static CImageData* createFromRGB24(const uint8_t* pBuffer, uint64_t nBufferSize, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat);

	static CImageData* createFromRGBA32(const uint8_t* pBuffer, uint64_t nBufferSize, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat);

	static CImageData* createFromYUY2(const uint8_t* pBuffer, uint64_t nBufferSize, const uint32_t nPixelCountX, const uint32_t nPixelCountY, const double dDPIValueX, const double dDPIValueY, eImagePixelFormat pixelFormat);

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

	IPNGImageStoreOptions* CreatePNGOptions() override;

	IPNGImageData* CreatePNGImage(IPNGImageStoreOptions* pPNGStorageOptions) override;

	IJPEGImageStoreOptions* CreateJPEGOptions() override;

	IJPEGImageData* CreateJPEGImage(IJPEGImageStoreOptions* pJPEGStorageOptions) override;

	void Clear(const LibMCEnv_uint32 nValue) override;

	LibMCEnv_uint32 GetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY) override;

	void SetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY, const LibMCEnv_uint32 nValue) override;

	void GetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, LibMCEnv_uint64 nValueBufferSize, LibMCEnv_uint64* pValueNeededCount, LibMCEnv_uint8 * pValueBuffer) override;

	void SetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, const LibMCEnv_uint64 nValueBufferSize, const LibMCEnv_uint8 * pValueBuffer) override;

	void GetPixels(const LibMCEnv_uint32 nStartX, const LibMCEnv_uint32 nStartY, const LibMCEnv_uint32 nCountX, const LibMCEnv_uint32 nCountY, const LibMCEnv::eImagePixelFormat eTargetFormat, LibMCEnv_uint64 nValueBufferSize, LibMCEnv_uint64* pValueNeededCount, LibMCEnv_uint8* pValueBuffer) override;

	void SetPixels(const LibMCEnv_uint32 nStartX, const LibMCEnv_uint32 nStartY, const LibMCEnv_uint32 nCountX, const LibMCEnv_uint32 nCountY, const LibMCEnv::eImagePixelFormat eSourceFormat, const LibMCEnv_uint64 nValueBufferSize, const LibMCEnv_uint8* pValueBuffer) override;

	void WriteToRawMemory(const LibMCEnv_uint32 nStartX, const LibMCEnv_uint32 nStartY, const LibMCEnv_uint32 nCountX, const LibMCEnv_uint32 nCountY, const LibMCEnv::eImagePixelFormat eTargetFormat, const LibMCEnv_pvoid pTarget, const LibMCEnv_uint32 nYLineOffset) override;

	void ReadFromRawMemory(const LibMCEnv_uint32 nStartX, const LibMCEnv_uint32 nStartY, const LibMCEnv_uint32 nCountX, const LibMCEnv_uint32 nCountY, const LibMCEnv::eImagePixelFormat eSourceFormat, const LibMCEnv_pvoid pSource, const LibMCEnv_uint32 nYLineOffset) override;

	void SetPixelsFromRawYUY2Data(const LibMCEnv_uint64 nYUY2DataBufferSize, const LibMCEnv_uint8* pYUY2DataBuffer) override;

	std::vector <uint8_t> & getPixelData ();
};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_IMAGEDATA
