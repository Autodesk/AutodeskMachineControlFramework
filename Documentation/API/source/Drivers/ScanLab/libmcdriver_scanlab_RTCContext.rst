
CRTCContext
====================================================================================================


.. cpp:class:: LibMCDriver_ScanLab::CRTCContext : public CBase 

	Selector of the RTC Controller Card




	.. cpp:function:: void LoadFirmware(const CInputVector<LibMCDriver_ScanLab_uint8> & FirmwareDataBuffer, const CInputVector<LibMCDriver_ScanLab_uint8> & FPGADataBuffer, const CInputVector<LibMCDriver_ScanLab_uint8> & AuxiliaryDataBuffer)

		Loads card firmware from resource files.

		:param FirmwareDataBuffer: byte array of the firmware program file. 
		:param FPGADataBuffer: byte array of the firmware FPGA file. 
		:param AuxiliaryDataBuffer: byte array of the binary auxiliary file. 


	.. cpp:function:: void LoadCorrectionFile(const CInputVector<LibMCDriver_ScanLab_uint8> & CorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension)

		Loads card calibration file from given resource file.

		:param CorrectionFileBuffer: binary data of the correction file. 
		:param nTableNumber: Correction table index of card (1..8) 
		:param nDimension: Is it a 2D or 3D correction file. 


	.. cpp:function:: void SelectCorrectionTable(const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB)

		Selects Correction Table on card.

		:param nTableNumberHeadA: Table Number for HeadA (1..8) or off (0). 
		:param nTableNumberHeadB: Table Number for HeadA (1..8) or off (0). 


	.. cpp:function:: void ConfigureLists(const LibMCDriver_ScanLab_uint32 nSizeListA, const LibMCDriver_ScanLab_uint32 nSizeListB)

		Configures list buffer size.

		:param nSizeListA: Size of List A 
		:param nSizeListB: Size of List B 


	.. cpp:function:: void SetLaserMode(const eLaserMode eLaserMode, const eLaserPort eLaserPort)

		Sets laser mode of card.

		:param eLaserMode: Laser Mode Enum 
		:param eLaserPort: Laser Port Enum 


	.. cpp:function:: void DisableAutoLaserControl()

		Disables automatic laser control.



	.. cpp:function:: void SetLaserControlParameters(const bool bDisableLaser, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive)

		Sets laser control parameters of card.

		:param bDisableLaser: Laser is disabled 
		:param bFinishLaserPulseAfterOn: Finish laser pulse after LaserOn 
		:param bPhaseShiftOfLaserSignal: 180 degree phase shift of Laser signal 
		:param bLaserOnSignalLowActive: Set Laser On Signal Low Active 
		:param bLaserHalfSignalsLowActive: Set Laser Half Signal Low Active 
		:param bSetDigitalInOneHighActive: Set Digital In 1 high Active 
		:param bOutputSynchronizationActive: Output synchronization active 


	.. cpp:function:: void SetLaserPulsesInBits(const LibMCDriver_ScanLab_uint32 nHalfPeriod, const LibMCDriver_ScanLab_uint32 nPulseLength)

		Sets laser control pulse interval (in 1/64th microseconds)

		:param nHalfPeriod: Half Output period in 1/64th microseconds 
		:param nPulseLength: Pulse Length in 1/64th microseconds 


	.. cpp:function:: void SetLaserPulsesInMicroSeconds(const LibMCDriver_ScanLab_double dHalfPeriod, const LibMCDriver_ScanLab_double dPulseLength)

		Sets laser control pulse interval (in microseconds)

		:param dHalfPeriod: Half Output period in microseconds 
		:param dPulseLength: Pulse Length in microseconds 


	.. cpp:function:: void SetStandbyInBits(const LibMCDriver_ScanLab_uint32 nHalfPeriod, const LibMCDriver_ScanLab_uint32 nPulseLength)

		Sets standby pulse interval (in 1/64th microseconds)

		:param nHalfPeriod: Half Output period in 1/64th microseconds 
		:param nPulseLength: Pulse Length in 1/64th microseconds 


	.. cpp:function:: void SetStandbyInMicroSeconds(const LibMCDriver_ScanLab_double dHalfPeriod, const LibMCDriver_ScanLab_double dPulseLength)

		Sets laser control pulse interval (in microseconds)

		:param dHalfPeriod: Half Output period in microseconds 
		:param dPulseLength: Pulse Length in microseconds 


	.. cpp:function:: LibMCDriver_ScanLab_uint32 GetSerialNumber()

		Returns serial number of card

		:returns: Returns serial number of board.


	.. cpp:function:: void SetStartList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition)

		Opens the list to write

		:param nListIndex: Index of List (1 or 2). 
		:param nPosition: Relative Position in List. 


	.. cpp:function:: void SetEndOfList()

		Closes the currently open list



	.. cpp:function:: void ExecuteList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition)

		Executes the list

		:param nListIndex: Index of List (1 or 2). 
		:param nPosition: Relative Position in List. 


	.. cpp:function:: void SetAutoChangePos(const LibMCDriver_ScanLab_uint32 nPosition)

		Sets the AutoChange Position of the currently not running list.

		:param nPosition: Relative Position in List. 


	.. cpp:function:: void SetDelays(const LibMCDriver_ScanLab_uint32 nMarkDelay, const LibMCDriver_ScanLab_uint32 nJumpDelay, const LibMCDriver_ScanLab_uint32 nPolygonDelay)

		Sets the laser delays

		:param nMarkDelay: Mark delay in microseconds (MUST be multiple of 10) 
		:param nJumpDelay: Jump delay in microseconds (MUST be multiple of 10) 
		:param nPolygonDelay: Polygon delay in microseconds (MUST be multiple of 10) 


	.. cpp:function:: void SetLaserDelaysInMicroseconds(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay)

		Sets the laser delays (on the list)

		:param dLaserOnDelay: LaserOn delay in microseconds 
		:param dLaserOffDelay: LaserOff delay in microseconds 


	.. cpp:function:: void SetLaserDelaysInBits(const LibMCDriver_ScanLab_uint32 nLaserOnDelay, const LibMCDriver_ScanLab_uint32 nLaserOffDelay)

		Sets the laser delays (on the list)

		:param nLaserOnDelay: LaserOn delay in bits (1/64th microseconds) 
		:param nLaserOffDelay: LaserOff delay in bits (1/64th microseconds) 


	.. cpp:function:: void DrawPolyline(const CInputVector<sPoint2D> & PointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue)

		Writes a polyline into the open list

		:param PointsBuffer: Points of polyline to draw. 
		:param fMarkSpeed: Mark speed in mm/s 
		:param fJumpSpeed: Mark speed in mm/s 
		:param fPower: Laser power in percent 
		:param fZValue: Focus Z Value 


	.. cpp:function:: void DrawHatches(const CInputVector<sHatch2D> & HatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue)

		Writes a list of hatches into the open list

		:param HatchesBuffer: Hatches to draw. 
		:param fMarkSpeed: Mark speed in mm/s 
		:param fJumpSpeed: Mark speed in mm/s 
		:param fPower: Laser power in percent 
		:param fZValue: Focus Z Value 


	.. cpp:function:: void AddCustomDelay(const LibMCDriver_ScanLab_uint32 nDelay)

		Adds a custom delay to the list

		:param nDelay: Custom delay value in microseconds (MUST be multiple of 10) 


	.. cpp:function:: LibMCDriver_ScanLab_double GetCorrectionFactor()

		Returns correction factor of Card Calibration (in bits per mm)

		:returns: CorrectionFactor


	.. cpp:function:: void GetStatus(bool & bBusy, LibMCDriver_ScanLab_uint32 & nPosition)

		Returns status of list execution

		:param bBusy: Execution is busy 
		:param nPosition: Returns current executed position 


	.. cpp:function:: void GetHeadStatus(const LibMCDriver_ScanLab_uint32 nHeadNo, bool & bPositionXisOK, bool & bPositionYisOK, bool & bTemperatureisOK, bool & bPowerisOK)

		Returns status of scan head

		:param nHeadNo: Head Number 
		:param bPositionXisOK: Position X is ok 
		:param bPositionYisOK: Position Y is ok 
		:param bTemperatureisOK: Temperature is ok 
		:param bPowerisOK: Power is ok 


	.. cpp:function:: void GetStateValues(bool & bLaserIsOn, LibMCDriver_ScanLab_int32 & nPositionX, LibMCDriver_ScanLab_int32 & nPositionY, LibMCDriver_ScanLab_int32 & nPositionZ, LibMCDriver_ScanLab_int32 & nCorrectedPositionX, LibMCDriver_ScanLab_int32 & nCorrectedPositionY, LibMCDriver_ScanLab_int32 & nCorrectedPositionZ, LibMCDriver_ScanLab_int32 & nFocusShift, LibMCDriver_ScanLab_int32 & nMarkSpeed)

		Returns status values of scan head

		:param bLaserIsOn: Laser is on 
		:param nPositionX: Current Position X in Units 
		:param nPositionY: Current Position Y in Units 
		:param nPositionZ: Current Position Z in Units 
		:param nCorrectedPositionX: Current Position X in Units 
		:param nCorrectedPositionY: Current Position Y in Units 
		:param nCorrectedPositionZ: Current Position Z in Units 
		:param nFocusShift: Current Focus Shift in Units 
		:param nMarkSpeed: Current Mark Speed in Units 


	.. cpp:function:: LibMCDriver_ScanLab_uint32 GetInputPointer()

		returns current input list position

		:returns: Returns current position of open list


	.. cpp:function:: void GetRTCVersion(LibMCDriver_ScanLab_uint32 & nRTCVersion, LibMCDriver_ScanLab_uint32 & nRTCType, LibMCDriver_ScanLab_uint32 & nDLLVersion, LibMCDriver_ScanLab_uint32 & nHEXVersion, LibMCDriver_ScanLab_uint32 & nBIOSVersion)

		Returns version information of the RTC Card

		:param nRTCVersion: RTC Card Version 
		:param nRTCType: RTC Card Type 
		:param nDLLVersion: RTC DLL Version 
		:param nHEXVersion: RTC HEX Version 
		:param nBIOSVersion: RTC BIOS Version 


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


.. cpp:type:: std::shared_ptr<CRTCContext> LibMCDriver_ScanLab::PRTCContext

	Shared pointer to CRTCContext to easily allow reference counting.

