#include "Utils.hpp"

template <typename T>
T Utils::ReturnEmpty(T ty) {
    if (std::is_same<T, void>()) return;
    else return {};
}

std::string Utils::ReverseHexString(const std::string &hex) {
    std::string out{};
    for (size_t i = 0; i < hex.length(); i += 2) out.insert(0, hex.substr(i, 2));
    return out;
}

Utils::PTR Utils::HexStr2Value(const std::string &hex) { return strtoull(hex.c_str(), nullptr, 16); }

#if defined(__ARM_ARCH_7A__)

bool Utils::IsBranchHex(const std::string &hex) {
    Utils::PTR hexW = Utils::HexStr2Value(Utils::ReverseHexString(hex));
    return (hexW & 0x0A000000) == 0x0A000000;
}

#elif defined(__aarch64__)

bool Utils::IsBranchHex(const std::string &hex) {
    Utils::PTR hexW = Utils::HexStr2Value(Utils::ReverseHexString(hex));
    return (hexW & 0xFC000000) == 0x14000000 || (hexW & 0xFC000000) == 0x94000000;
}

#elif defined(__i386__) || defined(__x86_64__)

bool Utils::IsCallHex(const std::string &hex) { return hex[0] == 'E' && hex[1] == '8'; }
#elif defined(__riscv)
    #error "Is it released for Android?"
#else
    #error "BNM only supports arm64, arm, x86 and x86_64"
#endif
static const char *hexChars = "0123456789ABCDEF";

template<size_t len>
std::string Utils::ReadMemory(Utils::PTR address) {
    char temp[len]; memset(temp, 0, len);
    std::string ret{};
    if (memcpy(temp, (void *)address, len) == nullptr) return ret;
    ret.resize(len * 2, 0);
    auto buf = (char *)ret.data();
    for (size_t i = 0; i < len; ++i) {
        *buf++ = hexChars[temp[i] >> 4];
        *buf++ = hexChars[temp[i] & 0x0F];
    }
    return ret;
}

bool Utils::DecodeBranchOrCall(const std::string &hex, Utils::PTR offset, Utils::PTR &outOffset) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    if (!Utils::IsBranchHex(hex)) return false;
#if defined(__aarch64__)
    uint8_t add = 0;
#else
    uint8_t add = 8;
#endif
    outOffset = ((int32_t)(((((Utils::HexStr2Value(Utils::ReverseHexString(hex))) & (((uint32_t)1 << 24) - 1) << 0) >> 0) << 2) << (32 - 26)) >> (32 - 26)) + offset + add;
#elif defined(__i386__) || defined(__x86_64__)
    if (!Utils::IsCallHex(hex)) return false;
    outOffset = offset + Utils::HexStr2Value(ReverseHexString(hex).substr(0, 8)) + 5;
#else
    #error "BNM only supports arm64, arm, x86 and x86_64"
    return false;
#endif
    return true;
}

Utils::PTR Utils::FindNextJump(Utils::PTR start, uint8_t index) {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    Utils::PTR offset = 0;
    std::string curHex = Utils::ReadMemory<4>(start);
    Utils::PTR outOffset = 0;
    bool out;
    while (!(out = Utils::DecodeBranchOrCall(curHex, start + offset, outOffset)) || index != 1) {
        offset += 4;
        curHex = Utils::Utils::ReadMemory<4>(start + offset);
        if (out) index--;
    }
    return outOffset;
#elif defined(__i386__) || defined(__x86_64__)
    Utils::PTR offset = 0;
    std::string curHex = Utils::ReadMemory<1>(start);
    Utils::PTR outOffset = 0;
    bool out;
    while (!(out = Utils::IsCallHex(curHex)) || index != 1) {
        offset += 1;
        curHex = Utils::ReadMemory<1>(start + offset);
        if (out) index--;
    }
    Utils::DecodeBranchOrCall(Utils::ReadMemory<5>(start + offset), start + offset, outOffset);
    return outOffset;
#else
    #error "BNM only supports arm64, arm, x86 and x86_64"
    return 0;
#endif
}

void* Utils::OffsetInLib(void *offsetInMemory) {
    if (offsetInMemory == nullptr) return nullptr;
    Dl_info info;
    dladdr(offsetInMemory, &info);
    return (void *) ((Utils::PTR) offsetInMemory - (Utils::PTR) info.dli_fbase);
}