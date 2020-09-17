/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#ifndef __TABLE_H__
#define __TABLE_H__

#include "value.h"
#include "common.h"

#define TABLE_MAX_LOAD (0.75)

typedef struct {
    obj_string *key;
    value value;
} entry_t;

typedef struct {
    int count;
    int capacity;
    entry_t *entries;
} table_t;

void init_table(table_t *table);
void free_table(vm_t *vm, table_t *table);
entry_t *find_entry(entry_t *entries, int capacity, obj_string *key);
bool table_set(vm_t *vm, table_t *table, obj_string *key, value value);
void table_add_all(vm_t *vm, table_t *from, table_t *to);
bool table_get(table_t *table, obj_string *key, value *value);
bool table_delete(table_t *table, obj_string *key);
obj_string *table_find_string(table_t *table, const char *chars,
                                    int length, uint32_t hash);
void mark_table(vm_t *vm, table_t *table);
void table_remove_white(table_t *table);

#endif