
CDriver_ADS
====================================================================================================


.. cpp:class:: LibMCDriver_ADS::CDriver_ADS : public CDriver 

	A Driver for a BR Automation PLC.




	.. cpp:function:: void SetToSimulationMode()

		Turns the driver into a simulation mode.



	.. cpp:function:: bool IsSimulationMode()

		Returns if the driver is in simulation mode.

		:returns: Flag if driver is in simulation mode.


	.. cpp:function:: void Connect(const LibMCDriver_ADS_uint32 nPort, const LibMCDriver_ADS_uint32 nTimeout)

		Connects to a ADS PLC Controller.

		:param nPort: Port of PLC Service. 
		:param nTimeout: Timeout in milliseconds. 


	.. cpp:function:: void Disconnect()

		Disconnects from the ADS PLC Controller.



	.. cpp:function:: bool VariableExists(const std::string & sVariableName)

		Returns if a variable exists.

		:param sVariableName: Name of variable. 
		:returns: Flag if value exists.


	.. cpp:function:: LibMCDriver_ADS_int64 ReadIntegerValue(const std::string & sVariableName)

		Reads a value from an integer Variable.

		:param sVariableName: Name of variable. 
		:returns: Result value.


	.. cpp:function:: void WriteIntegerValue(const std::string & sVariableName, const LibMCDriver_ADS_int64 nValue)

		Reads a value from an integer Variable. Fails if value is not within the bounds of the variable.

		:param sVariableName: Name of variable. 
		:param nValue: Value to set. 


	.. cpp:function:: LibMCDriver_ADS_double ReadFloatValue(const std::string & sVariableName)

		Reads a value from an float Variable.

		:param sVariableName: Name of variable. 
		:returns: Result value.


	.. cpp:function:: void WriteFloatValue(const std::string & sVariableName, const LibMCDriver_ADS_double dValue)

		Reads a value from an integer Variable.

		:param sVariableName: Name of variable. 
		:param dValue: Value to set. 


	.. cpp:function:: bool ReadBoolValue(const std::string & sVariableName)

		Reads a value from an boolean Variable.

		:param sVariableName: Name of variable. 
		:returns: Result value.


	.. cpp:function:: void WriteBoolValue(const std::string & sVariableName, const bool bValue)

		Reads a value from an boolean Variable.

		:param sVariableName: Name of variable. 
		:param bValue: Value to set. 


	.. cpp:function:: std::string ReadStringValue(const std::string & sVariableName)

		Reads a value from an string Variable.

		:param sVariableName: Name of variable. 
		:returns: Result value.


	.. cpp:function:: void WriteStringValue(const std::string & sVariableName, const std::string & sValue)

		Reads a value from an string Variable.

		:param sVariableName: Name of variable. 
		:param sValue: Value to set. 


	.. cpp:function:: void GetVariableBounds(const std::string & sVariableName, LibMCDriver_ADS_int64 & nMinValue, LibMCDriver_ADS_int64 & nMaxValue)

		Returns the min and max value an integer variable can hold.

		:param sVariableName: Name of variable. Fails if variable does not exist or is not an integer value. 
		:param nMinValue: Minimum value. 
		:param nMaxValue: Minimum value. 


.. cpp:type:: std::shared_ptr<CDriver_ADS> LibMCDriver_ADS::PDriver_ADS

	Shared pointer to CDriver_ADS to easily allow reference counting.

