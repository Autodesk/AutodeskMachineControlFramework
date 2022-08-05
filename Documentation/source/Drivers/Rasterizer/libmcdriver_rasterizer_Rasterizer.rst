
CRasterizer
====================================================================================================


.. cpp:class:: LibMCDriver_Rasterizer::CRasterizer : public CBase 

	Rasterizer for Layers.




	.. cpp:function:: void GetDPI(LibMCDriver_Rasterizer_double & dDPIValueX, LibMCDriver_Rasterizer_double & dDPIValueY)

		Returns DPI values in X and Y.

		:param dDPIValueX: DPI value in X 
		:param dDPIValueY: DPI value in Y 


	.. cpp:function:: void GetSize(LibMCDriver_Rasterizer_double & dSizeX, LibMCDriver_Rasterizer_double & dSizeY)

		Returns image sizes.

		:param dSizeX: Size in X in mm 
		:param dSizeY: Size in Y in mm 


	.. cpp:function:: void GetPixelSize(LibMCDriver_Rasterizer_uint32 & nPixelSizeX, LibMCDriver_Rasterizer_uint32 & nPixelSizeY)

		Returns image pixel sizes.

		:param nPixelSizeX: Number of pixels in X 
		:param nPixelSizeY: Number of pixels in Y 


	.. cpp:function:: void GetPosition(LibMCDriver_Rasterizer_double & dPositionX, LibMCDriver_Rasterizer_double & dPositionY)

		Returns image position.

		:param dPositionX: Position in X in mm 
		:param dPositionY: Position in Y in mm 


	.. cpp:function:: void SetPosition(const LibMCDriver_Rasterizer_double dPositionX, const LibMCDriver_Rasterizer_double dPositionY)

		Sets image position.

		:param dPositionX: Position in X in mm 
		:param dPositionY: Position in Y in mm 


	.. cpp:function:: void SetSubsampling(const LibMCDriver_Rasterizer_uint32 nSubsamplingX, const LibMCDriver_Rasterizer_uint32 nSubsamplingY)

		Set subsampling values in X and Y.

		:param nSubsamplingX: Subsampling in X 
		:param nSubsamplingY: Subsampling in Y 


	.. cpp:function:: void GetSubsampling(LibMCDriver_Rasterizer_uint32 & nSubsamplingX, LibMCDriver_Rasterizer_uint32 & nSubsamplingY)

		Returns subsampling values in X and Y.

		:param nSubsamplingX: Subsampling in X 
		:param nSubsamplingY: Subsampling in Y 


	.. cpp:function:: void SetSamplingParameters(const LibMCDriver_Rasterizer_uint32 nUnitsPerSubpixel, const LibMCDriver_Rasterizer_uint32 nPixelsPerBlock)

		Set sampling parameters of algorithm.

		:param nUnitsPerSubpixel: Units per subpixel. Line coordinates will be discretized with this value. Minimum 4, Maximum 1048576. Must be even. 
		:param nPixelsPerBlock: Pixels per lookup block. Improves calculation speed. Minimum 4, Maximum 1024. 


	.. cpp:function:: void GetSamplingParameters(LibMCDriver_Rasterizer_uint32 & nUnitsPerSubpixel, LibMCDriver_Rasterizer_uint32 & nPixelsPerBlock)

		Returns sampling parameters of algorithm.

		:param nUnitsPerSubpixel: Units per subpixel. Line coordinates will be discretized with this value. Minimum 4, Maximum 1048576. Must be even. 
		:param nPixelsPerBlock: Pixels per lookup block. Improves calculation speed. Minimum 4, Maximum 1024. 


	.. cpp:function:: void AddLayer(classParam<CLayerObject> pLayerObject)

		Adds a layer object to subsample.

		:param pLayerObject: Layer object instance. 


	.. cpp:function:: void CalculateImage(classParam<LibMCEnv::CImageData> pImageObject, const bool bAntialiased)

		Calculates the image.

		:param pImageObject: ImageObject Instance to render into 
		:param bAntialiased: Image output is greyscale if true, black and white with 0.5 threshold if false. 


.. cpp:type:: std::shared_ptr<CRasterizer> LibMCDriver_Rasterizer::PRasterizer

	Shared pointer to CRasterizer to easily allow reference counting.

