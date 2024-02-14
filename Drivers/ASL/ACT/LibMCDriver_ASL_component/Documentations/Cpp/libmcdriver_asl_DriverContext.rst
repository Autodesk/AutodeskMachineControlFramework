
CDriverContext
====================================================================================================


.. cpp:class:: LibMCDriver_ASL::CDriverContext : public CBase 

	An abstract Machine Control Driver




	.. cpp:function:: std::string GetSerialNumber()

		Returns the serial number of board

		:returns: Type of the driver.


	.. cpp:function:: void SetPower(const bool bPower)

		Set the board power.

		:param bPower: Power on/off. 


	.. cpp:function:: void SetPrintheadMode(const eBoardMode eMode)

		set the mode of the board for printing

		:param eMode: Type of the driver. 


	.. cpp:function:: void SetFrequency(const LibMCDriver_ASL_uint32 nFrequency)

		Set the frequency of the board (only supported in certain modes).

		:param nFrequency: Frequency in Hz 


	.. cpp:function:: void SetTemperature(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_double dTemperature)

		Set the temperature of a specific head.

		:param nIndex: Head index 
		:param dTemperature: Temperature to set 


	.. cpp:function:: void SetPrintStart(const LibMCDriver_ASL_uint32 nStartLocation)

		Set the print start location.

		:param nStartLocation: The start location of the print. 


.. cpp:type:: std::shared_ptr<CDriverContext> LibMCDriver_ASL::PDriverContext

	Shared pointer to CDriverContext to easily allow reference counting.

