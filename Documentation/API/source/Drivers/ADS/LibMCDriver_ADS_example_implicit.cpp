#include <iostream>
#include "libmcdriver_ads_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_ADS::CWrapper::loadLibrary();
    LibMCDriver_ADS_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_ADS.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

