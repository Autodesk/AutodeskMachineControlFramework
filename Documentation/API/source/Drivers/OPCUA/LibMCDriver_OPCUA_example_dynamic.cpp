#include <iostream>
#include "libmcdriver_opcua_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_OPCUA-library file here.
    auto wrapper = LibMCDriver_OPCUA::CWrapper::loadLibrary(libpath + "/libmcdriver_opcua."); // TODO: add correct suffix of the library
    LibMCDriver_OPCUA_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_OPCUA.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

