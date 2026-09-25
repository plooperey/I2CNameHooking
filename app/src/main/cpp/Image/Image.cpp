#include "Image.hpp"
#include "../Il2CppMethods/Il2CppMethods.hpp"

Image::Image(const char* asmName) {
    Il2CppDomain* domain = il2cppMethods.il2cpp_domain_get();
    if (!domain) {
        Logger::LogError("Unable to find domain in Image::Image!");
        return;
    }
    size_t count;
    auto assemblies = il2cppMethods.il2cpp_domain_get_assemblies(domain, &count);
    if (count <= 0) {
        Logger::LogError("No assemblies found in domain '" + std::string(domain->friendly_name) + "'!");
        return;
    }
    for (size_t i = 0; i < count; i++) {
        Il2CppImage* img = il2cppMethods.il2cpp_assembly_get_image(assemblies[i]);
        if (strcmp(img->name, asmName) == 0) {
            imageData = img;
            return;
        }
    }
}

Image::Image(const Il2CppAssembly* asmb) {
    imageData = il2cppMethods.il2cpp_assembly_get_image(asmb);
}

bool Image::IsValid() const {
    return imageData;
}

std::vector<Class> Image::GetClasses() {
    if (!IsValid()) {
        Logger::LogWarning("Trying to call GetClasses on an invalid class!");
        return std::vector<Class>{};
    }
    size_t count = imageData->typeCount;
    std::vector<Class> classes{};
    for (size_t i = 0; i < count; i++) {
        Il2CppClass* clazz = il2cppMethods.il2cpp_image_get_class(imageData, i);
        classes.push_back(Class(clazz));
    }
    return classes;
}