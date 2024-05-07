/**
 * @file NTPClient.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include "NTPClient.hpp"
#include <chrono>
#include <thread>
#include <cstring>
/* Usings--------------------------------------------------------------------*/
using namespace ntpc;

/* Private variables --------------------------------------------------------*/
static constexpr std::uint64_t NTP_TIMESTAMP_DELTA = 2208988800;
static constexpr std::uint32_t NTP_RETRY_DELAY = 10;
static constexpr std::uint32_t NTP_TIEMOUT_DELAY = 100; /* TIEMOUT_DELAY * RETRY_DELAY = 1000ms */

/* Public function ----------------------------------------------------------*/

NTPClient::NTPClient(std::string_view host, std::uint16_t port)
  : mHost(host)
  , mPort(port)
{
}

NTPClient::~NTPClient()
{
  if(nullptr != mTransport)
    mTransport->close();
}

/**
 * @brief Refresh time if conditions are met.
 * @note This method is to be called periodically
 * 
 * @param[out] epoch Time read.
 * @param[in] autowait Performs an automatic wait until the timeout expires.
 * @retval NTPClientResult.
 */
auto NTPClient::refresh(std::time_t& epoch, bool autowait) -> NTPClientResult
{
  auto diff = seconds() - mLastUpdate;
  if(diff >= mUpdateInterval || 0 == mLastUpdate)
    return forceRefresh(epoch);
  if(autowait)
  {
    diff = std::abs(mUpdateInterval - diff);
    std::this_thread::sleep_for(std::chrono::seconds(diff));
    return forceRefresh(epoch);
  }
  return NTPClientResult::Timeout; /* return false if update does not occur */
}

/**
 * @brief Force a time refresh.
 * 
 * @param[out] epoch Time read.
 * @retval NTPClientResult
 */
auto NTPClient::forceRefresh(std::time_t& epoch) -> NTPClientResult
{
  using enum NTPClientResult;
  if(nullptr == mTransport)
    return Error;
  if(mTransport->valid())
    mTransport->close();

  if(!mTransport->bind(mHost, mPort))
    return Error;
  /* flush any existing packets */
  mTransport->flush();

  if(!sendPacket())
  {
    mTransport->close();
    return Error;
  }
  auto packetSize = sizeof(mPacket);
  /* set all bytes in the buffer to 0 */
  mPacket = {};
  /* Wait till data is there or timeout... */
  uint8_t timeout = 0;
  int psize = 0;
  auto ret = Success;
  do
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(NTP_RETRY_DELAY));
    psize = mTransport->parsePacket();
    if(timeout > NTP_TIEMOUT_DELAY)
    {
      ret = Error; /* timeout after 1000 ms */
      break;
    }
    timeout++;
  } while(psize == 0);

  if(Success == ret)
  {
    mLastUpdate = seconds() - (timeout / 1000);
    PacketBytes bytes;
    bytes.reserve(packetSize);
    if(mTransport->read(bytes.data(), packetSize) != static_cast<int>(packetSize))
      ret = Error;
    else
    {
      memcpy(&mPacket, bytes.data(), packetSize);
      /* this is NTP time (seconds since Jan 1 1900): */
      epoch = static_cast<uint64_t>(ntohl(mPacket.transmitTsSec)) - NTP_TIMESTAMP_DELTA;
      mUpdateInterval = (0 == mPacket.poll) ? NTP_DEFAULT_POLL : (1 << mPacket.poll);
    }
  }
  mTransport->close();

  return ret;
}
/* Static/Private functions -------------------------------------------------*/
/**
 * @brief Sends the packet via the transport interface.
 * 
 * @retval false on error.
 */
auto NTPClient::sendPacket() -> bool
{
  if(nullptr == mTransport)
    return false;
  auto size = sizeof(mPacket);
  /* set all bytes in the buffer to 0 */
  mPacket = {};
  /* Initialize values needed to form NTP request */
  mPacket.li_vn_mode = 0x1B;          /* Version NTP 4, Client Mode */
  mPacket.poll = 6;                   /* Polling Interval */
  mPacket.precision = 0xEC;           /* Approx. 10^-6 seconds */
  mPacket.refId = mTransport->ipv4(); /* Basic reference ID */
  PacketBytes bytes;
  bytes.reserve(size);
  memcpy(bytes.data(), &mPacket, size);
  return static_cast<int>(size) == mTransport->write(bytes.data(), size);
}

/**
 * @brief Gets time in seconds.
 * 
 * @retval long long
 */
auto NTPClient::seconds() const -> long long
{
  auto now = std::chrono::system_clock::now();
  auto sec = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
  return sec;
}
