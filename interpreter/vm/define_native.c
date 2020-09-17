/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdio.h>
#include <time.h>
#include "vm.h"
#include "chunk_debug.h"
#include "my_printf.h"
#include "object.h"
#include "my.h"
#include <dlfcn.h>

// void define_native(vm_t *vm, const char *name, native_fn function)
// {
//     obj_native *nat = new_native(vm, function);
//     push(vm, OBJ_VAL(copy_string(vm, name, my_strlen(name))));
//     push(vm, OBJ_VAL(nat));
//     table_set(vm, &vm->globals, AS_STRING(vm->stack[0]), vm->stack[1]);
//     pop(vm);
//     pop(vm);
// }

void define_lib(vm_t *vm, const char *name, lib_reg *regs)
{
    obj_lib *lib = new_lib(vm);
    push(vm, OBJ_VAL(copy_string(vm, name, my_strlen(name))));
    push(vm, OBJ_VAL(lib));
    for (int i = 0; regs[i].name; i++) {
        table_set(vm, &lib->fields, copy_string(vm, regs[i].name,
                                                my_strlen(regs[i].name)),
                                OBJ_VAL(new_native(vm, regs[i].function)));
    }
    table_set(vm, &vm->globals, AS_STRING(vm->stack[0]), vm->stack[1]);
    pop(vm);
    pop(vm);
}

void loadlib(vm_t *vm)
{
    (void) vm;
    void *handle;
    int (*func_print_name)(vm_t *vm);

    handle = dlopen("./lib/libcsfml.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return;
    }

    func_print_name = dlsym(handle, "luaopen_mylib");
    if (!func_print_name) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return;
    }
    dlclose(handle);
}

void define_native(vm_t *vm, table_t *table, const char *name, native_fn func)
{
    obj_native *native = new_native(vm, func);
    push(vm, OBJ_VAL(native));
    obj_string *method_name = copy_string(vm, name, my_strlen(name));
    push(vm, OBJ_VAL(method_name));
    table_set(vm, table, method_name, OBJ_VAL(native));
    pop(vm);
    pop(vm);
}

// void define_native_property(vm_t *vm, table_t *table,
//                             const char *name, value value)
// {
//     // push(vm, value);
//     // obj_string *property_name = copy_string(vm, name, my_strlen(name));
//     // push(vm, OBJ_VAL(property_name));
//     // table_set(vm, table, property_name, value);
//     // pop(vm);
//     // pop(vm);
// }