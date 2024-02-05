#include <iostream>
#include "libmcdriver_tcpip_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_TCPIP-library file here.
    auto wrapper = LibMCDriver_TCPIP::CWrapper::loadLibrary(libpath + "/libmcdriver_tcpip."); // TODO: add correct suffix of the library
    LibMCDriver_TCPIP_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_TCPIP.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

