#include <iostream>
#include "libmcdriver_bur_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_BuR::CWrapper::loadLibrary();
    LibMCDriver_BuR_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_BuR.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

