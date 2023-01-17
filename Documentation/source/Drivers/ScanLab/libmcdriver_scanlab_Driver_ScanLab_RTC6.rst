
CDriver_ScanLab_RTC6
====================================================================================================


.. cpp:class:: LibMCDriver_ScanLab::CDriver_ScanLab_RTC6 : public CDriver_ScanLab 

	A LaserScanner Driver for a single laser ScanLAB RTC6 card.




	.. cpp:function:: void SetToSimulationMode()

		Turns the driver into a simulation mode.



	.. cpp:function:: bool IsSimulationMode()

		Returns if the driver is in simulation mode.

		:returns: Flag if driver is in simulation mode.


	.. cpp:function:: void Initialise(const std::string & sIP, const std::string & sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout, const LibMCDriver_ScanLab_uint32 nSerialNumber)

		Initializes the RTC6 Scanner Driver.

		:param sIP: IP Network Address. Empty string for local card. 
		:param sNetmask: IP Netmask Address. Empty string for local card. 
		:param nTimeout: Time out in microseconds. 
		:param nSerialNumber: Desired Serial Number of card. 


	.. cpp:function:: void LoadFirmware(const std::string & sFirmwareResource, const std::string & sFPGAResource, const std::string & sAuxiliaryResource)

		Loads the firmware from the driver resources.

		:param sFirmwareResource: resource name of the firmware program file. 
		:param sFPGAResource: resource name of the firmware FPGA file. 
		:param sAuxiliaryResource: resource name of the binary auxiliary file. 


	.. cpp:function:: void LoadCustomFirmware(const CInputVector<LibMCDriver_ScanLab_uint8> & FirmwareDataBuffer, const CInputVector<LibMCDriver_ScanLab_uint8> & FPGADataBuffer, const CInputVector<LibMCDriver_ScanLab_uint8> & AuxiliaryDataBuffer)

		Loads the firmware from custom resources.

		:param FirmwareDataBuffer: byte array of the firmware program file. 
		:param FPGADataBuffer: byte array of the firmware FPGA file. 
		:param AuxiliaryDataBuffer: byte array of the binary auxiliary file. 


	.. cpp:function:: void SetCorrectionFile(const CInputVector<LibMCDriver_ScanLab_uint8> & CorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB)

		Sets the correction file stream.

		:param CorrectionFileBuffer: binary data of the correction file. 
		:param nTableNumber: Correction table index of card (1..8) 
		:param nDimension: Is it a 2D or 3D correction file. 
		:param nTableNumberHeadA: Table number of Head A. 
		:param nTableNumberHeadB: Table number of Head B. 


	.. cpp:function:: void ConfigureLaserMode(const eLaserMode eLaserMode, const eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive)

		Configures the laser mode.

		:param eLaserMode: Laser Mode Enum 
		:param eLaserPort: Laser Port Enum 
		:param dMaxLaserPower: Maximum laser power. 
		:param bFinishLaserPulseAfterOn: Finish laser pulse after LaserOn 
		:param bPhaseShiftOfLaserSignal: 180 degree phase shift of Laser signal 
		:param bLaserOnSignalLowActive: Set Laser On Signal Low Active 
		:param bLaserHalfSignalsLowActive: Set Laser Half Signal Low Active 
		:param bSetDigitalInOneHighActive: Set Digital In 1 high Active 
		:param bOutputSynchronizationActive: Output synchronization active 


	.. cpp:function:: void ConfigureDelays(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay, const LibMCDriver_ScanLab_double dMarkDelay, const LibMCDriver_ScanLab_double dJumpDelay, const LibMCDriver_ScanLab_double dPolygonDelay)

		Configures the default laser and scanner delays.

		:param dLaserOnDelay: Laser On Delay in Microseconds 
		:param dLaserOffDelay: Laser Off Delay in Microseconds 
		:param dMarkDelay: Mark delay in microseconds (will be rounded to a multiple of 10) 
		:param dJumpDelay: Jump delay in microseconds (will be rounded to a multiple of 10) 
		:param dPolygonDelay: Polygon delay in microseconds (will be rounded to a multiple of 10) 


	.. cpp:function:: void DrawLayer(const std::string & sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex)

		Draws a layer of a build stream. Blocks until the layer is drawn.

		:param sStreamUUID: UUID of the build stream. Must have been loaded in memory by the system. 
		:param nLayerIndex: Layer index of the build file. 


	.. cpp:function:: void SetCommunicationTimeouts(const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier)

		Set RTC Ethernet communication timeouts

		:param dInitialTimeout: Initial timeout in ms 
		:param dMaxTimeout: Max timeout in ms 
		:param dMultiplier: Multiplier 


	.. cpp:function:: void GetCommunicationTimeouts(LibMCDriver_ScanLab_double & dInitialTimeout, LibMCDriver_ScanLab_double & dMaxTimeout, LibMCDriver_ScanLab_double & dMultiplier)

		Get RTC Ethernet communication timeouts

		:param dInitialTimeout: Initial timeout in ms 
		:param dMaxTimeout: Max timeout in ms 
		:param dMultiplier: Multiplier 


.. cpp:type:: std::shared_ptr<CDriver_ScanLab_RTC6> LibMCDriver_ScanLab::PDriver_ScanLab_RTC6

	Shared pointer to CDriver_ScanLab_RTC6 to easily allow reference counting.

