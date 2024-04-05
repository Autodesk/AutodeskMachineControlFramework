#include <iostream>
#include "libmcdriver_raylase_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_Raylase-library file here.
    auto wrapper = LibMCDriver_Raylase::CWrapper::loadLibrary(libpath + "/libmcdriver_raylase."); // TODO: add correct suffix of the library
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

