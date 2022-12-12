#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace Debug
{
    enum Level
    {
        Error = 1,
        Warning = 2,
        Info = 3,
        Verbose = 4,
        Debug = 5,
        Marker = Debug,

        NoLevel = 6 // Do not filter messages in this case
    };

    extern Level CurrentDebugLevel;
}

class Log
{
public:
    explicit Log(Debug::Level level);
    ~Log();

    // Perfect forwarding wrappers to give the chain of objects to cout
    template <typename T>
    Log& operator<<(T&& rhs)
    {
        if (mShouldLog)
            std::cout << std::forward<T>(rhs);

        return *this;
    }

    Log& operator<<(std::filesystem::path&& rhs);

    Log& operator<<(const std::filesystem::path& rhs);

    Log& operator<<(std::u8string&& rhs);

    Log& operator<<(std::u8string_view rhs);

    Log& operator<<(const char8_t* rhs);

private:
    const bool mShouldLog;
};

#endif
