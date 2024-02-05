#include <iostream>
#include "libmcdriver_s7net_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_S7Net::CWrapper::loadLibrary();
    LibMCDriver_S7Net_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_S7Net.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

