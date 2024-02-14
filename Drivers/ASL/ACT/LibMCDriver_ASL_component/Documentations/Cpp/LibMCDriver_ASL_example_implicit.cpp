#include <iostream>
#include "libmcdriver_asl_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_ASL::CWrapper::loadLibrary();
    LibMCDriver_ASL_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_ASL.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

