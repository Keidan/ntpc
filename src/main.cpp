/**
 * @file main.cpp
 * @author Keidan (Kevin Billonneau)
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 */
/* Includes -----------------------------------------------------------------*/
#include <iostream>
#include <cstring>
#include <csignal>
#include <sstream>
#include <chrono>  // chrono::system_clock
#include <iomanip> // put_time
#include "config.h"
#include "ntpc/Helper.hpp"
#include "ntpc/UDPSocket.hpp"
#include "ntpc/NTPClient.hpp"
#ifndef _WIN32
#  include <getopt.h>
#else
#  include <Windows.h>
#  include "win32/getopt.h"
#endif
/* Defines-------------------------------------------------------------------*/
#ifndef _WIN32
#  define NO_RETURN __attribute__((noreturn))
#else
#  define NO_RETURN
#endif /* _WIN32 */

/* Usings--------------------------------------------------------------------*/
using ntpc::Helper;
using ntpc::UDPSocket;
using ntpc::NTPClient;
using ntpc::NTPClientResult;

/* Private structures--------------------------------------------------------*/
struct Context
{
    std::string host{};
    std::uint16_t port = 0;
    bool update = false;
    bool continuous = false;
    std::uint32_t count = 0;
};
/* Private variables --------------------------------------------------------*/
static const std::vector<option> g_longOptions = {
  {    "help", 0, nullptr, 'h'},
  { "version", 0, nullptr, 'v'},
  { "address", 1, nullptr, 'a'},
  {    "port", 1, nullptr, 'p'},
  {  "update", 0, nullptr, 'u'},
  {"continue", 0, nullptr, 'c'},
  {   "count", 1, nullptr, '1'},
  {   nullptr, 0, nullptr,   0},
};

static auto g_loopExit = false;
/* Static forward -----------------------------------------------------------*/
static auto usage(int32_t xcode) -> void;
static auto version() -> void;
static auto signalHook(int s) -> void;
static auto shutdownHook() -> void;
static auto processRefresh(NTPClient& client, bool loopWait, int& ret, const Context& context) -> void;
static auto printSystemError(std::string_view title, int lastError) -> void;
static auto handleMain(const Context& context) -> int;
static auto handleArguments(int argc, char** argv, Context& context) -> void;

/* Public function ----------------------------------------------------------*/
auto main(int argc, char** argv) -> int
{
  Context context{};
#ifndef _WIN32
  struct sigaction sa;

  std::memset(&sa, 0, sizeof(sigaction));
  sa.sa_handler = &signalHook;
  sigaction(SIGINT, &sa, nullptr);
  sigaction(SIGTERM, &sa, nullptr);
#else
  signal(SIGINT, signalHook);
  signal(SIGTERM, signalHook);
#endif
  atexit(shutdownHook);

  handleArguments(argc, argv, context);

  /* The files are closed in the exit functions */
  return handleMain(context);
}

/* Static functions ---------------------------------------------------------*/
/**
 * @brief Hook function used to capture signals.
 * 
 * @param[in] s Signal.
 */
static NO_RETURN void signalHook(const int s)
{
  exit((SIGINT == s || SIGTERM == s) ? EXIT_SUCCESS : EXIT_FAILURE);
}

/**
 * @brief Hook function called by atexit.
 */
static auto shutdownHook() -> void
{
  g_loopExit = true;
}

/**
 * @brief usage function.
 * 
 * @param[in] xcode The exit code.
 */
static NO_RETURN void usage(const int32_t xcode)
{
  version();
  std::cout << "usage: " << APP_NAME << " [options]" << std::endl;
  std::cout << "\t--help, -h: Print this help." << std::endl;
  std::cout << "\t--address, -a: The remote host address (or name)." << std::endl;
  std::cout << "\t--port, -p: The remote host port." << std::endl;
  std::cout << "\t--update, -u: Update the system with the new date/time (requires sufficient rights)." << std::endl;
  std::cout << "\t--continue, -c: Continuous execution." << std::endl;
  std::cout << "\t--count: Performs a defined number of requests (in conflict with continue option)." << std::endl;
  exit(xcode);
}

/**
 * @brief Print the version
 */
static void version()
{
  std::cout << APP_NAME << " version " << VERSION_MAJOR << "." << VERSION_MINOR << " (";
#if DEBUG
  std::cout << "debug";
#else
  std::cout << "release";
#endif
  std::cout << ")" << std::endl;
}

