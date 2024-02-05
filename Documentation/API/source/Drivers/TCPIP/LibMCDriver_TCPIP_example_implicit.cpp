#include <iostream>
#include "libmcdriver_tcpip_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_TCPIP::CWrapper::loadLibrary();
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

