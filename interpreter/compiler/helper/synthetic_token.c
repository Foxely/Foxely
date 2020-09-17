/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdlib.h>
#include "compiler.h"
#include "vm.h"
#include "scanner.h"
#include "my.h"
#ifdef DEBUG_PRINT_CODE
#include "chunk_debug.h"
#include "my_debug.h"
#endif

token_t synthetic_token(const char *text)
{
    token_t token;
    token.start = text;
    token.length = my_strlen(text);
    return token;
}