
CDriver_Ximc
====================================================================================================


.. cpp:class:: LibMCDriver_Ximc::CDriver_Ximc : public CDriver 

	A Driver for a Ximc controller.




	.. cpp:function:: LibMCDriver_Ximc_uint32 GetDetectedDeviceCount()

		Returns how many devices have been detected.

		:returns: Number of Devices.


	.. cpp:function:: std::string GetDetectedDeviceName(const LibMCDriver_Ximc_uint32 nDeviceIndex)

		Returns the name of a detected device.

		:param nDeviceIndex: Index of Device. 
		:returns: Name of Device.


	.. cpp:function:: void Initialize(const std::string & sDeviceName)

		Initializes the Ximc controller.

		:param sDeviceName: Name of Device. 


	.. cpp:function:: LibMCDriver_Ximc_double GetCurrentPosition()

		Returns the current position of the controller.

		:returns: Position of Axis.


.. cpp:type:: std::shared_ptr<CDriver_Ximc> LibMCDriver_Ximc::PDriver_Ximc

	Shared pointer to CDriver_Ximc to easily allow reference counting.

