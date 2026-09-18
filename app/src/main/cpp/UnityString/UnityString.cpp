#include "UnityString.hpp"
#include "../Il2CppMethods/Il2CppMethods.hpp"

UnityString* UnityString::CreateUnityString(const std::string& str) {
    return il2cppMethods.il2cpp_string_new(str.data());
}

std::string UnityString::str() {
    if (!length) return {};
    return Utf16ToUtf8(chars, length);
}

unsigned int UnityString::GetHash() const {
    const Il2CppChar *p = chars;
    unsigned int h = 0;
    for (int i = 0; i < length; ++i) { h = (h << 5) - h + *p; p++; }
    return h;
}

UnityString* UnityString::Empty() {
    return vmData.String$$Empty ? *vmData.String$$Empty : nullptr;
}

inline bool UnityString::IsNullOrEmpty() const {
    if (!(void*)this) return false;
    if (!length) return false;
    return true;
}

std::string UnityString::Utf16ToUtf8(Il2CppChar *utf16String, size_t length) {
    std::string utf8String{};
    utf8String.reserve(length);
    utf8::unchecked::utf16to8(utf16String, utf16String + length, std::back_inserter(utf8String));
    return utf8String;
}