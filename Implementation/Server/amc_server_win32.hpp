/*++

Copyright (C) 2022 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Autodesk Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __AMCSERVER_SERVER_WIN32
#define __AMCSERVER_SERVER_WIN32

#include "amc_server.hpp"
#include <thread>
#include <mutex>

namespace AMC {


    class CServer_Win32;

    class CServerWin32IO : public CServerIO {
    private:
        CServer_Win32* m_pServer;

    public:

        CServerWin32IO();
        virtual ~CServerWin32IO();

        virtual void logMessageString(const std::string& sMessage) override;
        virtual std::string readConfigurationString(const std::string& sFileName) override;

        void setServer(CServer_Win32* pServer);

    };

    typedef std::shared_ptr<CServerWin32IO> PServerWin32IO;




    class CServer_Win32 : public CServer {
    private:

        std::wstring m_sWindowClassName;
        void* m_hInstance;
        void* m_hAccelTable;
        void* m_hWnd;

        std::thread m_ServerThread;
        std::mutex m_ServerMutex;

    public:

        CServer_Win32(void* hInstance);

        ~CServer_Win32();

        void executeNonBlocking (const std::string & sConfigurationFileName);

        void stopServerThread();

        ATOM registerWindowClass();

        void initInstance(int nCmdShow);

        void logMessageString(const std::string& sMessage);

        void* getAccelTable();


    };

}

#endif //__AMCSERVER_SERVER_WIN32


