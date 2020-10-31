
#ifndef __AMC_SERIALCONTROLLER_MARLIN
#define __AMC_SERIALCONTROLLER_MARLIN

#include "AMC_SerialController.hpp"
#include "serial/serial.h"

namespace AMC {



	class CSerialController_Marlin : public CSerialController {

	private:
		std::unique_ptr<serial::Serial> m_pConnection;
		std::string m_sCOMPort;
		uint32_t m_nBaudRate;
		uint32_t m_nConnectTimeout;
		uint32_t m_nLineNumber;
		bool m_bIsConnected;
		uint32_t m_nStatusUpdateTimerInterval;

		bool m_bDebug;
		bool m_bIsHomed;
		bool m_bDoQueryFirmwareInfo;
		bool m_bDisableHoming;

		double m_dAxisStepsPerUnitX;
		double m_dAxisStepsPerUnitY;
		double m_dAxisStepsPerUnitZ;
		double m_dAxisStepsPerUnitE;

		double m_dPidValueP;
		double m_dPidValueI;
		double m_dPidValueD;

		double m_dTargetPosX;
		double m_dTargetPosY;
		double m_dTargetPosZ;
		double m_dTargetPosE;

		double m_dCurrentPosX;
		double m_dCurrentPosY;
		double m_dCurrentPosZ;
		double m_dCurrentSpeedInMMperSecond;

		// firmware info
		std::string m_sFirmwareName;
		std::string m_sSourceCodeUrl;
		std::string m_sProtocolVersion;
		std::string m_sMachineType;
		uint32_t m_iExtruderCount;
		std::string m_sUUID;


		std::vector<double> m_dCurrentExtruderTemp;
		std::vector<double> m_dTargetExtruderTemp;
		double m_dCurrentBedTemp;
		double m_dTargetBedTemp;

		std::string m_sAckSymbol;
		std::string m_sResendSymbol;

		uint32_t m_nMinBufferSpace;
		uint32_t m_nMaxBufferSpace;
		uint32_t m_nCurrentBufferSpace;


		std::stringstream sendCommand(const std::string& sCommand);
		uint32_t calculateLineChecksum (const std::string& sCommand);
		void checkIsHomed();
		bool parseAckSymbol(const std::string& sLine, const uint32_t nLineNumber);
		void queryAxisStepsPerUnitStateAndPidValues();
		void queryFirmwareInfo();
		void moveToEx (bool bFastMove, bool bInX, const double dX, bool bInY, const double dY, bool bInZ, const double dZ, bool bInE, const double dE, const double dSpeedInMMperSecond);

	public:
		CSerialController_Marlin(bool bDebug, bool bDoQueryFirmwareInfo, bool bDisableHoming);
		virtual ~CSerialController_Marlin();

		void setStatusUpdateTimerInterval(const uint32_t dStatusUpdateTimerInterval);

		void setConnectTimeout(const uint32_t nConnectTimeout);
		uint32_t getConnectTimeout();

		void setCOMPort(const std::string& sCOMPort);
		std::string getCOMPort();

		void setBaudrate(const uint32_t nBaudrate);
		uint32_t getBaudrate();

		bool isConnected();

		void initializeController() override;
		void resetController() override;
		void disconnectController() override;
		void emergencyStop() override;

		void setPositioningAbolute();
		void setPositioningRelative();

		void setHeatedBedTargetTemperature(double nTemperatureInDegreeCelcius, bool bWait) override;
		void setExtruderTargetTemperature(uint32_t nExtruderIndex, double nTemperatureInDegreeCelcius, bool bWait) override;
		void setPidParameters(double dP, double dI, double dD) override;
		void setFanSpeed(uint32_t nFanIndex, uint32_t nSpeed) override;

		void queryTemperatureState(uint32_t nExtruderIndex) override;
		void queryPositionState() override;

		void getHeatedBedCurrentTemperature(double& dCurrentTemperature) override;
		void getHeatedBedTargetTemperature(double& dTargetTemperature) override;
		void getExtruderCurrentTemperature(uint32_t nExtruderIndex, double& dCurrentTemperature) override;
		void getExtruderTargetTemperature(uint32_t nExtruderIndex, double& dTargetTemperature) override;

		void getTargetPosition(double& dX, double& dY, double& dZ) override;
		void getCurrentPosition(double& dX, double& dY, double& dZ) override;
		void getExtruderTargetPosition(double& dE) override;
		void getPidParameters(double& dP, double& dI, double& dD) override;

		void startHoming() override;
		void setLcdMsg(const std::string& sLcdMsg) override;

		void moveXY(const double dX, const double dY, const double dE, const double dSpeedInMMperSecond) override;
		void moveFastXY(const double dX, const double dY, const double dSpeedInMMperSecond) override;
		void moveZ(const double dZ, const double dE, const double dSpeedInMMperSecond) override;
		void moveFastZ(const double dZ, const double dSpeedInMMperSecond) override;

		void setAxisPosition(const std::string& sAxis, double dValue) override;
		void extruderDoExtrude(double dE, double dSpeedInMMperSecond) override;
		void setAbsoluteExtrusion(bool bAbsolute) override;

		void stopIdleHold() override;
		void powerOff() override;
		
		bool isHomed() override;
		bool isMoving() override;
		bool canReceiveMovement() override;
		void waitForMovement() override;

	};


}



#endif // __AMC_SERIALCONTROLLER_MARLIN

