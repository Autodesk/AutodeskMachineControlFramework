#ifdef __LIBS7NET_EXPORTS
#define LIBS7NET_API __declspec(dllexport)
#else
#define LIBS7NET_API __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wtypes.h>
#include <ole2.h>
#include <memory>
#include <string>
#include <thread>
#include <mutex>

#include "libs7net_abi.hpp"


#define LIBS7NET_MAXBYTEBUFFERSIZE (1024 * 1024)

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

interface IBase : public IUnknown
{
};


interface IPLCReadData : public IBase
{

    virtual HRESULT __stdcall GetSize(uint32_t& nSize) = 0;

    virtual HRESULT __stdcall GetData(SAFEARRAY** pData) = 0;

};


interface IPLC : public IBase
{

    virtual HRESULT __stdcall Connect (uint32_t nCPUType, BSTR pIP, uint32_t nRack, uint32_t nSlot) = 0;

    virtual HRESULT __stdcall Disconnect() = 0;

    virtual HRESULT __stdcall GetLastError(uint32_t & nCPUType, BSTR * pLastErrorMessage) = 0;

    virtual HRESULT __stdcall WriteBytes(uint32_t DB, uint32_t Offset, SAFEARRAY* pBytes) = 0;

    virtual HRESULT __stdcall ReadBytes(uint32_t DB, uint32_t Offset, uint32_t Count, IPLCReadData** pPLCReadData) = 0; 

    virtual HRESULT __stdcall IsConnected(BOOL & bIsConnected) = 0;

    virtual HRESULT __stdcall IsAvailable(BOOL& bIsConnected) = 0;

    virtual HRESULT __stdcall GetSlot(uint32_t & nSlot) = 0;

    virtual HRESULT __stdcall GetRack(uint32_t& nRack) = 0;

    virtual HRESULT __stdcall GetCPUType(uint32_t& nCPUType) = 0;

    virtual HRESULT __stdcall GetIPAddress(BSTR* pIpAddress) = 0;

};

interface IWrapper : public IUnknown
{
    virtual HRESULT __stdcall CreatePLC(IPLC** ppCalculator) = 0;
};


class __declspec(uuid("{1C0B49DC-9BE4-4157-97B4-79225AAD7907}")) _CLSID_IBase;
class __declspec(uuid("{C27EFFB2-951B-4AF9-BB47-FAEABBDE5598}")) _IID_IBase;
class __declspec(uuid("{0CDEECF2-4252-4A1A-AE01-C98A9B02C78F}")) _CLSID_IPLC;
class __declspec(uuid("{C7ADB662-C099-41C1-831D-0F11BAA29640}")) _IID_IPLC;
class __declspec(uuid("{CB767BA1-E821-4849-8B81-9852F5053384}")) _CLSID_IPLCReadData;
class __declspec(uuid("{1C670D16-177D-44B2-B9E5-B264520178D6}")) _IID_IPLCReadData;
class __declspec(uuid("{EB056EA7-29D5-480E-A351-C95A544DCC95}")) _CLSID_IWrapper;
class __declspec(uuid("{10C868F6-6FCA-47FB-B7AA-0160193BA204}")) _IID_IWrapper;
   
#define CLSID_IBase __uuidof (_CLSID_IBase)
#define IID_IBase __uuidof (_IID_IBase)
#define CLSID_IPLC __uuidof (_CLSID_IPLC)
#define IID_IPLC __uuidof (_IID_IPLC)
#define CLSID_IPLCReadData __uuidof (_CLSID_IPLCReadData)
#define IID_IPLCReadData __uuidof (_IID_IPLCReadData)
#define CLSID_IWrapper __uuidof (_CLSID_IWrapper)
#define IID_IWrapper __uuidof (_IID_IWrapper)


class CCOMObject {
private:
    IBase* m_pIBase;
    std::auto_ptr<std::string> m_pLastError;
public:
    CCOMObject(IBase* pIBase)
        : m_pIBase(pIBase)
    {        
    }

    ~CCOMObject()
    {
        if (m_pIBase != nullptr)
            m_pIBase->Release();
        m_pIBase = nullptr;
    }

    IBase* get()
    {
        return m_pIBase;
    }

    IPLC* castToPLC()
    {
        if (m_pIBase == nullptr)
            return nullptr;

        IPLC* iPLC = nullptr;
        HRESULT hResult = m_pIBase->QueryInterface(IID_IPLC, (void**)&iPLC);
        if (hResult != S_OK)
            return nullptr;

        return iPLC;
    }

    IPLCReadData* castToPLCReadData()
    {
        if (m_pIBase == nullptr)
            return nullptr;

        IPLCReadData* iPLCReadData = nullptr;
        HRESULT hResult = m_pIBase->QueryInterface(IID_IPLCReadData, (void**)&iPLCReadData);
        if (hResult != S_OK)
            return nullptr;

        return iPLCReadData;
    }

