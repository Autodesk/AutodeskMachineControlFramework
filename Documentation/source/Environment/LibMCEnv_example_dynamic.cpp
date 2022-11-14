#include <iostream>
#include "libmcenv_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCEnv-library file here.
    auto wrapper = LibMCEnv::CWrapper::loadLibrary(libpath + "/libmcenv."); // TODO: add correct suffix of the library
    LibMCEnv_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCEnv.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

