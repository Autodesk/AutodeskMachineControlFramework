
CSignalHandler
====================================================================================================


.. cpp:class:: LibMCEnv::CSignalHandler : public CBase 

	Class for signal handling




	.. cpp:function:: void SignalHandled()

		Marks signal as handled and resets signal channel.



	.. cpp:function:: std::string GetName()

		Returns the signal name.

		:returns: Name Of Signal


	.. cpp:function:: std::string GetSignalID()

		Returns the signal id.

		:returns: Signal Identifier


	.. cpp:function:: std::string GetStateMachine()

		Returns the signal state machine instance.

		:returns: Name Of State Machine


	.. cpp:function:: std::string GetString(const std::string & sName)

		gets a string value

		:param sName: Name 
		:returns: Value


	.. cpp:function:: std::string GetUUID(const std::string & sName)

		gets a uuid value

		:param sName: Name 
		:returns: Value


	.. cpp:function:: LibMCEnv_double GetDouble(const std::string & sName)

		gets a double

		:param sName: Name 
		:returns: Value


	.. cpp:function:: LibMCEnv_int64 GetInteger(const std::string & sName)

		gets an int.

		:param sName: Name 
		:returns: Value


	.. cpp:function:: bool GetBool(const std::string & sName)

		gets a bool

		:param sName: Name 
		:returns: Value


	.. cpp:function:: void SetStringResult(const std::string & sName, const std::string & sValue)

		returns a string value of the result

		:param sName: Name 
		:param sValue: Value 


	.. cpp:function:: void SetUUIDResult(const std::string & sName, const std::string & sValue)

		returns a uuid value of the result

		:param sName: Name 
		:param sValue: Value 


	.. cpp:function:: void SetDoubleResult(const std::string & sName, const LibMCEnv_double dValue)

		returns a string value of the result

		:param sName: Name 
		:param dValue: Value 


	.. cpp:function:: void SetIntegerResult(const std::string & sName, const LibMCEnv_int64 nValue)

		sets an int.

		:param sName: Name 
		:param nValue: Value 


	.. cpp:function:: void SetBoolResult(const std::string & sName, const bool bValue)

		sets a bool

		:param sName: Name 
		:param bValue: Value 


.. cpp:type:: std::shared_ptr<CSignalHandler> LibMCEnv::PSignalHandler

	Shared pointer to CSignalHandler to easily allow reference counting.

