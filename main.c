/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chunk.h"
#include "chunk_debug.h"
#include "vm.h"
#include "my.h"

static void repl(vm_t *vm)
{
    char line[1024];

    for (;;) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        interpret(vm, line);
    }
}

int32_t main(int32_t ac, char **av)
{
    vm_t vm;
    init_vm(&vm);
    if (ac == 1) {
        repl(&vm);
    } else if (ac == 2) {
        run_file(&vm, av[1]);
    } else {
        fsprintf(2, "Usage: casm [path]\n");
        return (64);
    }
    free_vm(&vm);
    return (0);
}
