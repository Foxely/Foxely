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

void concatenate(vm_t *vm)
{
    obj_string *b = AS_STRING(peek(vm, 0));
    obj_string *a = AS_STRING(peek(vm, 1));

    int length = a->length + b->length;
    char *chars = ALLOCATE(vm, char, length + 1);
    my_memcpy(chars, a->chars, a->length);
    my_strcat(chars, b->chars);
    chars[length] = '\0';

    obj_string *result = take_string(vm, chars, length);
    pop(vm);
    pop(vm);
    push(vm, OBJ_VAL(result));
}

void concatenate_number(vm_t *vm)
{
    double b = IS_NUMBER(peek(vm, 0)) ? AS_NUMBER(peek(vm, 0)) : 0;
    b = IS_NUMBER(peek(vm, 1)) ? AS_NUMBER(peek(vm, 1)) : b;
    obj_string *a = IS_STRING(peek(vm, 0)) ? AS_STRING(peek(vm, 0)) : 0;
    a = IS_STRING(peek(vm, 1)) ? AS_STRING(peek(vm, 1)) : a;
    bool f_str = IS_NUMBER(peek(vm, 1)) ? false : true;

    int nb_string_length = snprintf(NULL, 0, "%.15g", b) + 1;
    int length = a->length + nb_string_length;
    char *chars = ALLOCATE(vm, char, length);
    if (f_str)
        snprintf(chars, length, "%s%.15g", a->chars, b);
    else
        snprintf(chars, length, "%.15g%s", b, a->chars);
    chars[length] = '\0';
    obj_string *result = take_string(vm, chars, length);
    pop(vm);
    pop(vm);
    push(vm, OBJ_VAL(result));
}