
CPLCCommand
====================================================================================================


.. cpp:class:: LibMCDriver_S7Net::CPLCCommand : public CBase 

	




	.. cpp:function:: void SetIntegerParameter(const std::string & sParameterName, const LibMCDriver_S7Net_int32 nValue)

		Sets an integer parameter of the command

		:param sParameterName: Parameter Value 
		:param nValue: Parameter Value 


	.. cpp:function:: void SetStringParameter(const std::string & sParameterName, const std::string & sValue)

		Sets a string parameter of the command

		:param sParameterName: Parameter Value 
		:param sValue: Parameter Value 


	.. cpp:function:: void SetBoolParameter(const std::string & sParameterName, const bool bValue)

		Sets a bool parameter of the command

		:param sParameterName: Parameter Value 
		:param bValue: Parameter Value 


	.. cpp:function:: void SetDoubleParameter(const std::string & sParameterName, const LibMCDriver_S7Net_double dValue)

		Sets a double parameter of the command

		:param sParameterName: Parameter Value 
		:param dValue: Parameter Value 


.. cpp:type:: std::shared_ptr<CPLCCommand> LibMCDriver_S7Net::PPLCCommand

	Shared pointer to CPLCCommand to easily allow reference counting.

