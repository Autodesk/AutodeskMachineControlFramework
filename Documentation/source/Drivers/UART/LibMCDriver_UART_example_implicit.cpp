#include <iostream>
#include "libmcdriver_uart_implicit.hpp"


int main()
{
  try
  {
    auto wrapper = LibMCDriver_UART::CWrapper::loadLibrary();
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