    void incRefCount()
    {
        if (m_pIBase != nullptr) {
            m_pIBase->AddRef();
        }
    }

    bool decRefCount()
    {
        if (m_pIBase != nullptr) {
            ULONG nCount = m_pIBase->Release();
            if (nCount == 0) {
                delete this;
                return true;
            }
        }

        return false;
    }

    void registerErrorMessage(const std::string& sErrorMessage)
    {
        if (m_pLastError.get() == nullptr) {
            m_pLastError.reset(new std::string());
        }
        *m_pLastError = sErrorMessage;
    }


    void clearErrorMessages()
    {
        m_pLastError.reset();
    }


    bool getLastErrorMessage(std::string& sErrorMessage)
    {
        if (m_pLastError.get()) {
            sErrorMessage = *m_pLastError;
            return true;
        }
        else {
            sErrorMessage = "";
            return false;
        }
    }

};



class CComWrapper {
private:
    std::mutex m_Mutex;
    IWrapper* m_pWrapper;

    void _releaseModule()
    {
        if (m_pWrapper != nullptr)
            m_pWrapper->Release();
        m_pWrapper = nullptr;
    }

    void _initModule()
    {
        _releaseModule();

        CoInitialize(nullptr);
		
		std::string sCOMHost = std::string (__STRINGIZE_VALUE_OF(__GITHASH)) + "_libs7net_managed.comhost.dll";
		std::wstring sCOMHostW (sCOMHost.begin(), sCOMHost.end());

        HMODULE hModule = LoadLibraryW (sCOMHostW.c_str ());
        if (hModule != nullptr) {
            HRESULT(WINAPI * pfnGetFactory)(REFCLSID, REFIID, void**) = (HRESULT(WINAPI*)(REFCLSID, REFIID, void**)) GetProcAddress (hModule, "DllGetClassObject");;
            if (pfnGetFactory != nullptr) {
                IClassFactory* pFactory = NULL;
                HRESULT hResult = pfnGetFactory (CLSID_IWrapper, IID_IClassFactory, (void**)&pFactory);
                if (hResult == S_OK) {
                    IUnknown* iUnknown = nullptr;

                    hResult = pFactory->CreateInstance(NULL, IID_IUnknown, (void**)&iUnknown);
                    if (hResult == S_OK) {
                        IWrapper* iWrapper = nullptr;
                        hResult = iUnknown->QueryInterface(IID_IWrapper, (void**)&iWrapper);

                        if (hResult == S_OK) {
                            m_pWrapper = iWrapper;
                        }
                    }
                    pFactory->Release();

                }
            }
        }



    }

public:


    CComWrapper()
        : m_pWrapper(nullptr)
    {
    }

    ~CComWrapper()
    {
        _releaseModule();
    }

    IWrapper* getWrapper()
    {
        std::lock_guard<std::mutex> lockGuard(m_Mutex);

        if (m_pWrapper == nullptr)
            _initModule();

        return m_pWrapper;
    }

};


CComWrapper GlobalWrapper;

