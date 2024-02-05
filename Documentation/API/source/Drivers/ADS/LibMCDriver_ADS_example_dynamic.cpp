#include <iostream>
#include "libmcdriver_ads_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_ADS-library file here.
    auto wrapper = LibMCDriver_ADS::CWrapper::loadLibrary(libpath + "/libmcdriver_ads."); // TODO: add correct suffix of the library
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

