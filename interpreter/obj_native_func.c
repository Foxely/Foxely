/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include "object.h"
#include "custom_memory.h"
#include "my.h"
#include "vm.h"

obj_native *new_native(vm_t *vm, native_fn function)
{
    obj_native *native = ALLOCATE_OBJ(vm, obj_native, OBJ_NATIVE);
    native->function = function;
    return native;
}

obj_lib *new_lib(vm_t *vm)
{
    obj_lib *lib = ALLOCATE_OBJ(vm, obj_lib, OBJ_LIB);
    init_table(&lib->fields);
    return lib;
}

obj_custom_field *
new_custom_field(vm_t *vm, char *type, void *field, field_destroyer destroy)
{
    obj_custom_field *custom = ALLOCATE_OBJ(vm, obj_custom_field, OBJ_CUSTOM);
    custom->type = copy_string(vm, type, my_strlen(type));
    custom->field = field;
    custom->destroy = destroy;
    return custom;
}