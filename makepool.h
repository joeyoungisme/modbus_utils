#ifndef SLAVE_H
#define DLAVE_H

#ifdef DEBUG_MSG
#define MYDEBUG                 1
#else
#define MYDEBUG                 0
#endif

#define dprint(fmt, ...) \
    do { if (MYDEBUG) printf(fmt, ##__VA_ARGS__); }while(0)

#define POOL_SIZE               1024


typedef struct __Pool {
    uint8_t coil_pool[POOL_SIZE];
    uint8_t input_pool[POOL_SIZE];

    uint16_t holding_pool[POOL_SIZE];
    uint16_t inputreg_pool[POOL_SIZE];
}Pool;



#endif
