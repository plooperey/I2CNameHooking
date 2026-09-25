#pragma once

#include "../2022.3.62f3.h"

template <typename TYPE> struct Field;
struct InnerField {
public:
    template <typename OTHER>
    InnerField(const Field<OTHER> obj);
    InnerField(const FieldInfo* info);
    InnerField(const InnerField& oth);

    InnerField& SetInstance(const Il2CppObject* inst) { instance = (Il2CppObject*)inst; return *this; };

    FieldInfo* fieldData{};
    bool isStatic = false;
    Il2CppObject* instance{};
};