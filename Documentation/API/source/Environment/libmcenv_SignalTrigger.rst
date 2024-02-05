
CSignalTrigger
====================================================================================================


.. cpp:class:: LibMCEnv::CSignalTrigger : public CBase 

	Class for signal triggering




	.. cpp:function:: bool CanTrigger()

		Returns, if signal channel is available.

		:returns: Returns true, if signal channel is available.


	.. cpp:function:: void Trigger()

		Triggers a signal, if signal channel is available.



	.. cpp:function:: bool WaitForHandling(const LibMCEnv_uint32 nTimeOut)

		Waits until the signal is reset.

		:param nTimeOut: Timeout in Milliseconds. 0 for Immediate return. 
		:returns: Flag if signal handling has been handled.


	.. cpp:function:: std::string GetName()

		Returns the signal name.

		:returns: Name Of Signal


	.. cpp:function:: std::string GetStateMachine()

		Returns the signal state machine instance.

		:returns: Name Of State Machine


	.. cpp:function:: void SetString(const std::string & sName, const std::string & sValue)

		sets a string value

		:param sName: Name 
		:param sValue: Value 


	.. cpp:function:: void SetUUID(const std::string & sName, const std::string & sValue)

		sets a uuid value

		:param sName: Name 
		:param sValue: Value 


	.. cpp:function:: void SetDouble(const std::string & sName, const LibMCEnv_double dValue)

		sets a double

		:param sName: Name 
		:param dValue: Value 


	.. cpp:function:: void SetInteger(const std::string & sName, const LibMCEnv_int64 nValue)

		sets an int.

		:param sName: Name 
		:param nValue: Value 


	.. cpp:function:: void SetBool(const std::string & sName, const bool bValue)

		sets a bool

		:param sName: Name 
		:param bValue: Value 


	.. cpp:function:: std::string GetStringResult(const std::string & sName)

		returns a string value of the result

		:param sName: Name 
		:returns: Value


	.. cpp:function:: std::string GetUUIDResult(const std::string & sName)

		returns a uuid value of the result

		:param sName: Name 
		:returns: Value


	.. cpp:function:: LibMCEnv_double GetDoubleResult(const std::string & sName)

		returns a string value of the result

		:param sName: Name 
		:returns: Value


	.. cpp:function:: LibMCEnv_int64 GetIntegerResult(const std::string & sName)

		returns an int value of the result.

		:param sName: Name 
		:returns: Value


	.. cpp:function:: bool GetBoolResult(const std::string & sName)

		returns a bool value of the result.

		:param sName: Name 
		:returns: Value


.. cpp:type:: std::shared_ptr<CSignalTrigger> LibMCEnv::PSignalTrigger

	Shared pointer to CSignalTrigger to easily allow reference counting.

