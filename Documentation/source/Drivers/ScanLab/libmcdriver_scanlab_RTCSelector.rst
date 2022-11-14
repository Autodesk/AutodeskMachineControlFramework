
CRTCSelector
====================================================================================================


.. cpp:class:: LibMCDriver_ScanLab::CRTCSelector : public CBase 

	Selector of the RTC Controller




	.. cpp:function:: LibMCDriver_ScanLab_uint32 SearchCards(const std::string & sIP, const std::string & sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout)

		Searches Ethernet Cards

		:param sIP: IP Network Address. 
		:param sNetmask: IP Netmask Address. 
		:param nTimeout: Time out in microseconds. 
		:returns: Returns how many ethernet cards have been found.


	.. cpp:function:: LibMCDriver_ScanLab_uint32 SearchCardsByRange(const std::string & sStartIP, const std::string & sEndIP, const LibMCDriver_ScanLab_uint32 nTimeout)

		Searches Ethernet Cards

		:param sStartIP: IP Network Address. 
		:param sEndIP: IP Network Address. 
		:param nTimeout: Time out in microseconds. 
		:returns: Returns how many ethernet cards have been found.


	.. cpp:function:: LibMCDriver_ScanLab_uint32 GetCardCount()

		Returns number of detected Cards

		:returns: Returns how many cards have been found.


	.. cpp:function:: LibMCDriver_ScanLab_uint32 GetEthernetCardCount()

		Returns number of found ethernet cards

		:returns: Returns how many ethernet cards have been found.


	.. cpp:function:: PRTCContext AcquireCard(const LibMCDriver_ScanLab_uint32 nNumber)

		Acquires a card and returns an RTCContext instance.

		:param nNumber: Number of Card (1-based). Must be between 1 and CardCount. 
		:returns: New Context# instance


	.. cpp:function:: PRTCContext AcquireCardBySerial(const LibMCDriver_ScanLab_uint32 nSerialNumber)

		Acquires a card and returns an RTCContext instance.

		:param nSerialNumber: Desired Serial Number of card. 
		:returns: New Context# instance


	.. cpp:function:: PRTCContext AcquireEthernetCard(const LibMCDriver_ScanLab_uint32 nNumber)

		Acquires an ethernet card and returns an RTCContext instance.

		:param nNumber: Number of Card (1-based). Must be between 1 and EthernetCardCount. 
		:returns: New Context# instance


	.. cpp:function:: PRTCContext AcquireEthernetCardBySerial(const LibMCDriver_ScanLab_uint32 nSerialNumber)

		Acquires an ethernet card and returns an RTCContext instance.

		:param nSerialNumber: Desired Serial Number of card. 
		:returns: New Context# instance


.. cpp:type:: std::shared_ptr<CRTCSelector> LibMCDriver_ScanLab::PRTCSelector

	Shared pointer to CRTCSelector to easily allow reference counting.

