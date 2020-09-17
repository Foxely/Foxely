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

static uint32_t hash_string(const char *key, int length)
{
    uint32_t hash = 2166136261u;

    for (int i = 0; i < length; i++) {
        hash ^= key[i];
        hash *= 16777619;
    }
    return hash;
}

static obj_string *allocate_string(vm_t *vm, char *chars,
                                int length, uint32_t hash)
{
    obj_string *string = ALLOCATE_OBJ(vm, obj_string, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    push(vm, OBJ_VAL(string));
    table_set(vm, &vm->strings, string, NIL_VAL);
    pop(vm);
    return string;
}

obj_string *copy_string(vm_t *vm, const char *chars, int length)
{
    uint32_t hash = hash_string(chars, length);
    obj_string *interned = table_find_string(&vm->strings, chars, length, hash);
    if (interned != NULL)
        return interned;
    char *heap_chars = ALLOCATE(vm, char, length + 1);
    my_memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(vm, heap_chars, length, hash);
}

obj_string *take_string(vm_t *vm, char *chars, int length)
{
    uint32_t hash = hash_string(chars, length);
    obj_string *interned = table_find_string(&vm->strings, chars, length, hash);

    if (interned != NULL) {
        FREE_ARRAY(vm, char, chars, length + 1);
        return interned;
    }
    return allocate_string(vm, chars, length, hash);
}