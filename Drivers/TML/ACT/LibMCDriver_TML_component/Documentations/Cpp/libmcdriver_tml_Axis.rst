
CAxis
====================================================================================================


.. cpp:class:: LibMCDriver_TML::CAxis : public CBase 

	An axis of a TML Channel.




	.. cpp:function:: void MoveRelative(const LibMCDriver_TML_double dDistance, const LibMCDriver_TML_double dSpeed, const LibMCDriver_TML_double dAcceleration)

		Moves the selected drive a relative distance.

		:param dDistance: Distance (mm) 
		:param dSpeed: Speed (mm/s) 
		:param dAcceleration: Acceleration (mm/s^2) 


	.. cpp:function:: void MoveAbsolute(const LibMCDriver_TML_double dDistance, const LibMCDriver_TML_double dSpeed, const LibMCDriver_TML_double dAcceleration)

		Moves the selected drive to an absolute location.

		:param dDistance: Distance (mm) 
		:param dSpeed: Speed (mm/s) 
		:param dAcceleration: Acceleration (mm/s^2) 


	.. cpp:function:: void CallSubroutine(const std::string & sRoutine)

		Runs a subroutine on the selected drive.

		:param sRoutine: Label of routine 


	.. cpp:function:: LibMCDriver_TML_double GetPosition()

		Retrieves the current position of the drive.

		:returns: Position (mm)


	.. cpp:function:: LibMCDriver_TML_double GetSpeed()

		Retrieves the current speed of the drive.

		:returns: Speed (mm/s)


	.. cpp:function:: LibMCDriver_TML_int32 GetIntVariable(const std::string & sVariableName)

		Retrieves the current position of the drive.

		:param sVariableName: Variable name 
		:returns: Value


	.. cpp:function:: bool MotionComplete()

		Checks to see if the is currently moving.

		:returns: Boolean reflecting if the drive is in currently moving.


	.. cpp:function:: std::string GetIdentifier()

		Returns the axis identifier.

		:returns: Axis identifier.


	.. cpp:function:: std::string GetChannelIdentifier()

		Returns the identifier of the channel of the axis.

		:returns: Channel identifier.


	.. cpp:function:: void SetPower(const bool bEnable)

		Sets the power for an axis.

		:param bEnable: Flag if the power is enabled or not enabled. 


	.. cpp:function:: LibMCDriver_TML_uint32 ReadRegister(const LibMCDriver_TML_uint32 nRegister)

		Reads the selected register of an axis.

		:param nRegister: Selected register. 
		:returns: Value inside register.


	.. cpp:function:: bool CheckPower()

		Checks the power of the selected axis.

		:returns: True for power on.


.. cpp:type:: std::shared_ptr<CAxis> LibMCDriver_TML::PAxis

	Shared pointer to CAxis to easily allow reference counting.

