#pragma once

namespace Razix {
    namespace Network {

        enum class SocketType
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

        class RZSocket
        {
        public:
            virtual ~RZSocket() = default;

            // Creates a socket, returns a boolean if it was successful
            virtual SocketStatus CreateSocket() = 0;

            // Binds the socket to an address and port
            virtual SocketStatus Bind(const std::string& address, uint16_t port) = 0;

            // Starts listening for incoming connections
            virtual SocketStatus Listen(int backlog = 5) = 0;

            // Accepts an incoming connection
            virtual SocketStatus Accept() = 0;

            // Sends data over the socket
            virtual SocketStatus Send(const char* data, size_t size) = 0;

            // Receives data from the socket
            virtual ssize_t Receive(char* buffer, size_t size) = 0;

            // Closes the socket
            virtual SocketStatus Close() = 0;

            // Checks if the socket is valid
            virtual SocketStatus IsValid() const = 0;

            // Converts a socket to a readable string representation for debugging
            virtual std::string ToString() const = 0;
        };

    }    // namespace Network
}    // namespace Razix