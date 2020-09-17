/*
** EPITECH PROJECT, 2019
** my_event
** File description:
** Header Event Container
*/

#ifndef _MY_EVENT_
#define _MY_EVENT_

typedef struct listener_t
{
    char *name;
    void *variable;
    void (*handle)(core_t *, void *);
    void (*handle2)(void *, void *);
} listener_t;

typedef struct listener_l
{
    listener_t array[100];
    int count;
} listener_l;

#endif