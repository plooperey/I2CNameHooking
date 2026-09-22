#pragma once

#include "../2022.3.62f3.h"
#include <string>
#include "utf8.h"

struct UnityString : Il2CppObject {
    static std::string Utf16ToUtf8(Il2CppChar* utf16String, size_t length);

    size_t length{};
    Il2CppChar chars[0]{};
    std::string str();

    [[nodiscard]] unsigned int GetHash() const;

    static UnityString* Empty();
    static UnityString* CreateUnityString(const std::string& str);

    [[nodiscard]] inline bool IsNullOrEmpty() const;
    inline constexpr UnityString() : Il2CppObject() {}
    UnityString(const char*);
};