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

bool
verif_ext(const char *name)
{
    char *find = my_strchr((char *)name, '.');
    if (!find) {
        fsprintf(2, "Could not find the extension in '%s'.\n", name);
        return (false);
    }
    size_t len = find - name;
    char *new = malloc(sizeof(char) * (len + 1));
    bool res = false;

    my_memset(new, 0, len + 1);
    if (new) {
        my_strncpy(new, name, len);
        new = my_strcat_dup(new, my_strdup(".fox"));
    }
    if (!my_strcmp(name, new))
        res = true;
    else
        fsprintf(2, "'%s' is not a allow (only allow *.fox)\n", name);
    free(new);
    return (res);
}

char *readfile(const char *path)
{
    int fd = fs_open_file(path, "r");
    char *res = NULL;

    if (fd == -1) {
        fsprintf(2, "Could not open file \"%s\".\n", path);
        return (res);
    }
    if (!verif_ext(path))
        return (NULL);
    lseek(fd, 0, SEEK_END);
    size_t size = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);
    res = malloc(sizeof(char) * (size + 1));
    if (!res) {
        fsprintf(2, "Not enough memory to read \"%s\".\n", path);
        return (0);
    }
    size_t bytes = read(fd, res, size);
    bytes < size ? free(res) : 0;
    if (bytes < size) {
        fsprintf(2, "Could not read file \"%s\".\n", path);
        return (0);
    }
    res[size] = 0;
    close(fd);
    return (res);
}

interpret_result run_file(vm_t *vm, const char *path)
{
    char *source = readfile(path);
    if (!source)
        return (INTERPRET_COMPILE_ERROR);
    interpret_result result = interpret(vm, source);
    free(source);

    // if (result == INTERPRET_COMPILE_ERROR)
    //     exit(65);
    // if (result == INTERPRET_RUNTIME_ERROR)
    //     exit(70);
    return (result);
}