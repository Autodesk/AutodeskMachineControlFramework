#include <iostream>
#include "libmcdriver_raylase_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_Raylase::CWrapper::loadLibrary();
    LibMCDriver_Raylase_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_Raylase.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

