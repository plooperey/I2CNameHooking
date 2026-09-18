#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include "dobby.h"
#include <array>
#include <cstddef>
#include "IL2CppNames.h"
#include "Il2CppMethods/Il2CppMethods.hpp"
#include "UnityString/UnityString.hpp"
#include "KittyMemory/KittyInclude.hpp"
#include <thread>
#include "Class/Class.hpp"
#include "Utils/Utils.hpp"

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
void* unityHandle = nullptr;
ElfScanner eelfScanner{};

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &oldBytes) {
    if ((void *) ptr != nullptr) ::DobbyHook((void *)ptr, (dobby_dummy_func_t) newMethod, (dobby_dummy_func_t*) &oldBytes);
    return (void *) ptr;
}

template<typename PTR_T, typename NEW_T, typename T_OLD>
inline void *BasicHook(PTR_T ptr, NEW_T newMethod, T_OLD &&oldBytes) {
    if ((void *) ptr != nullptr) DobbyHook((void *)ptr, (dobby_dummy_func_t) newMethod, (dobby_dummy_func_t *) &oldBytes);
    return (void *) ptr;
}

template<typename PTR_T>
inline void Unhook(PTR_T ptr) {
    if ((void *) ptr != nullptr) DobbyDestroy((void *)ptr);
}

void* MethodFinder(void* libHandle, const char* symbolName) {
    void* ptr = dlsym(libHandle, symbolName);
    char ptrStr[64];
    snprintf(ptrStr, sizeof(ptrStr), "%p", ptr);
    Logger::LogDebug("pointer to '" + std::string(symbolName) + "': " + std::string(ptrStr));
    return ptr;
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

    DONE:
    if (handle) il2cppHandle = handle;

    // get elfscanner
    Logger::LogInfo("Getting ElfScanner...");
    std::thread(GetElfScanner).join();
    if (!eelfScanner.isValid()) {
        Logger::LogError("Unable to find ElfScanner!");
        return false;
    }

    // get il2cpp methods
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    const uint8_t count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    const uint8_t count = 2;
#endif
    Logger::LogDebug("Count: " + std::to_string(count));
    il2cppMethods.Class$$Init = (decltype(il2cppMethods.Class$$Init)) Utils::FindNextJump(Utils::FindNextJump((Utils::PTR)MethodFinder(handle, IL2CPP_API_il2cpp_array_new_specific), count), count);
#define GetIl2cppMethod(method) do { il2cppMethods.method = reinterpret_cast<decltype(il2cppMethods.method)>(MethodFinder(handle, IL2CPP_API_##method)); char ptrStr[64]; snprintf(ptrStr, sizeof(ptrStr), "%p", (void*)il2cppMethods.method); Logger::LogDebug("Found " #method "! Pointer: " + std::string(ptrStr)); } while (0)
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
            return false;
        }
        Logger::LogDebug("Found assemblyClass!");
        Utils::PTR GetTypesAddr = Class(assemblyClass).GetMethod("GetTypes", 1).GetAddress();
        Utils::DumpInstructions(GetTypesAddr, 200);
        if (GetTypesAddr == 0) {
            Logger::LogError("Unable to find GetTypes address!");
            return false;
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
            return false;
        }
        Logger::LogDebug("Found orig_Image$$GetTypes!");
    }
//    Logger::LogDebug("Finding il2cpp_domain_get_assemblies...");
//    auto adr = (Utils::PTR)MethodFinder(IL2CPP_API_il2cpp_domain_get_assemblies);
//    if (adr == 0) {
//        Logger::LogError("Unable to find domain_get_assemblies!");
//        return false;
//    }
//    Logger::LogDebug("domain_get_assemblies address: " + std::to_string(adr));
//    //Utils::DumpInstructions(adr, 200);
//    il2cppMethods.Assembly$$GetAllAssemblies = (std::vector<Il2CppAssembly *> *(*)())adr;
//    if (!il2cppMethods.Assembly$$GetAllAssemblies) {
//        Logger::LogError("Unable to find Assembly$$GetAllAssemblies!");
//        return false;
//    }
//    Logger::LogDebug("Found Assembly$$GetAllAssemblies!");
    return result;
}

void (*Awake)(void*);
void new_Awake(void* instance) {
    Logger::LogInfo("Hooked AccountManager.Awake!");
    Awake(instance);
}

void OnLoad() {
    Class c = Class("", "AccountManager");
    InnerMethod m = c.GetMethod("Awake");
    Logger::LogDebug("AccountManager.Awake pointer: " + std::to_string(m.GetAddress()));
    if (m.IsValid()) BasicHook(Class("", "AccountManager").GetMethod("Awake").GetAddress(), new_Awake, Awake);
    else Logger::LogError("AccountManager.Awake is invalid!");
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved)
{
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        Logger::LogError("Unable to get ENV!");
        return -1;
    }
    if (!Init(env)) {
        Logger::LogError("Failed to init!");
        return -1;
    } else {
        Logger::LogInfo("Successfully injected into " + KittyMemory::getProcessName() + "!");
        OnLoad();
    }
    return JNI_VERSION_1_6;
}