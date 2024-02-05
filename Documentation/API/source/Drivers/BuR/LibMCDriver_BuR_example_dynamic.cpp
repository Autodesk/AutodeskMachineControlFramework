#include <iostream>
#include "libmcdriver_bur_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_BuR-library file here.
    auto wrapper = LibMCDriver_BuR::CWrapper::loadLibrary(libpath + "/libmcdriver_bur."); // TODO: add correct suffix of the library
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

