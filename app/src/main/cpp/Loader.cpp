#include <functional>
#include <string>
#include <dlfcn.h>
#include "KittyMemory/KittyInclude.hpp"
#include "Utils/Utils.hpp"
#include "Il2CppMethods/Il2CppMethods.hpp"
#include "IL2CppNames.h"
#include "Method/Method.hpp"
#include <thread>
#include <jni.h>

#if defined(__ARM_ARCH_7A__)
#    define CURRENT_ARCH "armeabi-v7a"
#elif defined(__aarch64__)
#    define CURRENT_ARCH "arm64-v8a"
#elif defined(__i386__)
#    define CURRENT_ARCH "x86"
#elif defined(__x86_64__)
#    define CURRENT_ARCH "x86_64"
#elif defined(__riscv)
#    define CURRENT_ARCH "riscv64"
#endif

#define UNITY_VERSION 222

bool successfullyLoaded = false;
void* il2cppHandle = nullptr;
Utils::PTR baseAddr{};
ElfScanner eelfScanner{};
std::function<void()> OnLoadFunc{};

void* MethodFinder(void* libHandle, const char* symbolName) {
    return dlsym(libHandle, symbolName);
}

void GetElfScanner() {
    std::string libName = "libil2cpp.so";
    while (!eelfScanner.isValid()) {
        sleep(1);
        eelfScanner = ElfScanner::findElf(libName);

        auto nativeSo = LinkerScanner::Get().findSoInfo(libName);
        if (nativeSo.ptr) {
            eelfScanner = ElfScanner::createWithSoInfo(nativeSo);
        }
        auto emSo = NativeBridgeScanner::Get().findSoInfo(libName);
        if (emSo.ptr) {
            eelfScanner = ElfScanner::createWithSoInfo(emSo);
        }
        for (auto& it : ElfScanner::findSymbolAll("il2cpp_init", EScanElfType::Any, EScanElfFilter::App)) {
            if (it.second.dynamic()) {
                eelfScanner = it.second;
                break;
            }
        }
    }
    sleep(1);
}


void LoadIl2CppMethods() {
    // get il2cpp methods
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    const uint8_t count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    const uint8_t count = 2;
#endif
    Logger::LogDebug("Count: " + std::to_string(count));
    il2cppMethods.Class$$Init = (decltype(il2cppMethods.Class$$Init)) Utils::FindNextJump(Utils::FindNextJump((Utils::PTR)MethodFinder(il2cppHandle, IL2CPP_API_il2cpp_array_new_specific), count), count);
#define GetIl2cppMethod(method) do { il2cppMethods.method = (decltype(il2cppMethods.method))MethodFinder(il2cppHandle, IL2CPP_API_##method); char ptrStr[64]; snprintf(ptrStr, sizeof(ptrStr), "%p", (void*)il2cppMethods.method); Logger::LogDebug("Found " #method "! Pointer: " + std::string(ptrStr)); } while (0)
    GetIl2cppMethod(il2cpp_get_corlib);
    GetIl2cppMethod(il2cpp_class_from_name);
    GetIl2cppMethod(il2cpp_assembly_get_image);
    GetIl2cppMethod(il2cpp_image_get_class);
    GetIl2cppMethod(il2cpp_method_get_param_name);
    GetIl2cppMethod(il2cpp_class_from_il2cpp_type);
    GetIl2cppMethod(il2cpp_array_class_get);
    GetIl2cppMethod(il2cpp_type_get_object);
    GetIl2cppMethod(il2cpp_object_new);
    GetIl2cppMethod(il2cpp_value_box);
    GetIl2cppMethod(il2cpp_array_new);
    GetIl2cppMethod(il2cpp_field_static_get_value);
    GetIl2cppMethod(il2cpp_field_static_set_value);
    GetIl2cppMethod(il2cpp_string_new);
    GetIl2cppMethod(il2cpp_resolve_icall);
    GetIl2cppMethod(il2cpp_runtime_invoke);
    GetIl2cppMethod(il2cpp_domain_get);
    GetIl2cppMethod(il2cpp_thread_current);
    GetIl2cppMethod(il2cpp_thread_attach);
    GetIl2cppMethod(il2cpp_thread_detach);
    GetIl2cppMethod(il2cpp_gc_alloc_fixed);
    GetIl2cppMethod(il2cpp_gc_free_fixed);
    GetIl2cppMethod(il2cpp_domain_get_assemblies);
    GetIl2cppMethod(il2cpp_domain_assembly_open);
#undef GetIl2cppMethod
    Logger::LogDebug("Found all il2cpp methods!");

    if (il2cppMethods.il2cpp_image_get_class == nullptr) {
        Logger::LogDebug("il2cpp_image_get_class is null, finding it using System::Reflection.Assembly...");
        auto assemblyClass = il2cppMethods.il2cpp_class_from_name(il2cppMethods.il2cpp_get_corlib(), "System.Reflection", "Assembly");
        if (!assemblyClass) {
            Logger::LogError("Unable to find assemblyClass!");
            return;
        }
        Logger::LogDebug("Found assemblyClass!");
        Utils::PTR GetTypesAddr = Class(assemblyClass).GetMethod("GetTypes", 1).GetAddress();
        if (GetTypesAddr == 0) {
            Logger::LogError("Unable to find GetTypes address!");
            return;
        }
        Logger::LogDebug("GetTypes address: " + std::to_string(GetTypesAddr));
#if UNITY_VERSION >= 211
        const int sCount = count;
#elif UNITY_VERSION > 174
        const int sCount = count + 1;
#else
        const int sCount = count + 2;
#endif
        Logger::LogDebug("finding orig_Image$$GetTypes...");
        il2cppMethods.orig_Image$$GetTypes = (decltype(il2cppMethods.orig_Image$$GetTypes))Utils::FindNextJump(Utils::FindNextJump(Utils::FindNextJump(GetTypesAddr, count), sCount), count);
        if (!il2cppMethods.orig_Image$$GetTypes) {
            Logger::LogError("Unable to find orig_Image$$GetTypes!");
            return;
        }
        Logger::LogDebug("Found orig_Image$$GetTypes!");
    }
//    Logger::LogDebug("Finding il2cpp_domain_get_assemblies...");
//    auto adr = (Utils::PTR)MethodFinder(il2cppHandle, IL2CPP_API_il2cpp_domain_get_assemblies);
//    if (adr == 0) {
//        Logger::LogError("Unable to find domain_get_assemblies!");
//        return;
//    }
//    Logger::LogDebug("domain_get_assemblies address: " + std::to_string(adr));
//    il2cppMethods.Assembly$$GetAllAssemblies = (std::vector<Il2CppAssembly*> *(*)())(Utils::FindNextJump(adr, count));
//    if (!il2cppMethods.Assembly$$GetAllAssemblies) {
//        Logger::LogError("Unable to find Assembly$$GetAllAssemblies!");
//        return;
//    }
//    Logger::LogDebug("Found Assembly$$GetAllAssemblies!");
    Logger::LogInfo("Getting ElfScanner...");
    std::thread(GetElfScanner).join();
    if (!eelfScanner.isValid()) {
        Logger::LogError("Unable to find ElfScanner!");
        return;
    }
    Logger::LogDebug("Got ElfScanner!");
    Logger::LogInfo("Successfully injected into " + KittyMemory::getProcessName() + "!");
    OnLoadFunc();
}

