/*
** EPITECH PROJECT, 2019
** csfml
** File description:
** csfml library
*/

#include "object.h"
#include <SFML/Graphics.h>

value create_window(vm_t *vm, int arg_count, value *args)
{
    if (arg_count != 3 || !IS_STRING(args[1]))
        return (NIL_VAL);
    printf("Create a new Window\n");
    sfVideoMode video = {1920, 1080, 32};
    sfRenderWindow *win = sfRenderWindow_create(video, AS_CSTRING(args[1]),
                                                sfDefaultStyle, NULL);
    obj_custom_field *window = 0;
    window = new_custom_field(vm, "sfRenderWindow",
                            win, sfRenderWindow_destroy);
    return (OBJ_VAL(window));
}

value is_open_window(vm_t *vm, int arg_count, value *args)
{
    if (!IS_CUSTOM(args[0]) || arg_count != 1) {
        return (BOOL_VAL(false));
    }
    printf("is open Window\n");
    bool res = sfRenderWindow_isOpen(AS_CUSTOM(args[0])->field);
    return (BOOL_VAL(res));
}

value clear_window(vm_t *vm, int arg_count, value *args)
{
    if (!IS_CUSTOM(args[0]) || arg_count != 1)
        return (NIL_VAL);
    sfRenderWindow_clear(AS_CUSTOM(args[0])->field, sfBlack);
    printf("clear Window\n");
    return (NIL_VAL);
}

value display_window(vm_t *vm, int arg_count, value *args)
{
    if (!IS_CUSTOM(args[0]) || arg_count != 1)
        return (NIL_VAL);
    sfRenderWindow_display(AS_CUSTOM(args[0])->field);
    printf("display Window\n");
    return (NIL_VAL);
}

value poll_window(vm_t *vm, int arg_count, value *args)
{
    if (!IS_CUSTOM(args[0]) || !IS_INSTANCE(args[1]) || arg_count != 2)
        return (BOOL_VAL(false));
    obj_instance *event = AS_INSTANCE(args[1]);
    value val;
    if (!table_get(&event->fields, copy_string(vm, "event", 5), &val)) {
        printf("Can't find the event property in ");
        print_object(OBJ_VAL(event));
        printf(".\n");
        return(BOOL_VAL(false));
    }
    if (IS_CUSTOM(val)) {
        bool res = sfRenderWindow_pollEvent(AS_CUSTOM(args[0])->field,
                                            AS_CUSTOM(val)->field);
        printf("poll event Window\n");
        return (BOOL_VAL(res));
    }
    printf("This is not an Event Class.\n");
    return (NIL_VAL);
}

static const lib_reg mylib [] = {
    {"create", create_window},
    {"isOpen", is_open_window},
    {"clear", clear_window},
    {"display", display_window},
    {"poll", poll_window},
    {NULL, NULL}
};

int luaopen_mylib (vm_t *vm)
{
    define_lib(vm, "sfml", mylib);
    return 1;
}
