/**
 * @file UDPSocket.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

#include "ITransport.hpp"

namespace ntpc
{
  class UDPSocket : public ITransport
  {
    public:
#ifdef _WIN32
      explicit UDPSocket();
      virtual ~UDPSocket();
#else
      UDPSocket() = default;
#endif /* _WIN32 */

      /**
       * @brief Bind socket.
       * 
       * @param[in] host Remote host.
       * @param[in] port Remote port.
       * @retval false on error.
       */
      auto bind(std::string_view host, std::uint16_t port) -> bool override;

      /**
       * @brief Get host IPv4 address.
       * 
       * @retval std::uint32_t
       */
      auto ipv4() -> std::uint32_t override;

      /**
       * @brief Test whether socket is valid or not.
       * 
       * @retval bool
       */
      auto valid() -> bool override;

      /**
       * @brief Close socket.
       */
      auto close() -> void override;

      /**
       * @brief Write to socket..
       * 
       * @param[out] buffer Data to be written.
       * @param[in] size Number of data to write.
       * @retval -1 on error otherwise number of bytes written.
       */
      auto write(const char* buffer, std::size_t length) -> int override;

      /**
       * @brief Read from socket.
       * 
       * @param[out] buffer Data read.
       * @param[in] size Number of data to be read.
       * @retval -1 on error otherwise number of bytes read.
       */
      auto read(char* buffer, std::size_t size) -> int override;

      /**
       * @brief Read from socket.
       * 
       * @retval -1 on error otherwise the byte read.
       */
      auto read() -> int override;

      /**
       * @brief Retrieves the number of bytes available on the socket.
       * 
       * @retval -1 on error otherwise number of bytes available.
       */
      auto available() -> int override;

      /**
       * @brief Flushes any existing packets.
       */
      auto flush() -> void override;

      /**
       * @brief Parse the packet read.
       * 
       * @retval -1 on error otherwise the packet size.
       */
      auto parsePacket() -> int override;

    private:
#ifdef _WIN32
      static std::uint32_t mInstances;
      using SocketFd = SOCKET;
#else
      static constexpr int INVALID_SOCKET = -1;
      using SocketFd = int;
#endif /* _WIN32 */
      SocketFd mFd = INVALID_SOCKET;
      std::uint32_t mParsedPacketSize = 0;
      sockaddr_in mAddr{};

      /**
       * @brief IPv4 address recovery.
       * 
       * @param[in] host Hostname.
       * @retval IPv4
       */
      static auto getIPv4(std::string_view host) -> std::uint32_t;
  };
} // namespace ntpc