/*++

Copyright (C) 2020 Autodesk Inc.

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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is the class declaration of CProcessController

*/


#ifndef __AMC_PROCESSDIRECTORY
#define __AMC_PROCESSDIRECTORY


#include <cstdint>
#include "Common/common_chrono.hpp"
#include "Common/common_exportstream_native.hpp"

#include "amc_logger.hpp"

#include <map>
#include <vector>
#include <thread>
#include <atomic>
#include <list>


namespace AMC {

    class CProcessDirectoryWriter {
    private:

        std::vector <uint8_t> m_MemoryBuffer;

        uint64_t m_nBytesWritten;
        uint64_t m_nPositionInBuffer;

        AMCCommon::PExportStream_Native m_pExportStream;
        std::string m_sLocalFileName;
        std::string m_sAbsoluteFileName;

    public:

        CProcessDirectoryWriter(const std::string& sLocalFileName, const std::string& sAbsoluteFileName, uint32_t nMemoryBufferSize);

        virtual ~CProcessDirectoryWriter();

        std::string getAbsoluteFileName();

        std::string getLocalFileName();

        void writeData(const uint8_t* pData, uint64_t nSize);

        void flushBuffer();

        void finish();

        bool isFinished();

        uint64_t getWrittenBytes();

    };

    typedef std::shared_ptr<CProcessDirectoryWriter> PProcessDirectoryWriter;
    typedef std::weak_ptr<CProcessDirectoryWriter> WProcessDirectoryWriter;

    class CProcessDirectoryStructure;
    class CProcessDirectory;
    typedef std::shared_ptr<CProcessDirectory> PProcessDirectory;
    typedef std::weak_ptr<CProcessDirectory> WProcessDirectory;

    class CProcessDirectory {
        private:
            bool m_bIsActive;

            std::string m_sWorkingDirectory;
            std::set<std::string> m_MonitoredFileNames;
            std::map<std::string, PProcessDirectoryWriter> m_WriterInstances;
			std::map<std::string, PProcessDirectory> m_SubDirectories;

            CProcessDirectoryStructure* m_pOwner;

        public:


            CProcessDirectory(CProcessDirectoryStructure* pOwner, const std::string& sWorkingDirectory);

            virtual ~CProcessDirectory();

            std::string getWorkingDirectory();

            std::string getAbsoluteFileName(const std::string& sFileName);

            void addNewMonitoredFile(const std::string& sFileName);

            PProcessDirectoryWriter addNewFileWriter(const std::string& sFileName, uint32_t nMemoryBufferSize);

            bool fileIsMonitored(const std::string& sFileName);

            void cleanUpDirectory(AMC::CLogger* pLoggerForUnmanagedFileWarnings);
            
            bool isActive();

            std::set<std::string> getFileNames();

            AMCCommon::PChrono getGlobalChrono();

            AMC::PLogger getLogger();

            PProcessDirectory createSubDirectory(const std::string & sDirectoryName);


    };



    class CProcessDirectoryStructure {
        private:
            AMCCommon::PChrono m_pGlobalChrono;
            AMC::PLogger m_pLogger;

            PProcessDirectory m_pRootDirectory;

        public:

            CProcessDirectoryStructure(const std::string& sBaseDirectory, AMCCommon::PChrono pGlobalChrono, AMC::PLogger pLogger);

            virtual ~CProcessDirectoryStructure();

            PProcessDirectory getRootDirectory ();

            AMCCommon::PChrono getGlobalChrono ();

            AMC::PLogger getLogger ();
    };

    typedef std::shared_ptr<CProcessDirectoryStructure> PProcessDirectoryStructure;


} // namespace AMC

#endif // __AMC_PROCESSDIRECTORY