int il2cpp_init(const char* domain) {
    Logger::LogDebug("Hooked il2cpp_init");
    auto ret = old_il2cpp_init(domain);
    Logger::LogDebug("domain name: " + std::string(domain) + " ret: " + std::to_string(ret));
    Method<void>::Unhook(il2cpp_init_ptr);
    LoadIl2CppMethods();
    return ret;
}

bool Init(JNIEnv* env) {
    bool result = false;
    if (successfullyLoaded) return result;
    // get application
    jobject application = nullptr;
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jobject currentThread = env->CallStaticObjectMethod(activityThread, env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;"));
    application = env->CallObjectMethod(currentThread, env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;"));
    env->DeleteLocalRef(currentThread);

    // get application info and flags
    jobject applicationInfo = env->CallObjectMethod(application, env->GetMethodID(env->GetObjectClass(application), "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;"));
    jclass applicationInfoClass = env->GetObjectClass(applicationInfo);
    jint flags = env->GetIntField(applicationInfo, env->GetFieldID(applicationInfoClass, "flags", "I"));
    bool libsExtracted = (flags & 0x10000000) == 0x10000000;

    // get libil2cpp dir
    std::string fieldName;
    if (libsExtracted) fieldName = "nativeLibraryDir";
    else fieldName = "sourceDir";
    auto dirJStr = (jstring)env->GetObjectField(applicationInfo, env->GetFieldID(applicationInfoClass, fieldName.c_str(), "Ljava/lang/String;"));
    std::string dirStr = (std::string)env->GetStringUTFChars(dirJStr, nullptr);
    env->DeleteLocalRef(applicationInfo);
    env->DeleteLocalRef(applicationInfoClass);
    std::string il2cppPath;
    if (libsExtracted) il2cppPath = "/libil2cpp.so";
    else il2cppPath = "!/lib/" CURRENT_ARCH "/libil2cpp.so";
    std::string il2cppDir = dirStr + il2cppPath;

    // check il2cpp handle
    void* handle = dlopen(il2cppDir.c_str(), RTLD_LAZY);
    void* init = dlsym(handle, IL2CPP_API_il2cpp_init);
    if (!init) {
        if (libsExtracted) {
            Logger::LogError("Failed to load libil2cpp.so!");
            return false;
        }
    } else {
        result = true;
        il2cpp_init_ptr = Method<void>::Hook(init, il2cpp_init, old_il2cpp_init);
        Logger::LogDebug("Hooked il2cpp_init 1 ptr: " + std::to_string((Utils::PTR)il2cpp_init_ptr));
        goto DONE;
    }
    if (libsExtracted) goto DONE;

    // get libil2cpp dir if not found
    il2cppDir.clear();
    il2cppDir = dirStr.substr(0, dirStr.length() - 8) + "split_config." CURRENT_ARCH ".apk!/lib/" CURRENT_ARCH "/libil2cpp.so";
    handle = dlopen(il2cppDir.c_str(), RTLD_LAZY);
    if (!handle) return false;
    init = dlsym(handle, IL2CPP_API_il2cpp_init);
    if (!init) {
        return false;
    }
    il2cpp_init_ptr = Method<void>::Hook(init, il2cpp_init, old_il2cpp_init);
    Logger::LogDebug("Hooked il2cpp_init 2 ptr: " + std::to_string((Utils::PTR)il2cpp_init_ptr));

    DONE:
    if (handle) il2cppHandle = handle;
    successfullyLoaded = true;
    Logger::LogDebug("Init is done!");
    return result;
}