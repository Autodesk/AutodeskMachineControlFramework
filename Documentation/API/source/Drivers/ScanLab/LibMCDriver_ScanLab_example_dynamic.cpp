#include <iostream>
#include "libmcdriver_scanlab_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_ScanLab-library file here.
    auto wrapper = LibMCDriver_ScanLab::CWrapper::loadLibrary(libpath + "/libmcdriver_scanlab."); // TODO: add correct suffix of the library
    LibMCDriver_ScanLab_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_ScanLab.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

