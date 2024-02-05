
CLayerObject
====================================================================================================


.. cpp:class:: LibMCDriver_Rasterizer::CLayerObject : public CBase 

	In-memory representation of a layer object.




	.. cpp:function:: LibMCDriver_Rasterizer_uint32 GetEntityCount()

		Returns count of Entity in Layer.

		:returns: Number of entity in layer.


	.. cpp:function:: void GetEntity(const LibMCDriver_Rasterizer_uint32 nEntityIndex, eGeometryType & eGeometryType, std::vector<sPosition2D> & PointsBuffer)

		Returns a entity in a layer.

		:param nEntityIndex: Index of entity in layer. 
		:param eGeometryType: Geometry type of Entity. 
		:param PointsBuffer: Points of Entity. 


	.. cpp:function:: LibMCDriver_Rasterizer_uint32 AddEntity(const CInputVector<sPosition2D> & PointsBuffer, const eGeometryType eGeometryType)

		Adds a entity to a layer.

		:param PointsBuffer: Points of Entity. 
		:param eGeometryType: Geometry type of Entity. 
		:returns: Index of entity in layer.


	.. cpp:function:: PLayerObject RemoveSelfIntersections()

		Removes self-intersections of a layer.

		:returns: New Layer Object Instance


	.. cpp:function:: void MergeInto(classParam<CLayerObject> pOtherLayerObject)

		Merges layer into another layer object.

		:param pOtherLayerObject: Layer object to modify. 


	.. cpp:function:: PLayerObject CalculateOffset(const LibMCDriver_Rasterizer_double dOffsetValue)

		Calculates an offset to a layer.

		:param dOffsetValue: Offset in mm 
		:returns: Offsetted Layer Object Instance


	.. cpp:function:: PLayerObject ThickenPolylines(const LibMCDriver_Rasterizer_double dThickness)

		Thickens up polylines of a layer.

		:param dThickness: Thickness in mm 
		:returns: Offsetted Layer Object Instance


	.. cpp:function:: PLayerObject ThickenHatches(const LibMCDriver_Rasterizer_double dThickness)

		Thickens up hatches of a layer.

		:param dThickness: Thickness in mm 
		:returns: Offsetted Layer Object Instance


	.. cpp:function:: PLayerObject DistortLayer(const LibMCDriver_Rasterizer_double dRefinementValue, const DistortionCallback pDistortionCallback, const LibMCDriver_Rasterizer_pvoid pUserData)

		Distorts a layer with a callback function

		:param dRefinementValue: Lines longer than this value are split up in smaller segments to not loose distortion information. 
		:param pDistortionCallback: pointer to the callback function. 
		:param pUserData: pointer to arbitrary user data that is passed without modification to the callback. 
		:returns: Offsetted Layer Object Instance


.. cpp:type:: std::shared_ptr<CLayerObject> LibMCDriver_Rasterizer::PLayerObject

	Shared pointer to CLayerObject to easily allow reference counting.

