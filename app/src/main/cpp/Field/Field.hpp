#pragma once

#include "../2022.3.62f3.h"
#include "InnerField.hpp"

#pragma pack(push, 1)
template <typename TYPE>
class Field : public InnerField {
public:
    constexpr Field() noexcept = default;
    Field(const FieldInfo* info) : InnerField(info) {};
    Field(const InnerField obj) : InnerField(obj) {};
    template <typename OTHER>
    Field(const Field<OTHER>& oth) : InnerField(oth) {};

    Field& operator[](const Il2CppObject* inst) { SetInstance(inst); return *this; }
    Field& operator[](void* inst) { SetInstance((Il2CppObject*)inst); return *this; }

    TYPE Get();
    TYPE Set(TYPE data);
};
#pragma pack(pop)