#include <iostream>
#include "libmcdriver_scanlaboie_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_ScanLabOIE::CWrapper::loadLibrary();
    LibMCDriver_ScanLabOIE_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_ScanLabOIE.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

