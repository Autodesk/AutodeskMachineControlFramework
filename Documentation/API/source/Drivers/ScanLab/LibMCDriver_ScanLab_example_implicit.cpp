#include <iostream>
#include "libmcdriver_scanlab_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_ScanLab::CWrapper::loadLibrary();
    LibMCDriver_ScanLab_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_ScanLab.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

