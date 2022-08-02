
CUIEnvironment
====================================================================================================


.. cpp:class:: LibMCEnv::CUIEnvironment : public CBase 

	Environment of the UI




	.. cpp:function:: void ActivateModalDialog(const std::string & sDialogName)

		activates a modal dialog on the client.

		:param sDialogName: Name of the dialog to activate. 


	.. cpp:function:: void CloseModalDialog()

		closes the active modal dialog on the client.



	.. cpp:function:: void ActivatePage(const std::string & sPageName)

		changes the current page on the client.

		:param sPageName: Name of the page to activate. 


	.. cpp:function:: void LogOut()

		Logs out the client session.



	.. cpp:function:: void ShowHint(const std::string & sHint, const LibMCEnv_uint32 nTimeoutInMS)

		Shows a hint message in the user interface.

		:param sHint: Hint to show. 
		:param nTimeoutInMS: How many milliseconds the snackbar should be shown. 


	.. cpp:function:: void ShowHintColored(const std::string & sHint, const LibMCEnv_uint32 nTimeoutInMS, const sColorRGB & Color, const sColorRGB & FontColor)

		Shows a hint message in the user interface in a certain color.

		:param sHint: Hint to show. 
		:param nTimeoutInMS: How many milliseconds the snackbar should be shown. 
		:param Color: Background color of hint. 
		:param FontColor: Font color of hint. 


	.. cpp:function:: void HideHint()

		Hides hint if any is displayed.



	.. cpp:function:: std::string ShowMessageDlg(const std::string & sCaption, const std::string & sTitle, const eMessageDialogType eDialogType, const std::string & sYesEvent, const std::string & sNoEvent, const std::string & sCancelEvent)

		Shows a message dialog in the user interface.

		:param sCaption: Caption of the dialog 
		:param sTitle: Title of the dialog 
		:param eDialogType: Which dialog type shall be shown. 
		:param sYesEvent: Event to be called when clicked yes or ok. 
		:param sNoEvent: Event to be called when clicked no. 
		:param sCancelEvent: Event to be called when dialog is closed or cancel is pressed. 
		:returns: Dialog UUID. Will be set as sender for triggered events.


	.. cpp:function:: std::string RetrieveEventSender()

		returns name of the UI control that triggered the event.

		:returns: Name of the sender element.


	.. cpp:function:: std::string RetrieveEventSenderUUID()

		returns uuid of the UI control that triggered the event.

		:returns: Name of the sender uuid.


	.. cpp:function:: PSignalTrigger PrepareSignal(const std::string & sMachineInstance, const std::string & sSignalName)

		prepares a signal object to trigger later.

		:param sMachineInstance: State machine instance name 
		:param sSignalName: Name Of signal channel. 
		:returns: Signal trigger object.


	.. cpp:function:: std::string GetMachineState(const std::string & sMachineInstance)

		Retrieves the machine state

		:param sMachineInstance: State machine instance name 
		:returns: Name of current state


	.. cpp:function:: void LogMessage(const std::string & sLogString)

		logs a string as message

		:param sLogString: String to Log 


	.. cpp:function:: void LogWarning(const std::string & sLogString)

		logs a string as warning

		:param sLogString: String to Log 


	.. cpp:function:: void LogInfo(const std::string & sLogString)

		logs a string as info

		:param sLogString: String to Log 


	.. cpp:function:: std::string GetMachineParameter(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)

		returns a string parameter of a state machine

		:param sMachineInstance: State machine instance name 
		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Current Parameter Value


	.. cpp:function:: std::string GetMachineParameterAsUUID(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)

		returns a uuid parameter of a state machine

		:param sMachineInstance: State machine instance name 
		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Current Parameter Value


	.. cpp:function:: LibMCEnv_double GetMachineParameterAsDouble(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)

		returns a double parameter of a state machine

		:param sMachineInstance: State machine instance name 
		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Current Parameter Value


	.. cpp:function:: LibMCEnv_int64 GetMachineParameterAsInteger(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)

		returns an int parameter of a state machine

		:param sMachineInstance: State machine instance name 
		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Current Parameter Value


	.. cpp:function:: bool GetMachineParameterAsBool(const std::string & sMachineInstance, const std::string & sParameterGroup, const std::string & sParameterName)

		returns a bool parameter of a state machine

		:param sMachineInstance: State machine instance name 
		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Current Parameter Value


	.. cpp:function:: std::string GetUIProperty(const std::string & sElementPath, const std::string & sPropertyName)

		returns a string property of a UI element on the client

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist. 
		:returns: Current property Value


	.. cpp:function:: std::string GetUIPropertyAsUUID(const std::string & sElementPath, const std::string & sPropertyName)

		returns a uuid variable of a UI element on the client

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist. 
		:returns: Current property Value


	.. cpp:function:: LibMCEnv_double GetUIPropertyAsDouble(const std::string & sElementPath, const std::string & sPropertyName)

		returns a double variable of a UI element on the client

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist. 
		:returns: Current property Value


	.. cpp:function:: LibMCEnv_int64 GetUIPropertyAsInteger(const std::string & sElementPath, const std::string & sPropertyName)

		returns a integer variable of a UI element on the client

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist. 
		:returns: Current property Value


	.. cpp:function:: bool GetUIPropertyAsBool(const std::string & sElementPath, const std::string & sPropertyName)

		returns a integer variable of a UI element on the client

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist. 
		:returns: Current property Value


	.. cpp:function:: void SetUIProperty(const std::string & sElementPath, const std::string & sPropertyName, const std::string & sValue)

		sets a string property of a UI element on the client.

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist or is readonly. 
		:param sValue: New property Value 


	.. cpp:function:: void SetUIPropertyAsUUID(const std::string & sElementPath, const std::string & sPropertyName, const std::string & sValue)

		sets a uuid property of a UI element on the client.

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist or is readonly. 
		:param sValue: New property Value 


	.. cpp:function:: void SetUIPropertyAsDouble(const std::string & sElementPath, const std::string & sPropertyName, const LibMCEnv_double dValue)

		sets a double property of a UI element on the client.

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist or is readonly. 
		:param dValue: New property Value 


	.. cpp:function:: void SetUIPropertyAsInteger(const std::string & sElementPath, const std::string & sPropertyName, const LibMCEnv_int64 nValue)

		sets a integer property of a UI element on the client.

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist or is readonly. 
		:param nValue: New property Value 


	.. cpp:function:: void SetUIPropertyAsBool(const std::string & sElementPath, const std::string & sPropertyName, const bool bValue)

		sets a bool property of a UI element on the client.

		:param sElementPath: Path of UI Element. Fails if element does not exist. 
		:param sPropertyName: Property name. Fails if property does not exist or is readonly. 
		:param bValue: New property Value 


	.. cpp:function:: PImageData CreateEmptyImage(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const eImagePixelFormat ePixelFormat)

		creates an empty image object.

		:param nPixelSizeX: Pixel size in X. MUST be positive. 
		:param nPixelSizeY: Pixel size in Y. MUST be positive. 
		:param dDPIValueX: DPI Value in X. MUST be positive. 
		:param dDPIValueY: DPI Value in Y. MUST be positive. 
		:param ePixelFormat: Pixel format to use. 
		:returns: Empty image instance.


	.. cpp:function:: PImageData LoadPNGImage(const CInputVector<LibMCEnv_uint8> & PNGDataBuffer, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const eImagePixelFormat ePixelFormat)

		creates an image object from a PNG data stream.

		:param PNGDataBuffer: DPI Value in X. MUST be positive. 
		:param dDPIValueX: DPI Value in X. MUST be positive. 
		:param dDPIValueY: DPI Value in Y. MUST be positive. 
		:param ePixelFormat: Pixel format to use. Might lose color and alpha information. 
		:returns: Image instance containing the PNG image.


	.. cpp:function:: LibMCEnv_uint64 GetGlobalTimerInMilliseconds()

		Returns the global timer in milliseconds.

		:returns: Timer value in Milliseconds


	.. cpp:function:: PTestEnvironment GetTestEnvironment()

		Returns a test environment instance.

		:returns: Test Environment Instance


.. cpp:type:: std::shared_ptr<CUIEnvironment> LibMCEnv::PUIEnvironment

	Shared pointer to CUIEnvironment to easily allow reference counting.

