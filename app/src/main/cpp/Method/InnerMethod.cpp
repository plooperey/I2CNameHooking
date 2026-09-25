#include "InnerMethod.hpp"

InnerMethod::InnerMethod(const MethodInfo* info) {
    methodData = (MethodInfo*)info;
    isStatic = (info->flags & 0x0010) == 0x0010;
}

InnerMethod::InnerMethod(const InnerMethod& meth) {
    methodData = meth.methodData;
    isStatic = meth.isStatic;
    instance = meth.instance;
}

template<typename Other>
InnerMethod::InnerMethod(const Method<Other> oth) {
    methodData = oth.methodData;
    isStatic = oth.isStatic;
    instance = oth.instance;
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