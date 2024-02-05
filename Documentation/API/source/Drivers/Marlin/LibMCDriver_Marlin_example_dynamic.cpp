#include <iostream>
#include "libmcdriver_marlin_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_Marlin-library file here.
    auto wrapper = LibMCDriver_Marlin::CWrapper::loadLibrary(libpath + "/libmcdriver_marlin."); // TODO: add correct suffix of the library
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

