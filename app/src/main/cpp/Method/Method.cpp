#include "Method.hpp"

InnerMethod::InnerMethod(const MethodInfo* data) {
    if (!data) return;
    methodData = (MethodInfo*)data;
}

bool InnerMethod::IsValid() const {
    if (methodData) return true;
    return false;
}

InnerMethod &InnerMethod::SetInstance(Il2CppObject *inst) {
    if (!inst) {
        Logger::LogWarning("Setting method instance to something invalid!");
        return *this;
    }
    instance = inst;
    return *this;
}

Utils::PTR InnerMethod::GetAddress() const {
    return methodData ? (Utils::PTR)methodData->methodPointer : 0;
}