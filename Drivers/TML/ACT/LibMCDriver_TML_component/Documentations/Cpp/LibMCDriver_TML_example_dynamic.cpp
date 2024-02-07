#include <iostream>
#include "libmcdriver_tml_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_TML-library file here.
    auto wrapper = LibMCDriver_TML::CWrapper::loadLibrary(libpath + "/libmcdriver_tml."); // TODO: add correct suffix of the library
    LibMCDriver_TML_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_TML.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

