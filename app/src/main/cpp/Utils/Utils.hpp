#pragma once

#include <string>
#include <android/log.h>

struct Utils {
#ifdef __LP64__
    typedef unsigned long PTR;
#else
    typedef unsigned int PTR;
#endif
    inline static const char* hexText = "1234567890ABCDEF";

    static std::string ReverseHexString(const std::string&);
    static PTR HexStr2Value(const std::string&);

#if defined(__ARM_ARCH_7A__)
    static bool IsBranchHex(const std::string&);
#elif defined(__aarch64__)
    static bool IsBranchHex(const std::string&);
#elif defined(__i386__) || defined(__x86_64__)
    static bool IsCall(const std::string&);
#endif

    template<size_t len>
    static std::string ReadMemory(PTR memAddr);
    static bool DecodeBranchOrCall(const std::string &hex, PTR offset, PTR &outOffset);
    static PTR FindNextJump(PTR start, uint8_t index);
    static void DumpInstructions(Utils::PTR start, int numInstructions);

    template <typename T>
    static T ReturnEmpty(T ty);
};

class Logger {
private:
    static constexpr const char* logTag{"RelictaLogger"};
    static void Log(const android_LogPriority& type, const std::string& logType, const std::string& text) {
        ((void)__android_log_print(type, logTag, "%s %s", logType.c_str(), text.c_str()));
    }
public:
    static void LogInfo(const std::string& text) { Log(ANDROID_LOG_INFO, "[INFO]", text); }
    static void LogDebug(const std::string& text) { Log(ANDROID_LOG_DEBUG, "[DEBUG]", text); }
    static void LogError(const std::string& text) { Log(ANDROID_LOG_ERROR, "[ERROR]", text); }
    static void LogWarning(const std::string& text) { Log(ANDROID_LOG_WARN, "[WARNING]", text); }
};

template <std::size_t S>
struct StringObfuscator {
private:
    static constexpr unsigned char obfKey = 0xFA;
public:
    std::array<unsigned char, S> strData{};

    constexpr StringObfuscator(const char(&str)[S]) {
        for (std::size_t t = 0; t < S; ++t) {
            strData[t] = static_cast<unsigned char>(str[t] ^ obfKey);
        }
    }

    [[nodiscard]]
    std::string Decode() const {
        std::string newStr;
        newStr.resize(S - 1);
        for (std::size_t i = 0; i < S - 1; ++i)
            newStr[i] = static_cast<char>(strData[i] ^ obfKey);
        return newStr;
    }
};