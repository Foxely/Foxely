/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include "table.h"
#include "custom_memory.h"
#include "my.h"

void table_remove_white(table_t *table)
{
    for (int i = 0; i <= table->capacity; i++) {
        entry_t *entry = &table->entries[i];
        if (entry->key != NULL && !entry->key->obj.is_marked) {
            table_delete(table, entry->key);
        }
    }
}