
#ifndef __AMC_SERIALCONTROLLER_MARLIN
#define __AMC_SERIALCONTROLLER_MARLIN

#include "AMC_SerialController.hpp"
#include "serial/serial.h"

namespace AMC {



	class CSerialController_Marlin : public CSerialController {

	private:
		// TODO log ccord to file just for testing => remove later		
		std::stringstream m_sPosTempLogStream;

		std::unique_ptr<serial::Serial> m_pConnection;
		std::string m_sCOMPort;
		uint32_t m_nBaudRate;
		uint32_t m_nLineNumber;
		double m_dStatusUpdateTimerInterval;

		bool m_bDebug;
		bool m_bIsHomed;

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
		double m_dCurrentPosE;
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
		void queryAxisStepsPerUnitStateAndPidValues() override;
		void queryFirmwareInfo() override;
		// TODO nur temp => wieder raus
		void savePosTempLog() override;
		uint32_t m_nLayer;

	public:
		CSerialController_Marlin(bool bDebug);
		virtual ~CSerialController_Marlin();

		void setStatusUpdateTimerInterval(const double dStatusUpdateTimerInterval);

		void setCOMPort(const std::string & sCOMPort);
		std::string getCOMPort();

		void setBaudrate(const uint32_t nBaudrate);
		uint32_t getBaudrate();

		void initializeController() override;
		void resetController() override;
		void disconnectController() override;

		void setPositioningAbolute();
		void setPositioningRelative();

		void setHeatedBedTargetTemperature(double nTemperatureInDegreeCelcius) override;
		void setExtruderTargetTemperature(uint32_t nExtruderIndex, double nTemperatureInDegreeCelcius) override;
		void setPidParameters(double dP, double dI, double dD) override;

		void queryTemperatureState(uint32_t nExtruderIndex) override;
		void queryPositionState() override;

		void getHeatedBedTemperature(double& dTargetTemperature, double& dCurrentTemperature) override;
		void getExtruderTemperature(uint32_t nExtruderIndex, double& dTargetTemperature, double& dCurrentTemperature) override;

		void getTargetPosition(double& dX, double& dY, double& dZ) override;
		void getCurrentPosition(double& dX, double& dY, double& dZ) override;
		void getExtruderPosition(double& dE) override;
		void getPidParameters(double& dP, double& dI, double& dD) override;

		void startHoming() override;
		void setLcdMsg(const std::string& sLcdMsg) override;

		void move(const double dX, const double dY, const double dZ, const double dSpeedInMMperSecond) override;
		void moveFast(const double dX, const double dY, const double dZ, const double dSpeedInMMperSecond) override;

		bool isHomed() override;
		bool isMoving() override;
		bool canReceiveMovement() override;
		void waitForMovement() override;

	};


}



#endif // __AMC_SERIALCONTROLLER_MARLIN