/**
 * @brief Handles the application's main function.
 * @note This function can call the "exit" method via the "usage" method call.
 * 
 * @param[in] context The application context, which is filled in with the application's arguments.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static auto handleMain(const Context& context) -> int
{
  g_loopExit = !(context.continuous || 0 != context.count);
  auto ret = EXIT_FAILURE;
  NTPClient client(context.host, context.port);
  client.buildTransport<UDPSocket>();
  auto loopWait = context.continuous || 0 != context.count;
  std::uint32_t count = 0;
  do
  {
    processRefresh(client, loopWait, ret, context);
    if((0 != context.count) && (++count >= context.count))
      g_loopExit = true;
  } while(!g_loopExit);
  return ret;
}

static auto processRefresh(NTPClient& client, bool loopWait, int& ret, const Context& context) -> void
{
  std::time_t epoch = 0;
  using enum NTPClientResult;
  auto res = client.refresh(epoch, loopWait);
  if(Error == res)
  {
    ret = EXIT_FAILURE;
    std::cerr << "Unable to retrieve date and time from NTP server!" << std::endl;
  }
  else if(Success == res)
  {
    std::stringstream ss;
    auto ltime = Helper::toTM(&epoch);
    ss << std::put_time(&ltime, "%Y-%m-%d %X");
    std::cout << "Network date/time: " << ss.str() << std::endl;
    if(context.update)
    {
      auto r = Helper::setSystemDateTime(&epoch);
      if(r)
        printSystemError("Unable to change system date and time", r);
      else
        ret = EXIT_SUCCESS;
    }
    else
      ret = EXIT_SUCCESS;
  }
}

static auto printSystemError(std::string_view title, int lastError) -> void
{
  std::string err{};
#ifdef _WIN32
  LPVOID msgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                lastError,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<LPTSTR>(&msgBuf),
                0,
                NULL);
  err = std::string(reinterpret_cast<LPTSTR>(msgBuf));
  LocalFree(msgBuf);
#else
  if(EFAULT == lastError)
    err = "Pointer outside the accessible address space.";
  else if(EINVAL == lastError)
    err = "Timezone (or something else) is invalid.";
  else if(EPERM == lastError)
    err = "The calling process has insufficient privilege to call settimeofday(); under Linux the CAP_SYS_TIME capability is required.";
#endif
  std::cerr << title << ", error: (" << lastError << ") " << err << std::endl;
}

/* Arguments functions ------------------------------------------------------*/
/**
 * @brief Handles arguments passed as parameters to the application.
 * @note This function can call the "exit" method via the "usage" method call.
 * 
 * @param[in] argc The number of arguments.
 * @param[in] argv The list of argumentsArguments count.
 * @param[out] context The context of the application that will be filled from the arguments of the application.
 */
static auto handleArguments(const int argc, char** argv, Context& context) -> void
{
  auto opt = -1;
  std::string sport{};
  std::string scount{};
  /* parse the options */
  const auto* longOptions = g_longOptions.data();
  while(-1 != (opt = getopt_long(argc, argv, "vha:p:uc1:", longOptions, nullptr)))
  {
    switch(opt)
    {
      case 'h': /* help */
        usage(EXIT_SUCCESS);
      case 'v': /* version */
        version();
        exit(EXIT_SUCCESS);
        break;
      case 'a': /* address */
        context.host = optarg;
        break;
      case 'p': /* port */
        sport = optarg;
        break;
      case 'u': /* update */
        context.update = true;
        break;
      case 'c': /* continue */
        context.continuous = true;
        break;
      case '1': /* count */
        scount = optarg;
        break;
      default: /* '?' */
        std::cerr << "Unknown option '" << static_cast<char>(opt) << "'." << std::endl;
        usage(EXIT_FAILURE);
    }
  }
  std::uint32_t port;
  std::uint32_t count = 0;
  std::string what;
  std::vector<std::string> errors;
  if(context.host.empty())
  {
    errors.emplace_back("- Invalid host address!");
  }
  if(sport.empty())
    port = ntpc::NTP_PORT;
  else
  {
    if(!Helper::parseInt(port, sport, what))
    {
      std::string e = "- Invalid port value: ";
      e.append(what);
      errors.emplace_back(e);
    }
    if(0 == port || 65535 < port)
    {
      errors.emplace_back("- Invalid port value: 0 < port <= 65535");
    }
  }
  if(!scount.empty())
  {
    if(!Helper::parseInt(count, scount, what))
    {
      std::string e = "- Invalid count value: ";
      e.append(what);
      errors.emplace_back(e);
    }
    if(context.continuous)
      errors.emplace_back("- Count option conflicts with continue option.");
  }
  if(!errors.empty())
  {
    std::cerr << "Arguments error(s):" << std::endl;
    for(const auto& e : errors)
      std::cerr << e << std::endl;
    usage(EXIT_FAILURE);
  }
  context.port = static_cast<std::uint16_t>(port);
  context.count = count;
}
