#pragma once

#include "../2022.3.62f3.h"
#include "../Utils/Utils.hpp"
#include "../Method/Method.hpp"
#include "../Image/Image.hpp"
#include <string>
#include <cstdio>

struct Image;
struct Class {
    Class(const Il2CppObject*);
    Class(const Il2CppClass*);
    Class(const char*, const char*);
    Class(const Image&, const char*, const char*);
    Class() = default;

    void TryInit() const;
    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] InnerMethod GetMethod(const std::string&) const;
    [[nodiscard]] InnerMethod GetMethod(const std::string&, int) const;
    std::vector<InnerMethod> GetMethods();

    Il2CppClass* classData{};
};