#include <iostream>
#include "libmcdriver_ximc_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_Ximc-library file here.
    auto wrapper = LibMCDriver_Ximc::CWrapper::loadLibrary(libpath + "/libmcdriver_ximc."); // TODO: add correct suffix of the library
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

