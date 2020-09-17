/*
** EPITECH PROJECT, 2019
** smart_ptr
** File description:
** smart_ptr
*/

#define smart __attribute__((cleanup(sfree_stack)))

void *smalloc(size_t size, void (*dtor)(void *));
void sfree(void *ptr);

__attribute__ ((always_inline))
inline void sfree_stack(void *ptr) {
    sfree(*(void **) ptr);
}