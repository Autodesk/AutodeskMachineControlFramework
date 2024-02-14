#include <iostream>
#include "libmcdriver_asl_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_ASL-library file here.
    auto wrapper = LibMCDriver_ASL::CWrapper::loadLibrary(libpath + "/libmcdriver_asl."); // TODO: add correct suffix of the library
    LibMCDriver_ASL_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_ASL.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

