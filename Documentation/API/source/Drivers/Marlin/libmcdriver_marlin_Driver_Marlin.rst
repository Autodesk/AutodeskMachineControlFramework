
CDriver_Marlin
====================================================================================================


.. cpp:class:: LibMCDriver_Marlin::CDriver_Marlin : public CDriver 

	A Driver for a Marlin Reprap controller board.




	.. cpp:function:: void Connect(const std::string & sCOMPort, const LibMCDriver_Marlin_uint32 nBaudrate, const LibMCDriver_Marlin_uint32 nStatusUpdateInterval, const LibMCDriver_Marlin_uint32 nConnectTimeout)

		Creates and initializes a new Marlin Connector.

		:param sCOMPort: Device Port to connect to 
		:param nBaudrate: Baudrate to use 
		:param nStatusUpdateInterval: Timer interval [ms] for updating status 
		:param nConnectTimeout: Timeout [ms] for connecting printer 


	.. cpp:function:: void Disconnect()

		Disconnects from the Marlin board.



	.. cpp:function:: void SetAbsolutePositioning(const bool bAbsolute)

		Sets the positioning to absolute values (default true).

		:param bAbsolute: If true, sets positioning to absolute, if false to relative 


	.. cpp:function:: void SetHeatedBedTargetTemperature(const LibMCDriver_Marlin_double dTemperatureInDegreeCelcius, const bool bWaitForTemp)

		Sets heated bed target temperature.

		:param dTemperatureInDegreeCelcius: Bed target temperature. 
		:param bWaitForTemp: If true, waits for the target bed temperature to be reached before proceeding 


	.. cpp:function:: void SetExtruderTargetTemperature(const LibMCDriver_Marlin_uint32 nExtruderID, const LibMCDriver_Marlin_double dTemperatureInDegreeCelcius, const bool bWaitForTemp)

		Sets target temperature of the given extruder.

		:param nExtruderID: ID of extruder. 
		:param dTemperatureInDegreeCelcius: Extruder target temperature. 
		:param bWaitForTemp: If true, waits for the target extruder temperature to be reached before proceeding 


	.. cpp:function:: void SetFanSpeed(const LibMCDriver_Marlin_uint32 nFanID, const LibMCDriver_Marlin_uint32 nSpeed)

		Turns on one of the fans and set its speed.

		:param nFanID: ID of fan. 
		:param nSpeed: Fan speed [0..255]. 0=0%...255=100% 


	.. cpp:function:: void SetPidParameters(const LibMCDriver_Marlin_double dP, const LibMCDriver_Marlin_double dI, const LibMCDriver_Marlin_double dD)

		Sets PID parameters.

		:param dP: New value for P parameter. 
		:param dI: New value for I parameter. 
		:param dD: New value for D parameter. 


	.. cpp:function:: void UpdatePositionState()

		Polls a new state from the printer.



	.. cpp:function:: void UpdateTemperatureState(const LibMCDriver_Marlin_uint32 nExtruderID)

		Polls a new temperature state from the printer.

		:param nExtruderID: ID of extruder. 


	.. cpp:function:: void GetCurrentPosition(LibMCDriver_Marlin_double & dX, LibMCDriver_Marlin_double & dY, LibMCDriver_Marlin_double & dZ)

		Returns the current axis position.

		:param dX: X Value in mm 
		:param dY: Y Value in mm 
		:param dZ: Z Value in mm 


	.. cpp:function:: void GetTargetPosition(LibMCDriver_Marlin_double & dX, LibMCDriver_Marlin_double & dY, LibMCDriver_Marlin_double & dZ)

		Returns the current target position.

		:param dX: X Value in mm 
		:param dY: Y Value in mm 
		:param dZ: Z Value in mm 


	.. cpp:function:: void GetExtruderTargetPosition(LibMCDriver_Marlin_double & dE)

		Returns the target extruder position.

		:param dE: E Value in mm 


	.. cpp:function:: void GetHeatedBedTargetTemperature(LibMCDriver_Marlin_double & dTargetTemperature)

		Returns the the target bed temperature.

		:param dTargetTemperature: Target Temperature in degree celsius. 


	.. cpp:function:: void GetHeatedBedCurrentTemperature(LibMCDriver_Marlin_double & dCurrentTemperature)

		Returns the current bed temperature.

		:param dCurrentTemperature: Current Temperature in degree celsius. 


	.. cpp:function:: void GetExtruderCurrentTemperature(const LibMCDriver_Marlin_uint32 nExtruderID, LibMCDriver_Marlin_double & dCurrentTemperature)

		Returns the current temperature of an extruder.

		:param nExtruderID: ID of Extruder 
		:param dCurrentTemperature: Current Temperature in degree celsius. 


	.. cpp:function:: void GetExtruderTargetTemperature(const LibMCDriver_Marlin_uint32 nExtruderID, LibMCDriver_Marlin_double & dTargetTemperature)

		Returns the target temperature of an extruder.

		:param nExtruderID: ID of Extruder 
		:param dTargetTemperature: Target Temperature in degree celsius. 


	.. cpp:function:: void GetPidParameters(LibMCDriver_Marlin_double & dP, LibMCDriver_Marlin_double & dI, LibMCDriver_Marlin_double & dD)

		Returns the current PID values.

		:param dP: Current P value. 
		:param dI: Current I value. 
		:param dD: Current D value. 


	.. cpp:function:: bool CanExecuteMovement()

		Returns if the movement buffer can receive another movement command..

		:returns: True if a MoveTo or MoveFastTo command can be triggered.


	.. cpp:function:: bool IsMoving()

		Returns if the extruder is moving..

		:returns: Extruder is moving


	.. cpp:function:: bool IsHomed()

		Returns if the printer is homed

		:returns: True if printer is homed.


	.. cpp:function:: bool IsConnected()

		Returns if the printer is coneccted

		:returns: True if printer is connected.


	.. cpp:function:: void MoveToXY(const LibMCDriver_Marlin_double dX, const LibMCDriver_Marlin_double dY, const LibMCDriver_Marlin_double dE, const LibMCDriver_Marlin_double dSpeed)

		Moves to/by a certain position by a linear move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dX: X Value in mm 
		:param dY: Y Value in mm 
		:param dE: E Value in mm 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void MoveFastToXY(const LibMCDriver_Marlin_double dX, const LibMCDriver_Marlin_double dY, const LibMCDriver_Marlin_double dSpeed)

		Moves to/by a certain position by a fast move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dX: X Value in mm 
		:param dY: Y Value in mm 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void MoveToZ(const LibMCDriver_Marlin_double dZ, const LibMCDriver_Marlin_double dE, const LibMCDriver_Marlin_double dSpeed)

		Moves to/by a certain position by a linear move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dZ: Z Value in mm 
		:param dE: E Value in mm 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void MoveFastToZ(const LibMCDriver_Marlin_double dZ, const LibMCDriver_Marlin_double dSpeed)

		Moves to/by a certain position by a fast move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dZ: Z Value in mm 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void StartHoming()

		Start Homing of printer.



	.. cpp:function:: void EmergencyStop()

		Used for emergency stopping. Shuts down the machine, turns off all the steppers and heaters, and if possible, turns off the power supply.



	.. cpp:function:: void SetAxisPosition(const std::string & sAxis, const LibMCDriver_Marlin_double dValue)

		Set the current position of given axis to the specified value.

		:param sAxis: Axis whose value is to be set. 
		:param dValue: New value for given Axis. 


	.. cpp:function:: void ExtruderDoExtrude(const LibMCDriver_Marlin_double dE, const LibMCDriver_Marlin_double dSpeed)

		Extrudes the specified value with given Feedrate.

		:param dE: E value in mm 
		:param dSpeed: Extrusion speed in mm/s 


	.. cpp:function:: void SetAbsoluteExtrusion(const bool bAbsolute)

		Sets the extrusion (E axis) to absolute mode.

		:param bAbsolute: If true, sets mode to absolute, if false to relative 


	.. cpp:function:: void StopIdleHold()

		Stop the idle hold on all axis and extruder.



	.. cpp:function:: void PowerOff()

		Turn off the high-voltage power supply.



.. cpp:type:: std::shared_ptr<CDriver_Marlin> LibMCDriver_Marlin::PDriver_Marlin

	Shared pointer to CDriver_Marlin to easily allow reference counting.

