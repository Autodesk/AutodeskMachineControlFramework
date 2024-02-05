
CImageData
====================================================================================================


.. cpp:class:: LibMCEnv::CImageData : public CBase 

	In memory representation of an image.




	.. cpp:function:: eImagePixelFormat GetPixelFormat()

		Returns Pixel format of the image.

		:returns: Pixel Format of image


	.. cpp:function:: void ChangePixelFormat(const eImagePixelFormat ePixelFormat)

		Changes Pixel format of the image. Might lose alpha or color information during the process.

		:param ePixelFormat: new Pixel Format of image 


	.. cpp:function:: void GetDPI(LibMCEnv_double & dDPIValueX, LibMCEnv_double & dDPIValueY)

		Returns DPI values in X and Y.

		:param dDPIValueX: DPI value in X 
		:param dDPIValueY: DPI value in Y 


	.. cpp:function:: void SetDPI(const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY)

		Sets DPI values in X and Y.

		:param dDPIValueX: new DPI value in X 
		:param dDPIValueY: new DPI value in Y 


	.. cpp:function:: void GetSizeInMM(LibMCEnv_double & dSizeX, LibMCEnv_double & dSizeY)

		Returns image sizes inmm.

		:param dSizeX: Size in X in mm 
		:param dSizeY: Size in Y in mm 


	.. cpp:function:: void GetSizeInPixels(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY)

		Returns image pixel sizes.

		:param nPixelSizeX: Number of pixels in X 
		:param nPixelSizeY: Number of pixels in Y 


	.. cpp:function:: void ResizeImage(LibMCEnv_uint32 & nPixelSizeX, LibMCEnv_uint32 & nPixelSizeY)

		Resizes Image pixel data.

		:param nPixelSizeX: Number of pixels in X 
		:param nPixelSizeY: Number of pixels in Y 


	.. cpp:function:: void LoadPNG(std::vector<LibMCEnv_uint8> & PNGDataBuffer)

		Loads a PNG from a binary array. Supports RGB, RGBA and Greyscale images.

		:param PNGDataBuffer: PNG Data stream. 


	.. cpp:function:: void EncodePNG()

		Encodes PNG and stores data stream in image object.



	.. cpp:function:: void GetEncodedPNGData(std::vector<LibMCEnv_uint8> & PNGDataBuffer)

		Retrieves encoded data stream of image object. MUST have been encoded with EncodePNG before.

		:param PNGDataBuffer: PNG Data stream. 


	.. cpp:function:: void ClearEncodedPNGData()

		Releases encoded data stream of image object.



	.. cpp:function:: void Clear(const LibMCEnv_uint32 nValue)

		Sets all pixels to a single value.

		:param nValue: Pixel value. 


	.. cpp:function:: LibMCEnv_uint32 GetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY)

		Returns one pixel of an image.

		:param nX: Pixel coordinate in X 
		:param nY: Pixel coordinate in Y 
		:returns: Pixel value at this position


	.. cpp:function:: void SetPixel(const LibMCEnv_uint32 nX, const LibMCEnv_uint32 nY, const LibMCEnv_uint32 nValue)

		Sets one pixel of an image.

		:param nX: Pixel coordinate in X 
		:param nY: Pixel coordinate in Y 
		:param nValue: New Pixel value at this position 


	.. cpp:function:: void GetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, std::vector<LibMCEnv_uint8> & ValueBuffer)

		Returns a subset of an image or the whole image data.

		:param nXMin: Min Pixel coordinate in X. MUST be within image bounds. 
		:param nYMin: Min Pixel coordinate in Y. MUST be within image bounds. 
		:param nXMax: Max Pixel coordinate in X. MUST be within image bounds. MUST be larger or equal than MinX 
		:param nYMax: Max Pixel coordinate in Y. MUST be within image bounds. MUST be larger or equal than MinY 
		:param ValueBuffer: Pixel values of the rectangle, rowwise array. MUST have the exact number of pixels in size and 1, 3 or 4 bytes per pixel, depending on pixel format. 


	.. cpp:function:: void SetPixelRange(const LibMCEnv_uint32 nXMin, const LibMCEnv_uint32 nYMin, const LibMCEnv_uint32 nXMax, const LibMCEnv_uint32 nYMax, const CInputVector<LibMCEnv_uint8> & ValueBuffer)

		Exchanges a subset of an image or the whole image data.

		:param nXMin: Min Pixel coordinate in X. MUST be within image bounds. 
		:param nYMin: Min Pixel coordinate in Y. MUST be within image bounds. 
		:param nXMax: Max Pixel coordinate in X. MUST be within image bounds. MUST be larger or equal than MinX 
		:param nYMax: Max Pixel coordinate in Y. MUST be within image bounds. MUST be larger or equal than MinY 
		:param ValueBuffer: New pixel values of the rectangle, rowwise array. MUST have the exact number of pixels in size and 1, 3 or 4 bytes per pixel, depending on pixel format. 


.. cpp:type:: std::shared_ptr<CImageData> LibMCEnv::PImageData

	Shared pointer to CImageData to easily allow reference counting.

