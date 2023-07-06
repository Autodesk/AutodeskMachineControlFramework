/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CRTCSelector

*/


#ifndef __LIBMCDRIVER_SCANLAB_RTCSELECTOR
#define __LIBMCDRIVER_SCANLAB_RTCSELECTOR

#include "libmcdriver_scanlab_interfaces.hpp"
#include "libmcdriver_scanlab_sdk.hpp"

// Parent classes
#include "libmcdriver_scanlab_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlab_rtccontext.hpp"

namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CRTCSelector 
**************************************************************************************************************************/

class CRTCSelector : public virtual IRTCSelector, public virtual CBase {
private:

protected:

	PScanLabSDK m_pScanLabSDK;

	IRTCContext* acquireCardEx(const LibMCDriver_ScanLab_uint32 nNumber, bool bIsNetworkCard);

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	IRTCContextOwner* m_pRTCContextOwner;

public:

	CRTCSelector(IRTCContextOwner* pRTCContextOwner, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CRTCSelector();

	LibMCDriver_ScanLab_uint32 SearchCards(const std::string & sIP, const std::string & sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout) override;

	LibMCDriver_ScanLab_uint32 SearchCardsByRange(const std::string & sStartIP, const std::string & sEndIP, const LibMCDriver_ScanLab_uint32 nTimeout) override;

	LibMCDriver_ScanLab_uint32 GetCardCount() override;

	LibMCDriver_ScanLab_uint32 GetEthernetCardCount() override;

	IRTCContext * AcquireCard(const LibMCDriver_ScanLab_uint32 nNumber) override;

	IRTCContext * AcquireCardBySerial(const LibMCDriver_ScanLab_uint32 nSerialNumber) override;

	IRTCContext * AcquireEthernetCard(const LibMCDriver_ScanLab_uint32 nNumber) override;

	IRTCContext * AcquireEthernetCardBySerial(const LibMCDriver_ScanLab_uint32 nSerialNumber) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_RTCSELECTOR
