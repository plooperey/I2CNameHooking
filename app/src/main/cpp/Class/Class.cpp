#include "Class.hpp"
#include "../Il2CppMethods/Il2CppMethods.hpp"

Class::Class(const Il2CppObject* object) {
    if (!object) {
        Logger::LogWarning("Creating class with null Il2CppObject!");
        return;
    }
    classData = object->klass;
}

Class::Class(const Il2CppClass* clazz) {
    if (!clazz) {
        Logger::LogWarning("Creating class with null Il2CppClass!");
        return;
    }
    classData = (Il2CppClass*)clazz;
}

Class::Class(const char* classNamespace, const char* className) {
    Logger::LogDebug("Finding class with namespace '" + std::string(classNamespace) + "' and class name '" + std::string(className) + "'");
    if (!il2cppMethods.il2cpp_domain_get_assemblies || !il2cppMethods.il2cpp_domain_get) {
        Logger::LogError("Required il2cpp methods are null!");
        return;
    }
    auto domain = il2cppMethods.il2cpp_domain_get();
    size_t assemblyCount = 0;
    auto assemblies = *il2cppMethods.il2cpp_domain_get_assemblies(domain, &assemblyCount);
    if (!assemblies || assemblyCount == 0) {
        Logger::LogError("Assemblies is null!");
        return;
    }
    //Logger::LogDebug("Got all assemblies!");
    for (size_t a = 0; a < assemblyCount; a++) {
        auto assembly = assemblies[a];
        Il2CppImage* image = assembly.image;
        size_t count = image->typeCount;
        for (size_t i = 0; i < count; i++) {
            auto clazz = il2cppMethods.il2cpp_image_get_class(image, i);
            if (!clazz || !clazz->namespaze || !clazz->name) continue;
            if (std::string(clazz->namespaze) == classNamespace &&
                std::string(clazz->name) == className) {
                classData = clazz;
                return;
            }
        }
    }
}

Class::Class(const Image& img, const char* classNamespace, const char* className) {
    uint32_t count = img.imageData->typeCount;
    for (uint32_t i = 0; i < count; i++) {
        auto clazz = il2cppMethods.il2cpp_image_get_class(img.imageData, i);
        if (!clazz || !clazz->namespaze || !clazz->name) continue;
        if (strcmp(clazz->namespaze, classNamespace) == 0 && strcmp(clazz->name, className) == 0) {
            classData = clazz;
            return;
        }
    }
}

void Class::TryInit() const {
    if (classData) il2cppMethods.Class$$Init(classData);
}

InnerMethod Class::GetMethod(const std::string& methodName) const {
    Class::TryInit();
    for (uint16_t i = 0; i < classData->method_count; i++) {
        const MethodInfo* method = classData->methods[i];
        if (method->name == methodName) return method;
    }
    return {};
}

InnerMethod Class::GetMethod(const std::string& methodName, int paramsCount) const {
    Class::TryInit();
    for (uint16_t i = 0; i < classData->method_count; i++) {
        auto method = classData->methods[i];
        if (method->name == methodName && method->parameters_count == paramsCount) return method;
    }
    return {};
}

bool Class::IsValid() const {
    Class::TryInit();
    return classData;
}

std::vector<InnerMethod> Class::GetMethods() {
    if (!IsValid()) {
        Logger::LogWarning("Trying to call GetMethods on an invalid class!");
        return std::vector<InnerMethod>{};
    }
    std::vector<InnerMethod> methods{};
    uint16_t count = classData->method_count;
    for (uint16_t i = 0; i < count; i++) {
        methods.push_back(classData->methods[i]);
    }
    return methods;
}