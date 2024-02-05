#include <iostream>
#include "libmcdriver_duet_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_Duet-library file here.
    auto wrapper = LibMCDriver_Duet::CWrapper::loadLibrary(libpath + "/libmcdriver_duet."); // TODO: add correct suffix of the library
    LibMCDriver_Duet_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_Duet.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

