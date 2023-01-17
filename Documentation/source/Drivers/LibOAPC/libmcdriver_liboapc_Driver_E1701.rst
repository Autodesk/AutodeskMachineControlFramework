
CDriver_E1701
====================================================================================================


.. cpp:class:: LibMCDriver_LibOAPC::CDriver_E1701 : public CDriver 

	A Driver for a E1701 Scanner Driver.




	.. cpp:function:: void SetCorrectionFile(const CInputVector<LibMCDriver_LibOAPC_uint8> & CorrectionFileBuffer)

		Sets the correction file stream.

		:param CorrectionFileBuffer: binary data of the correction file. 


	.. cpp:function:: void SetOpticalField(const LibMCDriver_LibOAPC_double dMinX, const LibMCDriver_LibOAPC_double dMinY, const LibMCDriver_LibOAPC_double dMaxX, const LibMCDriver_LibOAPC_double dMaxY)

		Sets the field size in mm of the optical system.

		:param dMinX: Minimum X Coordinate in mm 
		:param dMinY: Minimum Y Coordinate in mm 
		:param dMaxX: Maximum X Coordinate in mm 
		:param dMaxY: Maximum Y Coordinate in mm 


	.. cpp:function:: void SetLogicalField(const LibMCDriver_LibOAPC_int32 nMinX, const LibMCDriver_LibOAPC_int32 nMinY, const LibMCDriver_LibOAPC_int32 nMaxX, const LibMCDriver_LibOAPC_int32 nMaxY)

		Sets the field size in mm of the internal unit system.

		:param nMinX: Minimum X Coordinate in logical units 
		:param nMinY: Minimum Y Coordinate in logical units 
		:param nMaxX: Maximum X Coordinate in logical units 
		:param nMaxY: Maximum Y Coordinate in logical units 


	.. cpp:function:: void Connect(const std::string & sConnectionString, const std::string & sPassword)

		Creates and initializes the OAPC Scanner Driver.

		:param sConnectionString: Connection string of the device. 
		:param sPassword: Password of the device. Pass empty string if not used. 


	.. cpp:function:: void Disconnect()

		Disconnects from the Laser Scanner Emulator.



	.. cpp:function:: void DrawLayer(const std::string & sStreamUUID, const LibMCDriver_LibOAPC_uint32 nLayerIndex)

		Draws a layer of a build stream. Blocks until the layer is drawn.

		:param sStreamUUID: UUID of the build stream. Must have been loaded in memory by the system. 
		:param nLayerIndex: Layer index of the build file. 


.. cpp:type:: std::shared_ptr<CDriver_E1701> LibMCDriver_LibOAPC::PDriver_E1701

	Shared pointer to CDriver_E1701 to easily allow reference counting.

