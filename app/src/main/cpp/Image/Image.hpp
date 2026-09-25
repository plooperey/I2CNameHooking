#pragma once

#include "../Class/Class.hpp"
#include "../2022.3.62f3.h"
#include "../Utils/Utils.hpp"
#include <vector>
#include <cstdio>

struct Class;

struct Image {
    Image(const char*);
    Image(const Il2CppAssembly*);
    [[nodiscard]] bool IsValid() const;
    std::vector<Class> GetClasses();

    Il2CppImage* imageData{};
};