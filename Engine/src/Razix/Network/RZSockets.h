#pragma once

namespace Razix {
    namespace Network {

        // Range of free ports stats: https://stackoverflow.com/questions/10476987/best-tcp-port-number-range-for-internal-applications

        /**
         * Notes:
         * If the client application wants to connect using only IPv6 or IPv4, then the address family needs to be set to AF_INET6 for IPv6 or AF_INET for IPv4 in the hints parameter.
         */

/* Default port to connect the to application running as a server */
#define RAZIX_DEFAULT_SERVER_PORT 29000
#define RAZIX_DEFAULT_CLIENT_PORT 44000

        enum class SocketProtocol
        {
            TCP,
            UDP
        };

        enum class SocketStatus
        {
            SUCCESS = 0,
            ERR,
            TIMEOUT,
            CLOSED
        };

        /**
         * Creates a UDP/TCP socket for network communication
         */
        class RAZIX_API RZSocket
        {
        public:
            RZSocket()  = default;
            ~RZSocket() = default;

            // Creates a socket
            SocketStatus CreateSocket(SocketProtocol protocol);
            SocketStatus CreateSocket(void* socket);

            // Binds the socket to an address and port, typically used by a server connection port
            SocketStatus Bind(const RZString& address, uint16_t port);

            // Starts listening for incoming connections
            SocketStatus Listen(int backlog = 5);

            // Accepts an incoming connection and returns the pointer to the new socket
            RZSocket Accept();

            // Sends data over the socket
            SocketStatus Send(const char* data, size_t size);

            // Receives data from the socket
            size_t Receive(char* buffer, size_t size);

            // Closes the socket
            SocketStatus Close();

            // Checks if the socket is valid
            SocketStatus IsValid() const;

        private:
            void* m_Socket = nullptr;
        };
    }    // namespace Network
}    // namespace Razix