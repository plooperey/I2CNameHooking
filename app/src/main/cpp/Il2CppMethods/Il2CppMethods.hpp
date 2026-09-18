#pragma once

#include "../2022.3.62f3.h"
#include <vector>
#include "../UnityString/UnityString.hpp"
#include "../Class/Class.hpp"
#include "../Method/Method.hpp"

extern struct VMData {
    Class Object{}, UnityEngine$$Object{}, System$$List{};
    Method<Il2CppReflectionType *> Type$$GetType{};
    Method<void *> Interlocked$$CompareExchange{};
    //Method<BNM::MonoType *> RuntimeType$$MakeGenericType{};
    //Method<BNM::MonoType *> RuntimeType$$MakePointerType{};
    //Method<BNM::MonoType *> RuntimeType$$make_byref_type{};
    Method<Il2CppReflectionMethod *> RuntimeMethodInfo$$MakeGenericMethod_impl{};
    UnityString **String$$Empty{};
} vmData;

extern struct Il2CppMethods {
    Il2CppImage *(*il2cpp_get_corlib)() = nullptr;
    Il2CppClass *(*il2cpp_class_from_name)(const Il2CppImage *, const char *, const char *) = nullptr;
    Il2CppImage *(*il2cpp_assembly_get_image)(const Il2CppAssembly *) = nullptr;
    Il2CppClass *(*il2cpp_image_get_class)(const Il2CppImage *, unsigned int) = nullptr;
    const char *(*il2cpp_method_get_param_name)(const MethodInfo *, uint32_t) = nullptr;
    Il2CppClass *(*il2cpp_class_from_il2cpp_type)(const Il2CppType *) = nullptr;
    Il2CppClass *(*il2cpp_array_class_get)(const Il2CppClass *, uint32_t) = nullptr;
    Il2CppObject *(*il2cpp_type_get_object)(const Il2CppType *) = nullptr;
    Il2CppObject *(*il2cpp_object_new)(const Il2CppClass *) = nullptr;
    Il2CppObject *(*il2cpp_value_box)(const Il2CppClass *, void *) = nullptr;
    Il2CppArray *(*il2cpp_array_new)(const Il2CppClass *, il2cpp_array_size_t) = nullptr;
    void (*il2cpp_field_static_get_value)(const FieldInfo *, void *) = nullptr;
    void (*il2cpp_field_static_set_value)(const FieldInfo *, void *) = nullptr;
    UnityString *(*il2cpp_string_new)(const char *) = nullptr;
    void *(*il2cpp_resolve_icall)(const char *) = nullptr;
    void *(*il2cpp_runtime_invoke)(MethodInfo *, void *, void **, Il2CppException **) = nullptr;
    Il2CppDomain *(*il2cpp_domain_get)() = nullptr;
    Il2CppThread *(*il2cpp_thread_current)(Il2CppDomain *) = nullptr;
    Il2CppThread *(*il2cpp_thread_attach)(Il2CppDomain *) = nullptr;
    void (*il2cpp_thread_detach)(Il2CppThread *) = nullptr;
    void *(*il2cpp_gc_alloc_fixed)(size_t) = nullptr;
    void (*il2cpp_gc_free_fixed)(void*) = nullptr;
    const Il2CppAssembly**(*il2cpp_domain_get_assemblies)(const Il2CppDomain * domain, size_t * size) = nullptr;
    const Il2CppAssembly*(*il2cpp_domain_assembly_open)(Il2CppDomain * domain, const char* name) = nullptr;
    std::vector<Il2CppAssembly *> *(*Assembly$$GetAllAssemblies)() = nullptr;
    void (*orig_Image$$GetTypes)(const Il2CppImage *image, bool exportedOnly, std::vector<Il2CppClass *> *target) = nullptr;
    void (*Class$$Init)(Il2CppClass *klass) = nullptr;
} il2cppMethods;
