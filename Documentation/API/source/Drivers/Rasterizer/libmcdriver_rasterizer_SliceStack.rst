
CSliceStack
====================================================================================================


.. cpp:class:: LibMCDriver_Rasterizer::CSliceStack : public CBase 

	In-memory representation of a slice stack.




	.. cpp:function:: LibMCDriver_Rasterizer_uint32 GetLayerCount()

		Returns layer count of slice stack.

		:returns: Returns number of layers


	.. cpp:function:: LibMCDriver_Rasterizer_double GetLayerThickness()

		Returns layer thickness of slice stack.

		:returns: Returns layer thickness in mm


	.. cpp:function:: LibMCDriver_Rasterizer_double GetBottomZ()

		Returns Z value of bottom of slice stack in mm.

		:returns: Z-Value of bottom of slice stack


	.. cpp:function:: LibMCDriver_Rasterizer_double GetTopZ()

		Returns Z value of top of slice stack in mm.

		:returns: Z-Value of top of slice stack


	.. cpp:function:: PLayerObject GetLayer(const LibMCDriver_Rasterizer_uint32 nLayerIndex)

		Returns layer object for a given Z Value.

		:param nLayerIndex: Index of layer. 
		:returns: LayerObject Instance


.. cpp:type:: std::shared_ptr<CSliceStack> LibMCDriver_Rasterizer::PSliceStack

	Shared pointer to CSliceStack to easily allow reference counting.

