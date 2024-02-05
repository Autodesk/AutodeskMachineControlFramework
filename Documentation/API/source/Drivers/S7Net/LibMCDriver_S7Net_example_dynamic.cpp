#include <iostream>
#include "libmcdriver_s7net_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_S7Net-library file here.
    auto wrapper = LibMCDriver_S7Net::CWrapper::loadLibrary(libpath + "/libmcdriver_s7net."); // TODO: add correct suffix of the library
    LibMCDriver_S7Net_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_S7Net.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

