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

    #define CAST_SOCKET_PTR(s) *((SOCKET*) s)

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

            m_Socket = (SOCKET*) rz_malloc_aligned(sizeof(SOCKET));

            CAST_SOCKET_PTR(m_Socket) = socket(family, sockType, 0);
            if (CAST_SOCKET_PTR(m_Socket) == INVALID_SOCKET) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket creation failed: {0}", WSAGetLastError());
                WSACleanup();
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::CreateSocket(void* socket)
        {
            m_Socket = socket;
            if (CAST_SOCKET_PTR(m_Socket) == INVALID_SOCKET) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket invalid");
                WSACleanup();
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::Bind(const RZString& address, uint16_t port)
        {
            sockaddr_in service;
            service.sin_family      = AF_INET;
            service.sin_addr.s_addr = inet_addr(address.c_str());
            service.sin_port        = htons(port);

            if (bind(CAST_SOCKET_PTR(m_Socket), (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket bind failed: {0}", WSAGetLastError());
                Close();
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::Listen(int backlog)
        {
            if (listen(CAST_SOCKET_PTR(m_Socket), backlog) == SOCKET_ERROR) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket Listen failed: {0}", WSAGetLastError());
                Close();
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        RZSocket RZSocket::Accept()
        {
            SOCKET clientSocket = accept(CAST_SOCKET_PTR(m_Socket), nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket Accept failed: {0}", WSAGetLastError());
                return RZSocket();
            }
            RZSocket socket = RZSocket();
            socket.CreateSocket((void*) &clientSocket);
            return socket;
        }

        SocketStatus RZSocket::Send(const char* data, size_t size)
        {
            int result = send(CAST_SOCKET_PTR(m_Socket), data, static_cast<int>(size), 0);
            if (result == SOCKET_ERROR) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket Send failed: {0}", WSAGetLastError());
                return SocketStatus::ERR;
            }
            return SocketStatus::SUCCESS;
        }

        size_t RZSocket::Receive(char* buffer, size_t size)
        {
            int result = recv(CAST_SOCKET_PTR(m_Socket), buffer, static_cast<int>(size), 0);
            if (result == SOCKET_ERROR) {
                RAZIX_CORE_ERROR("[Network//Socket] Socket Receive failed: {0}", WSAGetLastError());
                return -1;
            }
            return result;
        }

        SocketStatus RZSocket::Close()
        {
            if (closesocket(CAST_SOCKET_PTR(m_Socket)) == SOCKET_ERROR) {
                RAZIX_CORE_ERROR("[Network//Socket] m_ListenSocket Close failed: {0}", WSAGetLastError());
                return SocketStatus::ERR;
            }
            WSACleanup();
            return SocketStatus::SUCCESS;
        }

        SocketStatus RZSocket::IsValid() const
        {
            return CAST_SOCKET_PTR(m_Socket) != INVALID_SOCKET ? SocketStatus::SUCCESS : SocketStatus::ERR;
        }
    }    // namespace Network
}    // namespace Razix
#endif