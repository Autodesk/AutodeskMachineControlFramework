
CToolpathLayer
====================================================================================================


.. cpp:class:: LibMCEnv::CToolpathLayer : public CBase 

	




	.. cpp:function:: std::string GetLayerDataUUID()

		Retrieves the layerdata's uuid

		:returns: Returns the uuid value.


	.. cpp:function:: LibMCEnv_uint32 GetSegmentCount()

		Retrieves the count of segments.

		:returns: Count


	.. cpp:function:: void GetSegmentInfo(const LibMCEnv_uint32 nIndex, eToolpathSegmentType & eType, LibMCEnv_uint32 & nPointCount)

		Retrieves the segment type information .

		:param nIndex: Index. Must be between 0 and Count - 1. 
		:param eType: Segment Type 
		:param nPointCount: Point count of segment. 


	.. cpp:function:: std::string GetSegmentProfileUUID(const LibMCEnv_uint32 nIndex)

		Retrieves the assigned segment profile uuid.

		:param nIndex: Index. Must be between 0 and Count - 1. 
		:returns: Segment Profile UUID


	.. cpp:function:: std::string GetSegmentProfileValue(const LibMCEnv_uint32 nIndex, const std::string & sValueName)

		Retrieves an assigned profile custom value.

		:param nIndex: Index. Must be between 0 and Count - 1. 
		:param sValueName: Value Name to query for. 
		:returns: String Value.


	.. cpp:function:: LibMCEnv_double GetSegmentProfileTypedValue(const LibMCEnv_uint32 nIndex, const eToolpathProfileValueType eValueType)

		Retrieves an assigned profile value of a standard type.

		:param nIndex: Index. Must be between 0 and Count - 1. 
		:param eValueType: Enum to query for. MUST NOT be custom. 
		:returns: Double Value


	.. cpp:function:: std::string GetSegmentPartUUID(const LibMCEnv_uint32 nIndex)

		Retrieves the assigned segment part uuid.

		:param nIndex: Index. Must be between 0 and Count - 1. 
		:returns: Segment Part UUID


	.. cpp:function:: void GetSegmentPointData(const LibMCEnv_uint32 nIndex, std::vector<sPosition2D> & PointDataBuffer)

		Retrieves the assigned segment point list. For type hatch, the points are taken pairwise.

		:param nIndex: Index. Must be between 0 and Count - 1. 
		:param PointDataBuffer: The point data array. Positions are absolute in units. 


	.. cpp:function:: LibMCEnv_int32 GetZValue()

		Retrieves the layers Z Value in units.

		:returns: Z Value of the layer in units.


	.. cpp:function:: LibMCEnv_double GetUnits()

		Retrieves the toolpath units in mm.

		:returns: Toolpath units.


.. cpp:type:: std::shared_ptr<CToolpathLayer> LibMCEnv::PToolpathLayer

	Shared pointer to CToolpathLayer to easily allow reference counting.

