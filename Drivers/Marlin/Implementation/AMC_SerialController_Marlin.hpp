
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
		uint32_t m_nLineNumber;

		bool m_bDebug;

		double m_dTargetPosX;
		double m_dTargetPosY;
		double m_dTargetPosZ;
		double m_dTargetPosE;

		double m_dCurrentPosX;
		double m_dCurrentPosY;
		double m_dCurrentPosZ;
		double m_dCurrentPosE;

		std::string m_sAckSymbol;
		std::string m_sResendSymbol;

		uint32_t m_nMinBufferSpace;
		uint32_t m_nMaxBufferSpace;
		uint32_t m_nCurrentBufferSpace;

		std::stringstream sendCommand(const std::string& sCommand);
		uint32_t calculateLineChecksum (const std::string& sCommand);
		bool parseAckSymbol(const std::string& sLine, const uint32_t nLineNumber);
		
	public:
		CSerialController_Marlin(bool bDebug);
		virtual ~CSerialController_Marlin();

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

		void queryTemperatureState(uint32_t nExtruderIndex) override;
		void queryPositionState() override;

		void getHeatedBedTemperature(double& dTargetTemperature, double& dCurrentTemperature) override;
		void getExtruderTemperature(uint32_t nExtruderIndex, double& dTargetTemperature, double& dCurrentTemperature) override;

		void getTargetPosition(double& dX, double& dY, double& dZ) override;
		void getCurrentPosition(double& dX, double& dY, double& dZ) override;
		void getExtruderPosition(double& dE) override;

		void startHoming() override;
		void setLcdMsg(const std::string& sLcdMsg) override;

		void move(const double dX, const double dY, const double dZ, const double dSpeedInMMperSecond) override;
		void moveFast(const double dX, const double dY, const double dZ, const double dSpeedInMMperSecond) override;

		bool isMoving() override;
		bool canReceiveMovement() override;
		void waitForMovement() override;

	};


}



#endif // __AMC_SERIALCONTROLLER_MARLIN

