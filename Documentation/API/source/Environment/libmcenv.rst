
The wrapper class CWrapper
===================================================================================


.. cpp:class:: LibMCEnv::CWrapper

	All types of MC Environment Interface reside in the namespace LibMCEnv and all
	functionality of MC Environment Interface resides in LibMCEnv::CWrapper.

	A suitable way to use LibMCEnv::CWrapper is as a singleton.

	.. cpp:function:: void GetVersion(LibMCEnv_uint32 & nMajor, LibMCEnv_uint32 & nMinor, LibMCEnv_uint32 & nMicro)
	
		retrieves the binary version of this library.
	
		:param nMajor: returns the major version of this library 
		:param nMinor: returns the minor version of this library 
		:param nMicro: returns the micro version of this library 

	
	.. cpp:function:: bool GetLastError(classParam<CBase> pInstance, std::string & sErrorMessage)
	
		Returns the last error recorded on this object
	
		:param pInstance: Instance Handle 
		:param sErrorMessage: Message of the last error 
		:returns: Is there a last error to query

	
	.. cpp:function:: void ReleaseInstance(classParam<CBase> pInstance)
	
		Releases shared ownership of an Instance
	
		:param pInstance: Instance Handle 

	
	.. cpp:function:: void AcquireInstance(classParam<CBase> pInstance)
	
		Acquires shared ownership of an Instance
	
		:param pInstance: Instance Handle 

	
	.. cpp:function:: LibMCEnv_pvoid GetSymbolLookupMethod()
	
		Returns the address of the SymbolLookupMethod
	
		:returns: Address of the SymbolAddressMethod

	
.. cpp:type:: std::shared_ptr<CWrapper> LibMCEnv::PWrapper
	
