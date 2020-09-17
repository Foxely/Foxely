/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <time.h>
#include <math.h>
#include "value.h"
#include "object.h"
#include "my.h"

typedef void (*callback_t)     (bool a);

value clock_native(vm_t *vm, int arg_count, value *args)
{
    (void) vm;
    (void) arg_count;
    (void) args;
    return NUMBER_VAL((double) clock() / CLOCKS_PER_SEC);
}

value sin_native(vm_t *vm, int arg_count, value *args)
{
    (void) vm;
    (void) arg_count;
    return NUMBER_VAL(sin(AS_NUMBER(args[1])));
}

value cos_native(vm_t *vm, int arg_count, value *args)
{
    (void) vm;
    (void) arg_count;
    return NUMBER_VAL(cos(AS_NUMBER(args[1])));
}

value readfile_native(vm_t *vm, int ac, value *args)
{
    (void) ac;
    char *name = AS_CSTRING(args[0]);
    char *content = readfile(name);
    if (!content)
        return (NIL_VAL);
    obj_string *cont = copy_string(vm, content, my_strlen(content));
    free(content);
    return (OBJ_VAL(cont));
}

value writefile_native(vm_t *vm, int ac, value *args)
{
    (void) vm;
    (void) ac;
    char *name = AS_CSTRING(args[0]);
    char *content = AS_CSTRING(args[1]);
    bool err = false;
    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd != -1) {
        write(fd, content, my_strlen(content));
    } else {
        err = true;
    }
    return (BOOL_VAL(err));
}