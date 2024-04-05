#include <iostream>
#include "libmcdriver_opcua_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_OPCUA::CWrapper::loadLibrary();
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