extern "C" {



    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plcreaddata_getdata(LibS7Net_PLCReadData pPLCReadData, const LibS7Net_uint64 nDataBufferSize, LibS7Net_uint64* pDataNeededCount, LibS7Net_uint8* pDataBuffer)
    {
        HRESULT hResult;

        if (pPLCReadData == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLCReadData* iPLCReadData = ((CCOMObject*)pPLCReadData)->castToPLCReadData();
        if (iPLCReadData == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;

        uint32_t nSize = 0;
        hResult = iPLCReadData->GetSize(nSize);
        if (hResult != S_OK)
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        if (pDataNeededCount != nullptr) {
            *pDataNeededCount = nSize;
        }

        if (pDataBuffer != nullptr) {

            SAFEARRAY * pData = nullptr;
            hResult = iPLCReadData->GetData(&pData);
            if (hResult != S_OK)
                return LIBS7NET_ERROR_GENERICEXCEPTION;

            if (pData == nullptr)
                return LIBS7NET_ERROR_GENERICEXCEPTION;


            hResult = SafeArrayLock(pData);
            if (SUCCEEDED(hResult))
            {
                // Access safe array’s data via psa->pvData
                BYTE* pSrc = static_cast<BYTE*>(pData->pvData);

                // Process data ...
                uint8_t* pDst = pDataBuffer;
                for (uint32_t nIndex = 0; nIndex < nSize; nIndex++) {
                    *pDst = *pSrc;
                    pSrc++;
                    pDst++;
                }
            }

            SafeArrayDestroy(pData);

        }

        return LIBS7NET_SUCCESS;

    }


    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_connect(LibS7Net_PLC pPLC, LibS7Net::eCPUType eCPUType, const char* pIP, LibS7Net_uint32 nRack, LibS7Net_uint32 nSlot)
    {
        HRESULT hResult;

        if (pPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pIP == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLC* iPLC = ((CCOMObject*)pPLC)->castToPLC();
        if (iPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;

        std::string sString = pIP;
        std::wstring sWideString (sString.begin (), sString.end());
        BSTR oleString = SysAllocString(sWideString.c_str());

        hResult = iPLC->Connect ((uint32_t)eCPUType, oleString, nRack, nSlot);

        SysFreeString(oleString);

        if (hResult != S_OK)
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        return LIBS7NET_SUCCESS;
    }


    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_isconnected(LibS7Net_PLC pPLC, bool* pConnected)
    {
        HRESULT hResult;

        if (pPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pConnected == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLC* iPLC = ((CCOMObject*)pPLC)->castToPLC();
        if (iPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;
        
        BOOL bValue = false;
        hResult = iPLC->IsConnected(bValue);
        if (hResult != S_OK)
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        *pConnected = bValue;

        return LIBS7NET_SUCCESS;
    }

    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_isavailable(LibS7Net_PLC pPLC, bool* pAvailable)
    {
        HRESULT hResult;

        if (pPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pAvailable == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLC* iPLC = ((CCOMObject*)pPLC)->castToPLC();
        if (iPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;

        BOOL bValue = false;
        hResult = iPLC->IsAvailable(bValue);
        if (hResult != S_OK)
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        *pAvailable = bValue;

        return LIBS7NET_SUCCESS;
    }


    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_getslot(LibS7Net_PLC pPLC, LibS7Net_uint32* pSlot)
    {
        HRESULT hResult;

        if (pPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pSlot == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLC* iPLC = ((CCOMObject*)pPLC)->castToPLC();
        if (iPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;

        uint32_t nValue = 0;
        hResult = iPLC->GetSlot(nValue);
        if (hResult != S_OK)
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        *pSlot = nValue;

        return LIBS7NET_SUCCESS;
    }


    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_getrack(LibS7Net_PLC pPLC, LibS7Net_uint32* pRack)
    {
        HRESULT hResult;

        if (pPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pRack == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLC* iPLC = ((CCOMObject*)pPLC)->castToPLC();
        if (iPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;

        uint32_t nValue = 0;
        hResult = iPLC->GetRack(nValue);
        if (hResult != S_OK)
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        *pRack = nValue;

        return LIBS7NET_SUCCESS;
    }

    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_getcputype(LibS7Net_PLC pPLC, LibS7Net::eCPUType* pCPUType)
    {
        HRESULT hResult;

        if (pPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pCPUType == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLC* iPLC = ((CCOMObject*)pPLC)->castToPLC();
        if (iPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;

        uint32_t nValue = 0;
        hResult = iPLC->GetRack(nValue);
        if (hResult != S_OK)
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        *pCPUType = (LibS7Net::eCPUType) nValue;

        return LIBS7NET_SUCCESS;
    }

    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_getipaddress(LibS7Net_PLC pPLC, const LibS7Net_uint32 nIPAddressBufferSize, LibS7Net_uint32* pIPAddressNeededChars, char* pIPAddressBuffer)
    {
        return LIBS7NET_SUCCESS;
    }

    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_writebytes(LibS7Net_PLC pPLC, LibS7Net_uint32 nDB, LibS7Net_uint32 nOffset, LibS7Net_uint64 nBytesBufferSize, const LibS7Net_uint8* pBytesBuffer)
    {
        HRESULT hResult;

        if (pPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pBytesBuffer == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        if (nBytesBufferSize > LIBS7NET_MAXBYTEBUFFERSIZE)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLC* iPLC = ((CCOMObject*)pPLC)->castToPLC();
        if (iPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;

        SAFEARRAYBOUND saBound;
        saBound.lLbound = 0;
        saBound.cElements = (uint32_t) nBytesBufferSize;
        
        SAFEARRAY* pBuffer = SafeArrayCreate(VT_UI1, 1, &saBound);
        if (pBuffer == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        
        hResult = SafeArrayLock(pBuffer);
        if (SUCCEEDED(hResult))
        {
            uint8_t* pDst = static_cast<BYTE*>(pBuffer->pvData);
            const uint8_t * pSrc = pBytesBuffer;

            for (LibS7Net_uint64 nIndex = 0; nIndex < nBytesBufferSize; nIndex++) {
                *pDst = *pSrc;
                pDst++;
                pSrc++;
            }

            SafeArrayUnlock(pBuffer);
        }
        else {
            SafeArrayDestroy(pBuffer);
            return LIBS7NET_ERROR_GENERICEXCEPTION;
        }

        hResult = iPLC->WriteBytes (nDB, nOffset, pBuffer);

        SafeArrayDestroy(pBuffer);
        if (hResult != S_OK)
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        return LIBS7NET_SUCCESS;
    }

    LIBS7NET_DECLSPEC LibS7NetResult libs7net_plc_readbytes(LibS7Net_PLC pPLC, LibS7Net_uint32 nDB, LibS7Net_uint32 nOffset, LibS7Net_uint32 nCount, LibS7Net_PLCReadData* pReadData)
    {

        if (pPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pReadData == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IPLC* iPLC = ((CCOMObject*)pPLC)->castToPLC();
        if (iPLC == nullptr)
            return LIBS7NET_ERROR_INVALIDCAST;

        IPLCReadData* iPLCReadData = nullptr;
        HRESULT hResult = iPLC->ReadBytes (nDB, nOffset, nCount, &iPLCReadData);
        if ((iPLCReadData == nullptr) || (hResult != S_OK))
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        *pReadData = new CCOMObject(iPLCReadData);

        return LIBS7NET_SUCCESS;
    }


    LIBS7NET_DECLSPEC LibS7NetResult libs7net_getversion(LibS7Net_uint32* pMajor, LibS7Net_uint32* pMinor, LibS7Net_uint32* pMicro)
    {
        if (pMajor == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pMinor == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pMicro == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        *pMajor = LIBS7NET_VERSION_MAJOR;
        *pMinor = LIBS7NET_VERSION_MINOR;
        *pMicro = LIBS7NET_VERSION_MICRO;

        return LIBS7NET_SUCCESS;
    }
    
    LIBS7NET_DECLSPEC LibS7NetResult libs7net_getlasterror(LibS7Net_Base pInstance, const LibS7Net_uint32 nErrorMessageBufferSize, LibS7Net_uint32* pErrorMessageNeededChars, char* pErrorMessageBuffer, bool* pHasError)
    {
        if (pInstance == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;
        if (pHasError == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        std::string sErrorMessage;
        if (((CCOMObject*)pInstance)->getLastErrorMessage(sErrorMessage)) {

            if (pErrorMessageNeededChars)
                *pErrorMessageNeededChars = (LibS7Net_uint32)(sErrorMessage.size() + 1);
            if (pErrorMessageBuffer) {
                if (sErrorMessage.size() >= nErrorMessageBufferSize)
                    return LIBS7NET_ERROR_BUFFERTOOSMALL;
                
                for (size_t iErrorMessage = 0; iErrorMessage < sErrorMessage.size(); iErrorMessage++)
                   pErrorMessageBuffer[iErrorMessage] = sErrorMessage[iErrorMessage];
                pErrorMessageBuffer[sErrorMessage.size()] = 0;
            }

            *pHasError = true;
        }
        else {
            if (pErrorMessageNeededChars)
                *pErrorMessageNeededChars = 1;

            if (pErrorMessageBuffer) {
                if (nErrorMessageBufferSize < 1)
                    return LIBS7NET_ERROR_BUFFERTOOSMALL;
                *pErrorMessageBuffer = 0;
            }

            *pHasError = false;
        }

        return LIBS7NET_SUCCESS;

    }

    LIBS7NET_DECLSPEC LibS7NetResult libs7net_acquireinstance(LibS7Net_Base pInstance)
    {
        if (pInstance == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        ((CCOMObject*)pInstance)->incRefCount();            

        return LIBS7NET_SUCCESS;
    }

    LIBS7NET_DECLSPEC LibS7NetResult libs7net_releaseinstance(LibS7Net_Base pInstance)
    {
        if (pInstance == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        ((CCOMObject*)pInstance)->decRefCount();
        return LIBS7NET_SUCCESS;
    }


    LIBS7NET_DECLSPEC LibS7NetResult libs7net_createplc(LibS7Net_PLC* pVálue)
    {
        if (pVálue == nullptr)
            return LIBS7NET_ERROR_INVALIDPARAM;

        IWrapper* iWrapper = GlobalWrapper.getWrapper ();
        if (iWrapper == nullptr)
            return LIBS7NET_ERROR_GENERICEXCEPTION;
        
        IPLC* iPLC = nullptr;
        HRESULT hResult = iWrapper->CreatePLC (&iPLC);
        if ((iPLC == nullptr) || (hResult != S_OK))
            return LIBS7NET_ERROR_GENERICEXCEPTION;

        *pVálue = new CCOMObject(iPLC);

        return LIBS7NET_SUCCESS;

    }


}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

