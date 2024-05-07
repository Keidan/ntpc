/**
 * @file NTPClient.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <ctime>
#include "ITransport.hpp"

namespace ntpc
{
  static constexpr std::uint16_t NTP_PORT = 123; /* NTP UDP */
  static constexpr std::uint32_t NTP_DEFAULT_POLL = 1 << 10;

#pragma pack(1)
  struct NTPPacket
  {
      std::uint8_t li_vn_mode = 0;      /* 8 bits. li, vn, and mode. */
      std::uint8_t stratum = 0;         /* 8 bits. Stratum level of the local clock. */
      std::uint8_t poll = 0;            /* 8 bits. Maximum interval between successive messages. */
      std::uint8_t precision = 0;       /* 8 bits. Precision of the local clock. */
      std::uint32_t rootDelay = 0;      /* 32 bits. Total round trip delay time. */
      std::uint32_t rootDispersion = 0; /* 32 bits. Max error aloud from primary clock source. */
      std::uint32_t refId = 0;          /* 32 bits. Reference clock identifier. */
      std::uint32_t refTsSec = 0;       /* 32 bits. Reference time-stamp seconds. */
      std::uint32_t refTsFrac = 0;      /* 32 bits. Reference time-stamp fraction of a second. */
      std::uint32_t origTsSec = 0;      /* 32 bits. Originate time-stamp seconds. */
      std::uint32_t origTsFrac = 0;     /* 32 bits. Originate time-stamp fraction of a second. */
      std::uint32_t recvTsSec = 0;      /* 32 bits. Received time-stamp seconds. */
      std::uint32_t recvTsFraq = 0;     /* 32 bits. Received time-stamp fraction of a second. */
      std::uint32_t transmitTsSec = 0;  /* 32 bits and the most important field the client cares about. Transmit time-stamp seconds. */
      std::uint32_t transmitTsFraq = 0; /* 32 bits. Transmit time-stamp fraction of a second. */
  };
#pragma pack()

  enum class NTPClientResult : std::uint8_t
  {
    Success = 0,
    Error,
    Timeout
  };

  class NTPClient
  {
    public:
      NTPClient(std::string_view host, std::uint16_t port = NTP_PORT);
      virtual ~NTPClient();

      /**
       * @brief Build a new transport interface.
       */
      template <class Transport>
      auto buildTransport() -> void
      {
        mTransport = std::make_unique<Transport>();
      }

      /**
       * @brief Refresh time if conditions are met.
       * @note This method is to be called periodically
       * 
       * @param[out] epoch Time read.
       * @param[in] autowait Performs an automatic wait until the timeout expires.
       * @retval NTPClientResult.
       */
      auto refresh(std::time_t& epoch, bool autowait) -> NTPClientResult;

      /**
       * @brief Force a time refresh.
       * 
       * @param[out] epoch Time read.
       * @retval NTPClientResult.
       */
      auto forceRefresh(std::time_t& epoch) -> NTPClientResult;

    private:
      std::string_view mHost{};
      std::uint16_t mPort = 0;
      NTPPacket mPacket{};
      std::unique_ptr<ITransport> mTransport = nullptr;
      std::uint32_t mUpdateInterval = NTP_DEFAULT_POLL;
      std::time_t mCurrentEpoc = 0;
      std::time_t mLastUpdate = 0;

      /**
       * @brief Sends the packet via the transport interface.
       * 
       * @retval false on error.
       */
      auto sendPacket() -> bool;

      /**
       * @brief Gets time in seconds.
       * 
       * @retval long long
       */
      auto seconds() -> long long;
  };
} // namespace ntpc
