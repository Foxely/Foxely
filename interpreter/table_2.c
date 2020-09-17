/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include "table.h"
#include "custom_memory.h"
#include "my.h"

void table_add_all(vm_t *vm, table_t *from, table_t *to)
{
    for (int i = 0; i <= from->capacity; i++) {
        entry_t *entry = &from->entries[i];
        if (entry->key != NULL) {
            table_set(vm, to, entry->key, entry->value);
        }
    }
}

bool table_get(table_t *table, obj_string *key, value *value)
{
    if (table->count == 0)
        return false;
    entry_t *entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return false;

    *value = entry->value;
    return true;
}

bool table_delete(table_t *table, obj_string *key)
{
    if (table->count == 0)
        return false;

    entry_t *entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return false;
    entry->key = NULL;
    entry->value = BOOL_VAL(true);
    return true;
}

obj_string *table_find_string(table_t *table, const char *chars,
                                    int length, uint32_t hash)
{
    if (table->count == 0)
        return NULL;
    uint32_t index = hash & table->capacity;

    for (;;) {
        entry_t *entry = &table->entries[index];

        if (entry->key == NULL) {
            if (IS_NIL(entry->value))
                return NULL;
        } else if (entry->key->length == length &&
                    entry->key->hash == hash &&
                    my_strncmp(entry->key->chars, chars, length) == 0) {
            return entry->key;
        }
        index = (index + 1) & table->capacity;
    }
}

void mark_table(vm_t *vm, table_t *table)
{
    for (int i = 0; i <= table->capacity; i++) {
        entry_t *entry = &table->entries[i];
        mark_object(vm, (obj_t *)entry->key);
        mark_value(vm, entry->value);
    }
}