
CRaylaseCard
====================================================================================================


.. cpp:class:: LibMCDriver_Raylase::CRaylaseCard : public CBase 

	Object to control a raylase-card.




	.. cpp:function:: bool IsConnected()

		Checks if the card is connected.

		:returns: Flag if the card is disconnected.


	.. cpp:function:: void Disconnect()

		Disconnects and unregisters the card.



	.. cpp:function:: void ResetToSystemDefaults()

		Resets the card settings to system defaults.



	.. cpp:function:: void LaserOn()

		Turns the laser on.



	.. cpp:function:: void LaserOff()

		Turns the laser off.



	.. cpp:function:: void ArmLaser(const bool bShallBeArmed)

		Arms or disarms the laser.

		:param bShallBeArmed: Shall the laser be armed or disarmed. 


	.. cpp:function:: bool IsLaserArmed()

		Checks if the laser is armed.

		:returns: Flag if the laser is armed.


	.. cpp:function:: void EnablePilot(const bool bShallBeEnabled)

		Enables the pilot pointer.

		:param bShallBeEnabled: Shall the pilot be enabled. 


	.. cpp:function:: bool PilotIsEnabled()

		Checks if the pilot is enabled.

		:returns: Flag if the pilot is enabled.


	.. cpp:function:: void GetLaserStatus(bool & bPilotIsEnabled, bool & bLaserIsArmed, bool & bLaserAlarm)

		Returns the laser status.

		:param bPilotIsEnabled: Flag if the pilot is enabled. 
		:param bLaserIsArmed: Flag if the laser is armed. 
		:param bLaserAlarm: Flag if the laser is in alarm state. 


	.. cpp:function:: void DrawLayer(const std::string & sStreamUUID, const LibMCDriver_Raylase_uint32 nLayerIndex)

		Draws a layer of a build stream. Blocks until the layer is drawn.

		:param sStreamUUID: UUID of the build stream. Must have been loaded in memory by the system. 
		:param nLayerIndex: Layer index of the build file. 


.. cpp:type:: std::shared_ptr<CRaylaseCard> LibMCDriver_Raylase::PRaylaseCard

	Shared pointer to CRaylaseCard to easily allow reference counting.

