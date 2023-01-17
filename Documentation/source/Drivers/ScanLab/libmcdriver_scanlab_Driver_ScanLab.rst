
CDriver_ScanLab
====================================================================================================


.. cpp:class:: LibMCDriver_ScanLab::CDriver_ScanLab : public CDriver 

	A LaserScanner Driver for a ScanLAB card.




	.. cpp:function:: void LoadSDK(const std::string & sResourceName)

		Initializes the ScanLab SDK.

		:param sResourceName: Resource name of Scanlab DLL 


	.. cpp:function:: void LoadCustomSDK(const CInputVector<LibMCDriver_ScanLab_uint8> & ScanlabDLLBuffer)

		Initializes the ScanLab SDK from an external source.

		:param ScanlabDLLBuffer: Byte array of Scanlab DLL 


	.. cpp:function:: PRTCSelector CreateRTCSelector()

		Creates and initializes a new RTC selector singleton. Should only be called once per Process.

		:returns: New Selector instance


.. cpp:type:: std::shared_ptr<CDriver_ScanLab> LibMCDriver_ScanLab::PDriver_ScanLab

	Shared pointer to CDriver_ScanLab to easily allow reference counting.

