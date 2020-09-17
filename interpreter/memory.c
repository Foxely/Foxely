/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include "vm.h"
#include "custom_memory.h"
#include "object.h"
#include "table.h"
#include "compiler.h"
#ifdef DEBUG_LOG_GC
#include <stdio.h>
#include "debug.h"
#endif

static void free_object(vm_t *vm, obj_t *object)
{
#ifdef DEBUG_LOG_GC
    printf("%p free vm, type %d\n", (void*)object, object->type);
#endif
    switch (object->type) {
        case OBJ_BOUND_METHOD:
            FREE(vm, obj_bound_method, object);
            break;
        case OBJ_CLASS: {
            obj_class *klass = (obj_class *) object;
            free_table(vm, &klass->methods);
            FREE(vm, obj_class, object);
            break;
        }
        case OBJ_CLOSURE: {
            obj_closure *closure = (obj_closure *) object;
            FREE_ARRAY(vm, obj_upvalue *, closure->upvalues,
                                        closure->upvalue_count);
            FREE(vm, obj_closure, object);
            break;
        }
        case OBJ_FUNCTION: {
            obj_function *function = (obj_function *)object;
            free_chunk(vm, &function->chunk);
            FREE(vm, obj_function, object);
            break;
        }
        case OBJ_INSTANCE: {
            obj_instance *instance = (obj_instance *)object;
            free_table(vm, &instance->fields);
            FREE(vm, obj_instance, object);
            break;
        }
        case OBJ_NATIVE: {
            FREE(vm, obj_native, object);
            break;
        }
        case OBJ_LIB: {
            obj_lib *lib = (obj_lib *) object;
            free_table(vm, &lib->fields);
            FREE(vm, obj_lib, object);
            break;
        }
        case OBJ_STRING: {
            obj_string *string = (obj_string *)object;
            FREE_ARRAY(vm, char, string->chars, string->length + 1);
            FREE(vm, obj_string, object);
            break;
        }
        case OBJ_UPVALUE:
            FREE(vm, obj_upvalue, object);
            break;
        case OBJ_CUSTOM: {
            obj_custom_field *cus = (obj_custom_field *)object;
            if (cus->destroy)
                cus->destroy(cus->field);
            FREE(vm, obj_custom_field, object);
            break;
        }
    }
}

void free_objects(vm_t *vm)
{
    obj_t *object = vm->objects;
    while (object != NULL) {
        obj_t *next = object->next;
        free_object(vm, object);
        object = next;
    }
    free(vm->gray_stack);
}

void mark_object(vm_t *vm, obj_t *object)
{
    if (object == NULL || object->is_marked)
        return;
#ifdef DEBUG_LOG_GC
    printf("%p mark ", (void*)object);
    print_value(OBJ_VAL(object));
    printf("\n");
#endif
    object->is_marked = true;

    if (vm->gray_capacity < vm->gray_count + 1) {
        vm->gray_capacity = GROW_CAPACITY(vm->gray_capacity);
        vm->gray_stack = realloc(vm->gray_stack,
                        sizeof(obj_t *) * vm->gray_capacity);
    }
    vm->gray_stack[vm->gray_count++] = object;
}

void mark_value(vm_t *vm, value value)
{
    if (!IS_OBJ(value))
        return;
    mark_object(vm, AS_OBJ(value));
}

static void mark_roots(vm_t *vm, parser_t *parser) {
    for (value *slot = vm->stack; slot < vm->stack_top; slot++) {
        mark_value(vm, *slot);
    }
    for (int i = 0; i < vm->frame_count; i++) {
        mark_object(vm, (obj_t *)vm->frames[i].closure);
    }
    for (obj_upvalue *upvalue = vm->open_upvalues;
        upvalue != NULL; upvalue = upvalue->next) {
        mark_object(vm, (obj_t *)upvalue);
    }
    mark_table(vm, &vm->globals);
    mark_compiler_roots(vm, parser->comp);
    mark_object(vm, (obj_t *) vm->init_string);
}

void mark_compiler_roots(vm_t *vm, compiler *comp) {
    while (comp != NULL) {
        mark_object(vm, (obj_t *)comp->function);
        comp = comp->enclosing;
    }
}

static void sweep(vm_t *vm) {
    obj_t *previous = NULL;
    obj_t *object = vm->objects;
    while (object != NULL) {
        if (object->is_marked) {
            object->is_marked = false;
            previous = object;
            object = object->next;
        } else {
            obj_t *unreached = object;
            object = object->next;
            if (previous != NULL) {
                previous->next = object;
            } else {
                vm->objects = object;
            }
                free_object(vm, unreached);
        }
    }
}

void collect_garbage(vm_t *vm)
{
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
    size_t before = vm->bytes_allocated;
#endif
    mark_roots(vm, vm->parser);
    trace_references(vm);
    table_remove_white(&vm->strings);
    sweep(vm);
    vm->next_gc = vm->bytes_allocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
    printf("-- gc end\n");
    printf("   collected %ld bytes (from %ld to %ld) next at %ld\n",
        before - vm->bytes_allocated, before, vm->bytes_allocated,
        vm->next_gc);
#endif
}

static void mark_array(vm_t *vm, value_array *array) {
    for (int i = 0; i < array->count; i++) {
        mark_value(vm, array->values[i]);
    }
}

static void blacken_object(vm_t *vm, obj_t *object) {
#ifdef DEBUG_LOG_GC
    printf("%p blacken ", (void *)object);
    print_value(OBJ_VAL(object));
    printf("\n");
#endif
    switch (object->type) {
        case OBJ_BOUND_METHOD: {
            obj_bound_method *bound = (obj_bound_method *)object;
            mark_value(vm, bound->receiver);
            mark_object(vm, (obj_t *)bound->method);
            break;
        }
        case OBJ_CLASS: {
            obj_class *klass = (obj_class *)object;
            mark_object(vm, (obj_t *)klass->name);
            mark_table(vm, &klass->methods);
            break;
        }
        case OBJ_CLOSURE: {
            obj_closure *closure = (obj_closure *)object;
            mark_object(vm, (obj_t *)closure->function);
            for (int i = 0; i < closure->upvalue_count; i++) {
                mark_object(vm, (obj_t *)closure->upvalues[i]);
            }
            break;
        }
        case OBJ_FUNCTION: {
            obj_function *function = (obj_function *)object;
            mark_object(vm, (obj_t *)function->name);
            mark_array(vm, &function->chunk.constants);
            break;
        }
        case OBJ_INSTANCE: {
            obj_instance *instance = (obj_instance *)object;
            mark_object(vm, (obj_t *)instance->klass);
            mark_table(vm, &instance->fields);
            break;
        }
        case OBJ_UPVALUE:
            mark_value(vm, ((obj_upvalue *) object)->closed);
            break;
        case OBJ_NATIVE:
        case OBJ_STRING:
        case OBJ_CUSTOM:
        case OBJ_LIB:
        break;
    }
}

void trace_references(vm_t *vm) {
    while (vm->gray_count > 0) {
        obj_t *object = vm->gray_stack[--vm->gray_count];
        blacken_object(vm, object);
    }
}