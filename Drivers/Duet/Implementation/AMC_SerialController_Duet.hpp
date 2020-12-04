
#ifndef __AMC_SERIALCONTROLLER_MARLIN
#define __AMC_SERIALCONTROLLER_MARLIN

#include "AMC_SerialController.hpp"
#include "serial/serial.h"

namespace AMC {



	class CSerialController_Duet : public CSerialController {

	private:
		std::unique_ptr<serial::Serial> m_pConnection;
		std::string m_sCOMPort;
		uint32_t m_nBaudRate;
		uint32_t m_nConnectTimeout;
		uint32_t m_nLineNumber;
		bool m_bIsConnected;
		double m_dStatusUpdateTimerInterval;

		bool m_bWaitForAck;
		bool m_bDebug;
		bool m_bIsHomed;
		bool m_bDoQueryFirmwareInfo;
		bool m_bDisableHoming;


		double m_dTargetPosX;
		double m_dTargetPosY;
		double m_dTargetPosZ;
		double m_dTargetPosA;
		double m_dTargetPosB;

		double m_dCurrentPosX;
		double m_dCurrentPosY;
		double m_dCurrentPosZ;
		double m_dCurrentPosA;
		double m_dCurrentPosB;

		double m_dCurrentSpeedInMMperSecond;

		std::string m_sAckSymbol;
		std::string m_sResendSymbol;

		int sendCommand(const std::string& sCommand);
		std::stringstream sendCommandBlocking(const std::string& sCommand);
		std::stringstream readResponseBlocking(const int nLineNumber);
		bool checkForAck(const int nLineNumber);
		uint32_t calculateLineChecksum (const std::string& sCommand);
		bool parseAckSymbol(const std::string& sLine, const uint32_t nLineNumber);
		void moveToEx(const bool bInX, const double dX, const bool bInY, const double dY, const bool bInZ, const double dZ, const bool bInA, const double dA, const bool bInB, const double dB, const double dLaserPower, const double dSpeedInMMperSecond);

	public:
		CSerialController_Duet();
		virtual ~CSerialController_Duet();

		void setStatusUpdateTimerInterval(const double dStatusUpdateTimerInterval);

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

		void setPositioningAbsolute();
		void setPositioningRelative();

		void queryPrinterState() override;

		void getTargetPosition(double& dX, double& dY, double& dZ, double& dA, double& dB) override;
		void getCurrentPosition(double& dX, double& dY, double& dZ, double& dA, double& dB) override;

		void startHoming() override;

		void moveXY(const double dX, const double dY, const double laserPercentage, const double dSpeedInMMperSecond) override;
		void moveFastXY(const double dX, const double dY, const double dSpeedInMMperSecond) override;

		void moveZ(const double dZ, const double dSpeedInMMperSecond) override;
		void moveA(const double dA, const double dSpeedInMMperSecond) override;
		void moveB(const double dB, const double dSpeedInMMperSecond) override;

		void setAxisPosition(const std::string& sAxis, double dValue) override;

		void powerOff() override;

		bool isHomed() override;
		bool isMoving() override;
		bool canReceiveMovement() override;
		void waitForMovement() override;

	};


}



#endif // __AMC_SERIALCONTROLLER_MARLIN

