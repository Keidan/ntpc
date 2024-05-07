/**
 * @file Helper.hpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
#pragma once

/* Includes -----------------------------------------------------------------*/
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <ctime> // localtime
#ifdef _WIN32
#  include <Windows.h>
#  include <sysinfoapi.h>
#else
#  include <cerrno>
#  include <sys/time.h>
#endif /* _WIN32 */
/* Public defines -----------------------------------------------------------*/

/* Public class -------------------------------------------------------------*/
namespace ntpc
{
  class Helper
  {
    public:
      virtual ~Helper() = default;

      /**
       * @brief Changes system date and time.
       * 
       * @param[in] now Time in epoch format.
       * @retval 0 in case of success, system error otherwise.
       */
      static auto setSystemDateTime(const std::time_t* now) -> int
      {
#ifdef _WIN32
        SYSTEMTIME st;
        auto local = toTM(now);
        st.wYear = local.tm_year + 1900;
        st.wMonth = local.tm_mon + 1;
        st.wDayOfWeek = local.tm_wday;
        st.wDay = local.tm_mday;
        st.wHour = local.tm_hour;
        st.wMinute = local.tm_min;
        st.wSecond = local.tm_sec;
        st.wMilliseconds = 0;
        if(!SetSystemTime(&st))
        {
          return GetLastError();
        }
#else
        struct timeval newTime;
        newTime.tv_sec = *now;
        newTime.tv_usec = 0;
        if(0 != settimeofday(&newTime, nullptr))
          return errno;
#endif /* _WIN32 */
        return 0;
      }

      /**
       * @brief Sets the value from a string.
       * 
       * @param[out] output Output value.
       * @param[in] value Integer value in string format.
       * @param[out] what The cause of the error (if the function returns false).
       * @retval False if error, otherwise true.
       */
      static auto parseInt(std::uint32_t& output, std::string_view value, std::string& what) -> bool
      {
        try
        {
          auto sv = std::string(value);
          std::int32_t val;
          if(value.starts_with("0x"))
          {
            sv = sv.substr(2);
            val = hex2int<std::int32_t>(sv);
          }
          else
            val = std::stoi(sv);
          if(val < 0)
          {
            val = 0U;
          }
          output = val;
        }
        catch(const std::invalid_argument& e)
        {
          what = std::string("invalid_argument: ") + e.what();
          return false;
        }
        catch(const std::out_of_range& e)
        {
          what = std::string("out_of_range: ") + e.what();
          return false;
        }
        return true;
      }

      /**
       * @brief Converts a hexadecimal string to unsigned interger.
       * 
       * @param[in] hex Hexadecimal string.
       * @retval T
       */
      template <typename T>
      static auto hex2int(std::string_view hex) -> T
      {
        int t;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> t;
        return static_cast<T>(t);
      }

      static auto toTM(const std::time_t* now) -> struct tm
      {
          struct tm newtime;
#ifndef _WIN32
          auto* t = localtime(now);
          memcpy(&newtime, t, sizeof(newtime));
#else
          localtime_s(&newtime, now);
#endif /* !_WIN32 */
          return newtime;
      }

      private : Helper() = default;
  };
} // namespace ntpc
