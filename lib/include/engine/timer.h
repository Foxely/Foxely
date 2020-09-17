/*
** EPITECH PROJECT, 2019
** timer
** File description:
** timer container
*/

#ifndef __TIMER_H_
#define __TIMER_H_

typedef struct s_timer
{
    float timer;
    float delay;
    float start;
    int increase;
    int finish;
} timerr_t;

timerr_t *timer(void);
float timer_start(timerr_t *timer, float start, float end, int increase);
int timer_update(timerr_t *timer, float delta);
void timer_reset(timerr_t *timer);

#endif