#include "Utils.hpp"

std::string Utils::ReverseHexString(const std::string &hex) {
    std::string out{};
    for (size_t i = 0; i < hex.length(); i += 2) out.insert(0, hex.substr(i, 2));
    return out;
}

Utils::PTR Utils::HexStr2Value(const std::string &hex) {
    return strtoull(hex.c_str(), nullptr, 16);
}

#if defined(__ARM_ARCH_7A__)

bool Utils::IsBranchHex(const std::string& hex) {
    PTR hexW = Utils::HexStr2Value(Utils::ReverseHexString(hex));
    return (hexW & 0x0A000000) == 0x0A000000;
}

#elif defined(__aarch64__)

bool Utils::IsBranchHex(const std::string& hex) {
    PTR hexW = Utils::HexStr2Value(Utils::ReverseHexString(hex));
    return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
}


#elif defined(__i386__) || defined(__x86_64__)
bool Utils::IsCall(const std::string& hex) {
    return hex[0] == 'E' && hex[1] == '8';
}
#endif

template<size_t len>
std::string Utils::ReadMemory(Utils::PTR memAddr) {
    char temp[len];
    memset(temp, 0, len);
    std::string ret{};
    if (memcpy(temp, (void *)memAddr, len) == nullptr) return ret;
    ret.resize(len * 2, 0);
    auto buf = (char*)ret.data();
    for (size_t i = 0; i < len; ++i) {
        *buf++ = hexText[temp[i] >> 4];
        *buf++ = hexText[temp[i] & 0x0F];
    }
    return ret;
}

bool Utils::DecodeBranchOrCall(const std::string &hex, Utils::PTR offset, Utils::PTR &outOffset) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    if (!IsBranchHex(hex)) return false;
#if defined(__aarch64__)
        uint8_t add = 0;
#else
        uint8_t add = 8;
#endif
        outOffset = ((int32_t)(((((HexStr2Value(ReverseHexString(hex))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + add;
#elif defined(__i386__) || defined(__x86_64__)
    if (!Utils::IsCall(hex)) return false;
    outOffset = offset + Utils::HexStr2Value(Utils::ReverseHexString(hex).substr(0, 8)) + 5;
#else
    #error "BNM only supports arm64, arm, x86 and x86_64"
        return false;
#endif
    return true;
}

Utils::PTR Utils::FindNextJump(Utils::PTR start, uint8_t index) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    PTR offset = 0;
        std::string curHex = Utils::ReadMemory<4>(start);
        const PTR maxOffset = 0x8000;
        PTR outOffset = 0;
        bool out;
        while (!(out = Utils::DecodeBranchOrCall(curHex, start + offset, outOffset)) || index != 1) {
            offset += 4;
            if (offset > maxOffset) {
                char buf[32];
                snprintf(buf, sizeof(buf), "0x%lx", (unsigned long)start);
                Logger::LogError("FindNextJump: scan limit hit from start=" + std::string(buf));
                return 0;
            }
            curHex = Utils::ReadMemory<4>(start + offset);
            if (out) index--;
        }
        return outOffset;
#elif defined(__i386__) || defined(__x86_64__)
    PTR offset = 0;
    std::string curHex = Utils::ReadMemory<1>(start);
    const PTR maxOffset = 0x8000;
    PTR outOffset = 0;
    bool out;
    while (!(out = Utils::IsCall(curHex)) || index != 1) {
        offset += 1;
        if (offset > maxOffset) {
            char buf[32];
            snprintf(buf, sizeof(buf), "0x%lx", (unsigned long)start);
            Logger::LogError("FindNextJump: scan limit hit from start=" + std::string(buf));
            return 0;
        }
        curHex = Utils::ReadMemory<1>(start + offset);
        if (out) index--;
    }
    Utils::DecodeBranchOrCall(Utils::ReadMemory<5>(start + offset), start + offset, outOffset);
    return outOffset;
#endif
}

void Utils::DumpInstructions(Utils::PTR start, int numInstructions) {
    for (int i = 0; i < numInstructions; i++) {
        Utils::PTR addr = start + (i * 4);
        std::string hex = Utils::ReadMemory<4>(addr);
        bool isBranch = Utils::IsBranchHex(hex);
        char buf[64];
        snprintf(buf, sizeof(buf), "0x%lx: %s %s", (unsigned long)addr, hex.c_str(), isBranch ? "<-- BRANCH" : "");
        Logger::LogDebug(std::string(buf));
    }
}

template <typename T>
T Utils::ReturnEmpty(T ty) {
    if (std::is_same<T, void>()) return;
    else return {};
}