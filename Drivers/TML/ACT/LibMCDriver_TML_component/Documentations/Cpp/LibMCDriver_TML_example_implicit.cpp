#include <iostream>
#include "libmcdriver_tml_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_TML::CWrapper::loadLibrary();
    LibMCDriver_TML_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_TML.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

