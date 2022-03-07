// OIEEmu.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <brynet/net/TcpService.hpp>
#include <brynet/net/TcpConnection.hpp>
#include <brynet/net/AsyncConnector.hpp>
#include <brynet/net/wrapper/ConnectionBuilder.hpp>
#include <brynet/base/AppStatus.hpp>

#include <vector>

using namespace brynet;
using namespace brynet::net;


class CPacket {
public:

    std::vector <uint8_t> m_Buffer;

    void addUint8(uint8_t nValue) {
        m_Buffer.push_back(nValue);
    }

    void addBool(bool bValue) {
        if (bValue)
            addUint8(1);
        else
            addUint8(0);
    }

    void addUint32(uint32_t nValue)
    {
        uint8_t* pValue = (uint8_t*)&nValue;
        addUint8(*pValue);
        addUint8(*(pValue + 1));
        addUint8(*(pValue + 2));
        addUint8(*(pValue + 3));
    }

    void addUint64(uint64_t nValue)
    {
        uint8_t* pValue = (uint8_t*)&nValue;
        addUint8(*pValue);
        addUint8(*(pValue + 1));
        addUint8(*(pValue + 2));
        addUint8(*(pValue + 3));
        addUint8(*(pValue + 4));
        addUint8(*(pValue + 5));
        addUint8(*(pValue + 6));
        addUint8(*(pValue + 7));
    }


    void addString(const std::string& sValue)
    {
        addUint32((uint32_t)sValue.length());
        for (const char & c : sValue) {
            addUint8((uint8_t) c);
        }
    }

};

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: <host> <port> <net work thread num> <session num>\n");
        exit(-1);
    }

    CPacket loginRequestPacket;
    loginRequestPacket.addUint32(15); // type
    loginRequestPacket.addUint32(13 + 28); // variable Header size
    loginRequestPacket.addUint32(1); // version
    loginRequestPacket.addString("devicename");
    loginRequestPacket.addString("application");
    loginRequestPacket.addString("version");
    loginRequestPacket.addBool(false);


    auto service = TcpService::Create();
    service->startWorkerThread(atoi(argv[3]));

    auto connector = AsyncConnector::Create();
    connector->startWorkerThread();

    auto enterCallback = [loginRequestPacket](const TcpConnection::Ptr& session) {
        session->setDataCallback([session](const char* buffer, size_t len) {
            session->send(buffer, len);
            return len;
        });
        session->send((char*)loginRequestPacket.m_Buffer.data (), loginRequestPacket.m_Buffer.size ());
    };

    auto failedCallback = []() {
        std::cout << "connect failed" << std::endl;
    };

    wrapper::ConnectionBuilder connectionBuilder;
    connectionBuilder.configureService(service)
        .configureConnector(connector)
        .configureConnectionOptions({
            brynet::net::AddSocketOption::AddEnterCallback(enterCallback),
            brynet::net::AddSocketOption::WithMaxRecvBufferSize(1024 * 1024)
            });

    const auto num = 1;
    const auto ip = argv[1];
    const auto port = std::atoi(argv[2]);
    for (auto i = 0; i < num; i++)
    {
        try
        {
            connectionBuilder.configureConnectOptions({
                    ConnectOption::WithAddr(ip, port),
                    ConnectOption::WithTimeout(std::chrono::seconds(10)),
                    ConnectOption::WithFailedCallback(failedCallback),
                    ConnectOption::AddProcessTcpSocketCallback([](TcpSocket& socket) {
                        socket.setNodelay();
                    })
                })
                .asyncConnect();
        }
        catch (std::runtime_error& e)
        {
            std::cout << "error:" << e.what() << std::endl;
        }
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (brynet::base::app_kbhit())
        {
            break;
        }
    }

    return 0;
}
