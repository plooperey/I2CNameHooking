#include "InnerMethod.hpp"

InnerMethod::InnerMethod(const MethodInfo* inf) {
    methodData = (MethodInfo*)inf;
}

bool InnerMethod::IsValid() const {
    return methodData != nullptr;
}

InnerMethod& InnerMethod::SetInstance(Il2CppObject* inst) {
    instance = inst;
    return *this;
}

Utils::PTR InnerMethod::GetAddress() const {
    return (Utils::PTR)methodData->methodPointer;
}