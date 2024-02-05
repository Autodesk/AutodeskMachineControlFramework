
CDriver_Rasterizer
====================================================================================================


.. cpp:class:: LibMCDriver_Rasterizer::CDriver_Rasterizer : public CDriver 

	A generic Image rasterizer.




	.. cpp:function:: PSliceStack CreateSliceStack(const LibMCDriver_Rasterizer_uint32 nLayerCount, const LibMCDriver_Rasterizer_double dLayerThickness, const LibMCDriver_Rasterizer_double dBottomZ)

		creates an empty slice stack.

		:param nLayerCount: Number of layers. MUST be positive. 
		:param dLayerThickness: Layerthickness in mm. MUST be positive. 
		:param dBottomZ: Bottom Z value in mm. 
		:returns: Instance of Slice Stack


	.. cpp:function:: PLayerObject CreateEmptyLayer()

		creates an empty layer object.

		:returns: Instance of a Layer Object


	.. cpp:function:: PRasterizer RegisterInstance(const std::string & sIdentifier, const LibMCDriver_Rasterizer_uint32 nPixelSizeX, const LibMCDriver_Rasterizer_uint32 nPixelSizeY, const LibMCDriver_Rasterizer_double dDPIX, const LibMCDriver_Rasterizer_double dDPIY)

		creates a new rasterization instance.

		:param sIdentifier: Identifier object. MUST be unique. 
		:param nPixelSizeX: Pixel size in X. MUST be positive. 
		:param nPixelSizeY: Pixel size in Y. MUST be positive. 
		:param dDPIX: DPI in X. MUST be positive. 
		:param dDPIY: DPI in Y. MUST be positive. 
		:returns: Instance of Rasterizer


	.. cpp:function:: void UnregisterInstance(const std::string & sIdentifier)

		destroys a rasterization instance. Instance MUST exist.

		:param sIdentifier: Identifier object. MUST be unique. 


	.. cpp:function:: bool HasInstance(const std::string & sIdentifier)

		Checks if a rasterization instance exists.

		:param sIdentifier: Identifier object. MUST be unique. 
		:returns: Flag if instance exists.


	.. cpp:function:: PRasterizer GetInstance(const std::string & sIdentifier)

		retrieves an existing rasterization instance.

		:param sIdentifier: Identifier object. MUST be unique. 
		:returns: Instance of Rasterizer


.. cpp:type:: std::shared_ptr<CDriver_Rasterizer> LibMCDriver_Rasterizer::PDriver_Rasterizer

	Shared pointer to CDriver_Rasterizer to easily allow reference counting.

