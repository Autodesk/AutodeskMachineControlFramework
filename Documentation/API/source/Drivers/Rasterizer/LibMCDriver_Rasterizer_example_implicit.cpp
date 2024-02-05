#include <iostream>
#include "libmcdriver_rasterizer_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_Rasterizer::CWrapper::loadLibrary();
    LibMCDriver_Rasterizer_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_Rasterizer.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

