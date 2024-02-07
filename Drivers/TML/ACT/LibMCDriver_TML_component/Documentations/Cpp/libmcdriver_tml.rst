
The wrapper class CWrapper
===================================================================================


.. cpp:class:: LibMCDriver_TML::CWrapper

	All types of MC Driver Technosoft Motion Library reside in the namespace LibMCDriver_TML and all
	functionality of MC Driver Technosoft Motion Library resides in LibMCDriver_TML::CWrapper.

	A suitable way to use LibMCDriver_TML::CWrapper is as a singleton.

	.. cpp:function:: void GetVersion(LibMCDriver_TML_uint32 & nMajor, LibMCDriver_TML_uint32 & nMinor, LibMCDriver_TML_uint32 & nMicro)
	
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

	
	.. cpp:function:: void InjectComponent(const std::string & sNameSpace, const LibMCDriver_TML_pvoid pSymbolAddressMethod)
	
		Injects an imported component for usage within this component
	
		:param sNameSpace: NameSpace of the injected component 
		:param pSymbolAddressMethod: Address of the SymbolAddressMethod of the injected component 

	
	.. cpp:function:: LibMCDriver_TML_pvoid GetSymbolLookupMethod()
	
		Returns the address of the SymbolLookupMethod
	
		:returns: Address of the SymbolAddressMethod

	
	.. cpp:function:: PDriver CreateDriver(const std::string & sName, const std::string & sType, classParam<LibMCEnv::CDriverEnvironment> pDriverEnvironment)
	
		Creates a driver instance with a specific name.
	
		:param sName: Name of driver to be created. 
		:param sType: Type of driver to be created. 
		:param pDriverEnvironment: Environment of this driver. 
		:returns: New Driver instance

	
.. cpp:type:: std::shared_ptr<CWrapper> LibMCDriver_TML::PWrapper
	
