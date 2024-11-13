// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Razix/Network/RZSockets.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>

    // Link with ws2_32.lib
    #pragma comment(lib, "ws2_32.lib")

namespace Razix {
    namespace Network {

        static SOCKET m_ListenSocket     = INVALID_SOCKET;
        static SOCKET m_ConnectionSocket = INVALID_SOCKET;

        SocketStatus RZSocket::CreateSocket(SocketProtocol protocol)
        {
            // TODO: Move this to a central network system like RZNetworkSystem class for global network initialization
            WSADATA wsaData;
            int     result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (result != 0) {
                RAZIX_CORE_ERROR("[Network//Socket] WSA Startup failed!");
                return SocketStatus::ERR;
            }

            // TCP vs UDP
            int sockType = (protocol == SocketProtocol::TCP) ? SOCK_STREAM : SOCK_DGRAM;
            int family   = AF_INET;

            m_ListenSocket = socket(family, sockType, 0);
            if (m_ListenSocket == INVALID_SOCKET) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket creation failed: {0}", WSAGetLastError());
                WSACleanup();
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::Bind(const std::string& address, uint16_t port)
        {
            sockaddr_in service;
            service.sin_family      = AF_INET;
            service.sin_addr.s_addr = inet_addr(address.c_str());
            service.sin_port        = htons(port);

            if (bind(m_ListenSocket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket bind failed: {0}", WSAGetLastError());
                Close();
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::Listen(int backlog)
        {
            if (listen(m_ListenSocket, backlog) == SOCKET_ERROR) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket Listen failed: {0}", WSAGetLastError());
                Close();
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::Accept()
        {
            SOCKET clientSocket = accept(m_ListenSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket Accept failed: {0}", WSAGetLastError());
                return SocketStatus::ERR;
            }
            m_ConnectionSocket = clientSocket;
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::Send(const char* data, size_t size)
        {
            int result = send(m_ConnectionSocket, data, static_cast<int>(size), 0);
            if (result == SOCKET_ERROR) {
                std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        ssize_t RZSocket::Receive(char* buffer, size_t size)
        {
            int result = recv(m_ConnectionSocket, buffer, static_cast<int>(size), 0);
            if (result == SOCKET_ERROR) {
                std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
                return -1;
            }
            return result;
        }

        SocketStatus RZSocket::Close()
        {
            if (closesocket(m_ListenSocket) == SOCKET_ERROR) {
                std::cerr << "Close failed: " << WSAGetLastError() << std::endl;
                return SocketStatus::ERR;
            }
            WSACleanup();
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::IsValid() const
        {
            return m_ListenSocket != INVALID_SOCKET ? SocketStatus::SUCCESS : SocketStatus::ERR;
        }
    }    // namespace Network
}    // namespace Razix

#endif