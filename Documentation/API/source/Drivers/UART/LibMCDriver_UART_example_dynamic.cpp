#include <iostream>
#include "libmcdriver_uart_dynamic.hpp"


int main()
{
  try
  {
    std::string libpath = (""); // TODO: put the location of the LibMCDriver_UART-library file here.
    auto wrapper = LibMCDriver_UART::CWrapper::loadLibrary(libpath + "/libmcdriver_uart."); // TODO: add correct suffix of the library
    LibMCDriver_UART_uint32 nMajor, nMinor, nMicro;
    wrapper->GetVersion(nMajor, nMinor, nMicro);
    std::cout << "LibMCDriver_UART.Version = " << nMajor << "." << nMinor << "." << nMicro;
    std::cout << std::endl;
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
  return 0;
}

