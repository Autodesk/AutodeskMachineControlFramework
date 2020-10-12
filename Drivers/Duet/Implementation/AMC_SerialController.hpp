
#ifndef __AMC_SERIALCONTROLLER
#define __AMC_SERIALCONTROLLER

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

namespace AMC {


	class CSerialController {

	private:


	public:
		CSerialController()
		{

		}

		virtual ~CSerialController()
		{

		}

		virtual void initializeController () = 0;
		virtual void disconnectController() = 0;
		virtual void resetController() = 0;
		virtual void emergencyStop() = 0;

		virtual void setPositioningAbolute() = 0;
		virtual void setPositioningRelative() = 0;

		virtual void queryPositionState () = 0;

		virtual void getTargetPosition (double & dX, double& dY, double& dZ) = 0;
		virtual void getCurrentPosition(double& dX, double& dY, double& dZ) = 0;
		virtual void getExtruderTargetPosition (double &dE) = 0;

		virtual void startHoming() = 0;

		virtual void moveXY(const double dX, const double dY, const double dE, const double dSpeedInMMperSecond) = 0;
		virtual void moveFastXY(const double dX, const double dY, const double dSpeedInMMperSecond) = 0;

		virtual void moveZ(const double dZ, const double dE, const double dSpeedInMMperSecond) = 0;
		virtual void moveFastZ(const double dZ, const double dSpeedInMMperSecond) = 0;

		virtual void setAxisPosition(const std::string& sAxis, double dValue) = 0;

		virtual void powerOff() = 0;

		virtual bool isHomed() = 0;
		virtual bool isMoving() = 0;
		virtual bool isConnected() = 0;
		virtual bool canReceiveMovement () = 0;
		virtual void waitForMovement() = 0;

	};

	typedef std::shared_ptr<CSerialController> PSerialController;

}



#endif // __AMC_SERIALCONTROLLER

