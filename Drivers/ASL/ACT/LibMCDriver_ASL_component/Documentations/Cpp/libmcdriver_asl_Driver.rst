
CDriver
====================================================================================================


.. cpp:class:: LibMCDriver_ASL::CDriver : public CBase 

	An abstract Machine Control Driver




	.. cpp:function:: void Configure(const std::string & sConfigurationString)

		Configures a driver with its specific configuration data.

		:param sConfigurationString: Configuration data of driver. 


	.. cpp:function:: std::string GetName()

		returns the name identifier of the driver

		:returns: Name of the driver.


	.. cpp:function:: std::string GetType()

		returns the type identifier of the driver

		:returns: Type of the driver.


	.. cpp:function:: void GetVersion(LibMCDriver_ASL_uint32 & nMajor, LibMCDriver_ASL_uint32 & nMinor, LibMCDriver_ASL_uint32 & nMicro, std::string & sBuild)

		returns the version identifiers of the driver

		:param nMajor: Major version. 
		:param nMinor: Minor version. 
		:param nMicro: Micro version. 
		:param sBuild: Build identifier. 


	.. cpp:function:: void QueryParameters()

		Updates the driver parameters in the driver environment. Should only be called in the driver thread.



	.. cpp:function:: void QueryParametersEx(classParam<LibMCEnv::CDriverStatusUpdateSession> pDriverUpdateInstance)

		Updates the driver parameters in the driver environment. Might be called out of thread. Implementation MUST be able to handle parallel calls.

		:param pDriverUpdateInstance: Status update instance. 


.. cpp:type:: std::shared_ptr<CDriver> LibMCDriver_ASL::PDriver

	Shared pointer to CDriver to easily allow reference counting.

