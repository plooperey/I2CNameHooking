#pragma once

#include "../2022.3.62f3.h"
#include "../Utils/Utils.hpp"
#include "../Method/Method.hpp"
#include <string>

struct Class {
    Class(const Il2CppObject*);
    Class(const Il2CppClass*);
    Class(const char*, const char*);
    Class() = default;

    void TryInit() const;
    [[nodiscard]] InnerMethod GetMethod(const std::string&) const;
    [[nodiscard]] InnerMethod GetMethod(const std::string&, int) const;

    Il2CppClass* classData{};
};