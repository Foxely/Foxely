/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include "object.h"
#include "custom_memory.h"
#include "my.h"

obj_t *allocate_object(vm_t *vm, size_t size, obj_type type)
{
    obj_t *object = reallocate(vm, NULL, 0, size);
    object->type = type;
    object->is_marked = false;

    object->next = vm->objects;
    vm->objects = object;
#ifdef DEBUG_LOG_GC
    printf("%p allocate %ld for %d\n", (void*)object, size, type);
#endif
    return object;
}

void print_object(value val)
{
    if (OBJ_TYPE(val) == OBJ_BOUND_METHOD)
        print_function(AS_BOUND_METHOD(val)->method->function);
    if (OBJ_TYPE(val) == OBJ_CLASS)
        printf("%s class", AS_CLASS(val)->name->chars);
    if (OBJ_TYPE(val) == OBJ_CLOSURE)
        print_function(AS_CLOSURE(val)->function);
    if (OBJ_TYPE(val) == OBJ_FUNCTION)
        print_function(AS_FUNCTION(val));
    if (OBJ_TYPE(val) == OBJ_INSTANCE)
        printf("%s instance", AS_INSTANCE(val)->klass->name->chars);
    if (OBJ_TYPE(val) == OBJ_NATIVE)
        printf("<native fn>");
    if (OBJ_TYPE(val) == OBJ_STRING)
        printf("%s", AS_CSTRING(val));
    if (OBJ_TYPE(val) == OBJ_UPVALUE)
        printf("upvalue");
    if (OBJ_TYPE(val) == OBJ_CUSTOM)
        printf("<custom %s>", AS_CUSTOM(val)->type->chars);
}

obj_instance *new_instance(vm_t *vm, obj_class *klass)
{
    obj_instance *instance = ALLOCATE_OBJ(vm, obj_instance, OBJ_INSTANCE);
    instance->klass = klass;
    init_table(&instance->fields);
    return instance;
}

obj_bound_method *
new_bound_method(vm_t *vm, value receiver, obj_closure *method)
{
    obj_bound_method *bound = ALLOCATE_OBJ(vm, obj_bound_method,
                                                OBJ_BOUND_METHOD);
    bound->receiver = receiver;
    bound->method = method;
    return bound;
}