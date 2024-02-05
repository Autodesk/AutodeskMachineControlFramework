#include <iostream>
#include "libmcdriver_scanlaboie_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_ScanLabOIE-library file here.
    auto wrapper = LibMCDriver_ScanLabOIE::CWrapper::loadLibrary(libpath + "/libmcdriver_scanlaboie."); // TODO: add correct suffix of the library
    LibMCDriver_ScanLabOIE_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_ScanLabOIE.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

