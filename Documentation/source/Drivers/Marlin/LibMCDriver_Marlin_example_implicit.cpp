#include <iostream>
#include "libmcdriver_marlin_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_Marlin::CWrapper::loadLibrary();
    LibMCDriver_Marlin_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_Marlin.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

