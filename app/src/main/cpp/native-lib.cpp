#include <jni.h>
#include <android/log.h>
#include "dobby.h"
#include <array>
#include <cstddef>
#include "UnityString/UnityString.hpp"
#include "KittyMemory/KittyInclude.hpp"
#include <thread>
#include "Class/Class.hpp"
#include "Utils/Utils.hpp"
#include "Loader.cpp"

void (*Awake)(void*);
void new_Awake(void* instance) {
    Logger::LogInfo("Hooked AccountManager.Awake!");
    Awake(instance);
}

void (*JoinedRoom)(void*, UnityString*);
void new_JoinedRoom(void* inst, const char* room) {
    Logger::LogDebug("Joining room '" + std::string(room) + '\'');
    JoinedRoom(inst, UnityString::CreateUnityString(room));
}

void OnLoad() {
    Class c = Class("", "KeyboardManager");
    InnerMethod m = c.GetMethod("JoinRoom");
    if (m.IsValid()) Method<void>::Hook(m.GetAddress(), new_JoinedRoom, JoinedRoom);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved)
{
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        Logger::LogError("Unable to get ENV!");
        return -1;
    }
    OnLoadFunc = OnLoad;
    if (!Init(env)) {
        Logger::LogError("Failed to init!");
        return -1;
    }
    return JNI_VERSION_1_6;
}