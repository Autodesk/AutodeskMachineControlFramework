#include <iostream>
#include "libmcdriver_liboapc_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_LibOAPC-library file here.
    auto wrapper = LibMCDriver_LibOAPC::CWrapper::loadLibrary(libpath + "/libmcdriver_liboapc."); // TODO: add correct suffix of the library
    LibMCDriver_LibOAPC_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_LibOAPC.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

