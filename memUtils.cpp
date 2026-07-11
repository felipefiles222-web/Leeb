#include "memUtils.h"

#include "And64InlineHook.hpp"

uintptr_t g_libAddress = 0;

uintptr_t GetLibraryAddress(const char* libName) {
    FILE* fp = fopen("/proc/self/maps", "rt");
    if (fp == nullptr) {
        perror("fopen");
        return 0;
    }

    uintptr_t addr = 0;
    char line[1024];

    while (fgets(line, sizeof(line), fp) != nullptr) {
        if (strstr(line, libName) != nullptr) {
            addr = strtoul(line, nullptr, 16);
            break;
        }
    }

    fclose(fp);
    return addr;
}


uintptr_t GetActualOffset(uintptr_t offset)
{
    if (g_libAddress == 0)
    {
        g_libAddress = GetLibraryAddress("libPVZ2.so");
    }
    return g_libAddress + offset;
}

uintptr_t GetOriginalOffset(uintptr_t actualOffset) {
    if (g_libAddress == 0)
    {
        g_libAddress = GetLibraryAddress("libPVZ2.so");
    }
    return actualOffset - g_libAddress;
}

uintptr_t* GetMemberPtr(void* obj, uintptr_t memberOffset)
{
    // OJO: la version original hacia "*(uintptr_t*)(*(int*)obj + memberOffset)" y
    // devolvia "&member" (direccion de una variable LOCAL -> dangling pointer, UB) y
    // encima truncaba obj a 'int' (32 bits) antes de sumarle el offset. Esto asume que
    // el uso real es "puntero al byte 'memberOffset' dentro de obj", que es lo que
    // efectivamente se necesita para leer/escribir un campo a mano.
    return (uintptr_t*)((uintptr_t)obj + memberOffset);
}

void PVZ2HookFunction(uintptr_t offset, void* replace, void** result)
{
    A64HookFunction((void*)GetActualOffset(offset), replace, result);
}

void* CopyVFTable(intptr_t vftableAddr, int numVFuncs)
{
    // OJO: la version original usaba 'int' para la direccion (trunca un puntero de 64
    // bits) y sizeof(int) en vez de sizeof(void*) (copiaba la mitad de los bytes
    // necesarios por slot en ARM64).
    size_t size = numVFuncs * sizeof(void*);
    void* vftableCopy = malloc(size);
    memcpy(vftableCopy, (const void*)vftableAddr, size);
    return vftableCopy;
}

void CopyVFTable(void* dest, intptr_t vftableAddr, int numVFuncsToCopy)
{
    size_t size = numVFuncsToCopy * sizeof(void*);
    memcpy(dest, (const void*)vftableAddr, size);
}

void PatchVFTable(void* vftable, void* funcAddr, int index)
{
    ((reinterpret_cast<void**>(vftable))[index]) = funcAddr;
}

void* CreateChildVFTable(int vFuncsCount, intptr_t parentVftable, int nuMVFuncsToCopy)
{
    size_t size = vFuncsCount * sizeof(void*);
    void* childVftable = malloc(size);
    CopyVFTable(childVftable, parentVftable, nuMVFuncsToCopy);

    return childVftable;
}

void SetVFTable(void* obj, uintptr_t newVftablePtr)
{
    // OJO: la version original escribia con 'int' (4 bytes), truncando el puntero de
    // vtable de 64 bits y corrompiendo la mitad alta.
    *reinterpret_cast<uintptr_t*>(obj) = newVftablePtr;
}

void* GetVirtualFunc(void* obj, int index)
{
    void** vtable = *reinterpret_cast<void***>(obj);
    return vtable[index];
}