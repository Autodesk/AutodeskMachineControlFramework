#include <iostream>
#include "libmcdriver_ximc_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_Ximc::CWrapper::loadLibrary();
    LibMCDriver_Ximc_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_Ximc.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

