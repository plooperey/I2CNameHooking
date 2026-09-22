#pragma once

#include "../2022.3.62f3.h"
#include "../Utils/Utils.hpp"
#include "InnerMethod.hpp"

template <typename Ret>
struct Method : public InnerMethod {
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
    inline Ret operator()(Params ...params) { Call(params...);  }
};