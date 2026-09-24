#pragma once

#include "../2022.3.62f3.h"
#include "../Utils/Utils.hpp"
#include "InnerMethod.hpp"
#include "Dobby/include/dobby.h"

template <typename Ret>
class Method : public InnerMethod {
public:
    constexpr Method() noexcept = default;
    Method(const MethodInfo* inf) : InnerMethod(inf) {}
    Method(const InnerMethod& meth) : InnerMethod(meth) {}
    template <typename Other>
    Method(const Method<Other> oth) : InnerMethod(oth) { this = oth; }

    template <typename ...Params>
    Ret Call(Params ...params) const {
        if (methodData == nullptr) {
            Logger::LogError("Calling invalid function!");
            return Utils::ReturnEmpty<Ret>();
        }
        bool canInfo = true;
        if (sizeof ...(Params) != methodData->parameters_count){
            canInfo = false;
            Logger::LogWarning("Invalid params count on function '" + std::string(methodData->name) + "'!");
        }
        if (!isStatic && instance == nullptr) {
            Logger::LogError("Calling non static function with no instance on function '" + std::string(methodData->name) + "'!");
            return Utils::ReturnEmpty<Ret>();
        }
        auto method = methodData;
        using func = Ret(*)(Il2CppObject*, Params..., MethodInfo*);
        using funcNoInf = Ret(*)(Il2CppObject*, Params...);
        using staticFunc = Ret(*)(Params..., MethodInfo*);
        using staticFuncNoInf = Ret(*)(Params...);
        if (!isStatic) {
            if (canInfo) return reinterpret_cast<func>(methodData->methodPointer)(instance, params..., methodData);
            return reinterpret_cast<funcNoInf>(methodData->methodPointer)(instance, params...);
        }
#if UNITY_VER > 174
        if (canInfo) return reinterpret_cast<staticFunc>(methodData->methodPointer)(params..., methodData);
        return reinterpret_cast<staticFuncNoInf>(methodData->methodPointer)(params...);
#else
        if (canInfo) return reinterpret_cast<func>(methodData->methodPointer)(nullptr, params..., methodData);
        return reinterpret_cast<funcNoInf>(methodData->methodPointer)(nullptr, params..., methodData);
#endif
    }

    template <typename ...Params>
    inline Ret operator()(Params ...params) { return Call(params...);  }

    Method<Ret>& operator[](Il2CppObject* inst) { SetInstance(inst); return *this; }
    Method<Ret>& operator[](void* inst) { SetInstance((Il2CppObject*)inst); return *this; }

    template <typename PTR_T, typename NEW_T, typename OLD_T>
    static void* Hook(PTR_T ptr, NEW_T newMethod, OLD_T& oldBytes) {
        if ((void*)ptr != nullptr)
            ::DobbyHook((void*)ptr, (dobby_dummy_func_t)newMethod, (dobby_dummy_func_t*)&oldBytes);
        return (void*)ptr;
    }

    template <typename TYPE_T, typename NEW_T, typename OLD_T>
    static void* Hook(const Method<TYPE_T>& ptr, NEW_T newMethod, OLD_T& oldBytes) {
        if ((void*)ptr.GetAddress() != nullptr)
            ::DobbyHook((void*)ptr.GetAddress(), (dobby_dummy_func_t)newMethod, (dobby_dummy_func_t*)&oldBytes);
        return (void*)ptr.GetAddress();
    }

    template <typename NEW_T, typename OLD_T>
    void* Hook(NEW_T newMethod, OLD_T& oldMethod) {
        if (this->IsValid())
            Method::Hook(this->GetAddress(), newMethod, oldMethod);
        return this->GetAddress();
    }

    template <typename PTR_T>
    static void Unhook(PTR_T ptr) {
        if ((void*)ptr != nullptr)
            DobbyDestroy((void*)ptr);
    }

    void UnHook() {
        if (this->IsValid())
            Unhook(this->GetAddress());
    }
};