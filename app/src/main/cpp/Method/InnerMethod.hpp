#pragma once

#include "../2022.3.62f3.h"
#include "../Utils/Utils.hpp"

template <typename Ret> struct Method;
struct InnerMethod {
    InnerMethod(const MethodInfo* data);
    InnerMethod(const InnerMethod& meth);
    inline constexpr InnerMethod() = default;
    template <typename Other>
    InnerMethod(const Method<Other> oth);
    [[nodiscard]] bool IsValid() const;

    inline InnerMethod& operator[](Il2CppObject* inst) { SetInstance(inst); return *this; }

    InnerMethod& SetInstance(Il2CppObject* inst);

    template<typename NewType>
    inline Method<NewType> &cast() const { return (Method<NewType> &)*this; }

    [[nodiscard]] Utils::PTR GetAddress() const;

    MethodInfo* methodData{};
    bool isStatic = false;
    Il2CppObject* instance{};
};
