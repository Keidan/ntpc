/**
 * @file UDPSocket.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "UDPSocket.hpp"
#ifndef _WIN32
#  include <sys/ioctl.h>
#  include <netdb.h>
#  include <sys/param.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#endif /* !_WIN32 */
#include <cstring>
/* Defines-------------------------------------------------------------------*/

/* Usings--------------------------------------------------------------------*/
using namespace ntpc;
/* Private structures--------------------------------------------------------*/

/* Private variables --------------------------------------------------------*/
#ifdef _WIN32
std::uint32_t UDPSocket::mInstances = 0;
#endif /* _WIN32 */
/* Static forward -----------------------------------------------------------*/

/* Public function ----------------------------------------------------------*/
#ifdef _WIN32
UDPSocket::UDPSocket()
{
  if(0U == mInstances)
  {
    WSADATA wsaData{};
    WSAStartup(MAKEWORD(2, 2), &wsaData);
  }
  mInstances++;
}
UDPSocket::~UDPSocket()
{
  if(1U == mInstances)
  {
    WSACleanup();
  }
  mInstances--;
}
#endif /* _WIN32 */

/**
 * @brief Bind socket.
 * 
 * @param[in] host Remote host.
 * @param[in] port Remote port.
 * @retval false on error.
 */
auto UDPSocket::bind(std::string_view host, std::uint16_t port) -> bool
{
  mParsedPacketSize = 0;
  if(INVALID_SOCKET != mFd)
    close();
  mFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(INVALID_SOCKET == mFd)
    return false;

  struct sockaddr_in addr;
  addr.sin_addr.s_addr = getIPv4(host);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  auto len = sizeof(addr);
  if(!::bind(mFd, reinterpret_cast<sockaddr*>(&addr), len))
  {
    close();
    return false;
  }
  memcpy(&mAddr, &addr, len);
  return true;
}

/**
 * @brief Get host IPv4 address.
 * 
 * @retval std::uint32_t
 */
auto UDPSocket::ipv4() -> std::uint32_t
{
  return mAddr.sin_addr.s_addr;
}

/**
 * @brief Test whether socket is valid or not.
 * 
 * @retval bool
 */
auto UDPSocket::valid() -> bool
{
  return INVALID_SOCKET != mFd;
}

/**
 * @brief Close socket.
 */
auto UDPSocket::close() -> void
{
  if(INVALID_SOCKET == mFd)
  {
#ifdef _WIN32
    closesocket(mFd);
#else
    ::close(mFd);
#endif /* _WIN32 */
    mFd = INVALID_SOCKET;
  }
}

/**
 * @brief Write to socket..
 * 
 * @param[out] buffer Data to be written.
 * @param[in] size Number of data to write.
 * @retval -1 on error otherwise number of bytes written.
 */
auto UDPSocket::write(const void* buffer, std::size_t length) -> int
{
  if(INVALID_SOCKET == mFd)
    return -1;
  return sendto(mFd, reinterpret_cast<const char*>(buffer), length, 0, (struct sockaddr*)&mAddr, sizeof(mAddr));
}

/**
 * @brief Read from socket.
 * 
 * @param[out] buffer Data read.
 * @param[in] size Number of data to be read.
 * @retval -1 on error otherwise number of bytes read.
 */
auto UDPSocket::read(void* buffer, std::size_t size) -> int
{
  if(INVALID_SOCKET == mFd)
    return -1;
  sockaddr_in from;
  socklen_t fromlen = sizeof(from);
  auto reads = recvfrom(mFd, reinterpret_cast<char*>(buffer), size, 0, reinterpret_cast<sockaddr*>(&from), &fromlen);

  if(reads > 0)
    mParsedPacketSize -= reads;
  return reads;
}

/**
 * @brief Read from socket.
 * 
 * @retval -1 on error otherwise the byte read.
 */
auto UDPSocket::read() -> int
{
  std::uint8_t b;
  if(1 != read(reinterpret_cast<void*>(&b), sizeof(b)))
    return -1;
  return b;
}

/**
 * @brief Retrieves the number of bytes available on the socket.
 * 
 * @retval -1 on error otherwise number of bytes available.
 */
auto UDPSocket::available() -> int
{
  if(INVALID_SOCKET == mFd)
    return 0;
  unsigned long nb = 0;
#ifdef _WIN32
  if(0 != ioctlsocket(mFd, FIONREAD, &nb))
#else
  if(0 != ioctl(mFd, FIONREAD, &nb))
#endif /* _WIN32 */
    return -1;

  return static_cast<int>(nb);
}

/**
 * @brief Flushes any existing packets.
 */
auto UDPSocket::flush() -> void {}

/**
 * @brief Parse the packet read.
 * 
 * @retval -1 on error otherwise the packet size.
 */
auto UDPSocket::parsePacket() -> int
{
  if(INVALID_SOCKET == mFd)
    return 0;

  if(mParsedPacketSize > 0)
  {
    // previously parsed data, discard data
    while(available())
      read();
  }

  return (mParsedPacketSize = available());
}

/**
 * @brief IPv4 address recovery.
 * 
 * @param[in] host Hostname.
 * @retval IPv4
 */
auto UDPSocket::getIPv4(std::string_view host) -> std::uint32_t
{
  struct addrinfo hints;
  struct addrinfo* result;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
  hints.ai_flags = AI_CANONNAME;   /* For wildcard IP address */
  hints.ai_protocol = IPPROTO_TCP;
  auto s = getaddrinfo(host.data(), nullptr, &hints, &result);
  if(0 != s)
    return 0;
  std::uint32_t ipv4 = 0;
  for(auto* rp = result; nullptr != rp; rp = rp->ai_next)
    if(AF_INET == rp->ai_family && SOCK_STREAM == rp->ai_socktype && IPPROTO_TCP == rp->ai_protocol)
    {
      ipv4 = reinterpret_cast<struct sockaddr_in*>(rp->ai_addr)->sin_addr.s_addr;
      break;
    }

  freeaddrinfo(result); /* No longer needed */
  return ipv4;
}