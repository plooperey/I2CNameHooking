#pragma once

#include <functional>
#include <string>
#include <dlfcn.h>
#include "KittyMemory/KittyInclude.hpp"
#include "../Utils/Utils.hpp"
#include "../Il2CppMethods/Il2CppMethods.hpp"
#include "../IL2CppNames.h"
#include "../Method/Method.hpp"
#include <thread>
#include <jni.h>

namespace Loader {
    void LoadIl2CppMethods();
    void GetElfScanner();
    void* MethodFinder(void*, const char*);
    bool Init(JNIEnv*);
    int il2cpp_init(const char*);

    inline bool successfullyLoaded = false;
    inline void* il2cppHandle{};
    inline Utils::PTR baseAddr{};
    inline ElfScanner eelfScanner{};
    inline std::function<void()> OnLoadFunc{};
}