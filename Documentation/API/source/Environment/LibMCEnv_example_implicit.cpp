#include <iostream>
#include "libmcenv_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCEnv::CWrapper::loadLibrary();
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

