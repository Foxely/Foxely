/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include "table.h"
#include "custom_memory.h"

void init_table(table_t *table)
{
    table->count = 0;
    table->capacity = -1;
    table->entries = 0;
}

void free_table(vm_t *vm, table_t *table)
{
    FREE_ARRAY(vm, entry_t, table->entries, table->capacity + 1);
    init_table(table);
}

entry_t *find_entry(entry_t *entries, int capacity, obj_string *key)
{
    uint32_t index = key->hash & capacity;
    entry_t *tombstone = NULL;

    for (;;) {
        entry_t *entry = &entries[index];

        if (entry->key == NULL) {
            if (IS_NIL(entry->value))
                return tombstone != NULL ? tombstone : entry;
            else
                if (tombstone == NULL) tombstone = entry;
        } else if (entry->key == key)
            return entry;
        index = (index + 1) & capacity;
    }
}

static void adjust_capacity(vm_t *vm, table_t *table, int capacity)
{
    entry_t *entries = ALLOCATE(vm, entry_t, capacity + 1);
    for (int i = 0; i <= capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NIL_VAL;
    }
    table->count = 0;
    for (int i = 0; i <= table->capacity; i++) {
        entry_t *entry = &table->entries[i];
        if (entry->key == NULL)
            continue;
        entry_t *dest = find_entry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }
    FREE_ARRAY(vm, entry_t, table->entries, table->capacity + 1);
    table->entries = entries;
    table->capacity = capacity;
}

bool table_set(vm_t *vm, table_t *table, obj_string *key, value value)
{
    if (table->count + 1 > (table->capacity + 1) * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity + 1) - 1;
        adjust_capacity(vm, table, capacity);
    }

    entry_t *entry = find_entry(table->entries, table->capacity, key);
    bool is_new_key = entry->key == NULL;
    if (is_new_key && IS_NIL(entry->value))
        table->count++;

    entry->key = key;
    entry->value = value;
    return is_new_key;
}