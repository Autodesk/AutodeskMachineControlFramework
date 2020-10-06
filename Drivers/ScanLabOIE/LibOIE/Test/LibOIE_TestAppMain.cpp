// ConsoleApplication1.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <Windows.h>
#include "..\Headers\CppDynamic\liboie_dynamic.hpp"

int main()
{

    try {

        std::cout << "Loading library.." << std::endl;

        auto pLibOIE = LibOIE::CWrapper::loadLibrary("../../build/Debug/liboie.dll");

        std::cout << "Creating server.." << std::endl;

        auto pServer = pLibOIE->CreateServer();

        std::cout << "Accepting Device.." << std::endl;
        pServer->AcceptDevice("testdevice", "", "");

        std::cout << "Starting server.." << std::endl;
        pServer->Start("0.0.0.0", 5752, false);


        while (true) {
            std::cout << "While Loop..." << std::endl;


            Sleep(2000);

            /*auto pIterator = pServer->ListConnections();
            if (pIterator->MoveNext()) {
                auto pConnection = pIterator->GetCurrentConnection();

                pConnection->StartFirmware("test.frm");
                auto pBufferedPackages = pConnection->GetBufferedPyroPackages();

            }  */

        }




    }
    catch (std::exception& E) {
        std::cout << "fatal error: " << E.what() << std::endl;
    }
}
