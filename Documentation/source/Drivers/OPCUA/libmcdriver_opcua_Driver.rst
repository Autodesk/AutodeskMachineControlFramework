
CDriver
====================================================================================================


.. cpp:class:: LibMCDriver_OPCUA::CDriver : public CBase 

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


	.. cpp:function:: void GetVersion(LibMCDriver_OPCUA_uint32 & nMajor, LibMCDriver_OPCUA_uint32 & nMinor, LibMCDriver_OPCUA_uint32 & nMicro, std::string & sBuild)

		returns the version identifiers of the driver

		:param nMajor: Major version. 
		:param nMinor: Minor version. 
		:param nMicro: Micro version. 
		:param sBuild: Build identifier. 


	.. cpp:function:: void GetHeaderInformation(std::string & sNameSpace, std::string & sBaseName)

		returns the header information

		:param sNameSpace: NameSpace of the driver. 
		:param sBaseName: BaseName of the driver. 


	.. cpp:function:: void QueryParameters()

		Stores the driver parameters in the driver environment.



.. cpp:type:: std::shared_ptr<CDriver> LibMCDriver_OPCUA::PDriver

	Shared pointer to CDriver to easily allow reference counting.

