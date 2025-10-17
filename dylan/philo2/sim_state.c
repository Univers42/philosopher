#include "philo.h"

int get_simulation_state(t_data *ctx)
{
    int state;

    pthread_mutex_lock(&ctx->alive_m);
    state = ctx->all_alive;
    pthread_mutex_unlock(&ctx->alive_m);
    return (state);
}

void set_simulation_state(t_data *ctx, int state)
{
    pthread_mutex_lock(&ctx->alive_m);
    ctx->all_alive = state;
    pthread_mutex_unlock(&ctx->alive_m);
}
