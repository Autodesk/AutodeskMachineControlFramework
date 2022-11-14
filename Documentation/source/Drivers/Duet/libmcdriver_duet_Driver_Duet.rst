
CDriver_Duet
====================================================================================================


.. cpp:class:: LibMCDriver_Duet::CDriver_Duet : public CDriver 

	A Driver for a Duet Reprap controller board.




	.. cpp:function:: void Connect(const std::string & sCOMPort, const LibMCDriver_Duet_uint32 nBaudrate, const LibMCDriver_Duet_double dStatusUpdateInterval, const LibMCDriver_Duet_uint32 nConnectTimeout)

		Creates and initializes a new Duet Connector.

		:param sCOMPort: Device Port to connect to 
		:param nBaudrate: Baudrate to use 
		:param dStatusUpdateInterval: Timer interval [ms] for updating status 
		:param nConnectTimeout: Timeout [ms] for connecting printer 


	.. cpp:function:: void Disconnect()

		Disconnects from the Marlin board.



	.. cpp:function:: void SetAbsolutePositioning(const bool bAbsolute)

		Sets the positioning to absolute values (default true).

		:param bAbsolute: If true, sets positioning to absolute, if false to relative 


	.. cpp:function:: void QueryParameters()

		Stores the driver parameters in the driver environment.



	.. cpp:function:: void GetTargetPosition(LibMCDriver_Duet_double & dX, LibMCDriver_Duet_double & dY, LibMCDriver_Duet_double & dZ, LibMCDriver_Duet_double & dA, LibMCDriver_Duet_double & dB)

		Returns the current target position.

		:param dX: X Value in mm 
		:param dY: Y Value in mm 
		:param dZ: Z Value in mm 
		:param dA: A Value in mm 
		:param dB: B Value in mm 


	.. cpp:function:: void GetCurrentPosition(LibMCDriver_Duet_double & dX, LibMCDriver_Duet_double & dY, LibMCDriver_Duet_double & dZ, LibMCDriver_Duet_double & dA, LibMCDriver_Duet_double & dB)

		Returns the current position.

		:param dX: X Value in mm 
		:param dY: Y Value in mm 
		:param dZ: Z Value in mm 
		:param dA: A Value in mm 
		:param dB: B Value in mm 


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

		Returns if the printer is connected

		:returns: True if printer is connected.


	.. cpp:function:: void MoveToXY(const LibMCDriver_Duet_double dX, const LibMCDriver_Duet_double dY, const LibMCDriver_Duet_double dLaserPower, const LibMCDriver_Duet_double dSpeed)

		Moves to/by a certain position by a linear move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dX: X Value in mm 
		:param dY: Y Value in mm 
		:param dLaserPower: Laser power in percent of maximum power 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void MoveFastToXY(const LibMCDriver_Duet_double dX, const LibMCDriver_Duet_double dY, const LibMCDriver_Duet_double dSpeed)

		Moves to/by a certain position by a fast move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dX: X Value in mm 
		:param dY: Y Value in mm 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void MoveToZ(const LibMCDriver_Duet_double dZ, const LibMCDriver_Duet_double dSpeed)

		Moves to/by a certain position by a linear move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dZ: Z Value in mm 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void MoveToA(const LibMCDriver_Duet_double dA, const LibMCDriver_Duet_double dSpeed)

		Moves to/by a certain position by a linear move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dA: A Value in mm 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void MoveToB(const LibMCDriver_Duet_double dB, const LibMCDriver_Duet_double dSpeed)

		Moves to/by a certain position by a linear move. Takes the relative/absolute mode into account. Fails if it cannot execute a movement.

		:param dB: B Value in mm 
		:param dSpeed: Movement speed in mm/s 


	.. cpp:function:: void StartHoming()

		Start Homing of printer.



	.. cpp:function:: void EmergencyStop()

		Used for emergency stopping. Shuts down the machine, turns off all the steppers and heaters, and if possible, turns off the power supply.



	.. cpp:function:: void SetAxisPosition(const std::string & sAxis, const LibMCDriver_Duet_double dValue)

		Set the current position of given axis to the specified value.

		:param sAxis: Axis whose value is to be set. 
		:param dValue: New value for given Axis. 


	.. cpp:function:: void PowerOff()

		Turn off the high-voltage power supply.



.. cpp:type:: std::shared_ptr<CDriver_Duet> LibMCDriver_Duet::PDriver_Duet

	Shared pointer to CDriver_Duet to easily allow reference counting.

