/**
 * @file ITransport.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

#include <cstdint>
#include <string>
#ifndef _WIN32
#  include <arpa/inet.h>
#else
// link with Ws2_32.lib
#  pragma comment(lib, "Ws2_32.lib")
#  include <WinSock2.h>
#  include <WS2tcpip.h>
#  include <Windows.h>
#endif /* _WIN32 */

namespace ntpc
{
  class ITransport
  {
    public:
      ITransport() = default;
      virtual ~ITransport() = default;

      /**
       * @brief Bind socket.
       * 
       * @param[in] host Remote host.
       * @param[in] port Remote port.
       * @retval false on error.
       */
      virtual auto bind(std::string_view host, std::uint16_t port) -> bool = 0;

      /**
       * @brief Get host IPv4 address.
       * 
       * @retval std::uint32_t
       */
      virtual auto ipv4() -> std::uint32_t = 0;

      /**
       * @brief Test whether socket is valid or not.
       * 
       * @retval bool
       */
      virtual auto valid() -> bool = 0;

      /**
       * @brief Close socket.
       */
      virtual auto close() -> void = 0;

      /**
       * @brief Write to socket..
       * 
       * @param[out] buffer Data to be written.
       * @param[in] size Number of data to write.
       * @retval -1 on error otherwise number of bytes written.
       */
      virtual auto write(const void* buffer, std::size_t length) -> int = 0;

      /**
       * @brief Read from socket.
       * 
       * @param[out] buffer Data read.
       * @param[in] size Number of data to be read.
       * @retval -1 on error otherwise number of bytes read.
       */
      virtual auto read(void* buffer, std::size_t size) -> int = 0;

      /**
       * @brief Read from socket.
       * 
       * @retval -1 on error otherwise the byte read.
       */
      virtual auto read() -> int = 0;

      /**
       * @brief Retrieves the number of bytes available on the socket.
       * 
       * @retval -1 on error otherwise number of bytes available.
       */
      virtual auto available() -> int = 0;

      /**
       * @brief Flushes any existing packets.
       */
      virtual auto flush() -> void = 0;

      /**
       * @brief Parse the packet read.
       * 
       * @retval -1 on error otherwise the packet size.
       */
      virtual auto parsePacket() -> int = 0;
  };
} // namespace ntpc
