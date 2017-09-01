#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include<modbus.h>

#include "slave.h"

static Pool *my_pool;

int check_addr(int start, int length)
{
    if(start < 0 || start > POOL_SIZE)
        return -1;

    if(length < 0 || length > POOL_SIZE)
        return -1;

    if(start + length > POOL_SIZE)
        return -1;

    return 0;
}

Pool *pool_new(void)
{

    Pool *new_pool = (Pool *)malloc(sizeof(Pool));

    if(new_pool)
        memset(new_pool, 0, sizeof(Pool));

    return new_pool;
}

void pool_free(Pool *pool)
{
    if(pool)
        free(pool);
}

void pool_show_section(Pool *pool, int start, int length)
{
    if(!pool)
        return;

    if(check_addr(start, length))
        return;

    for(int index = 0; index < length; ++index) {
        printf("Coil %d : %d\t", start + index, pool->coil_pool[start+index]);
        printf("Input %d : %d\t", start + index, pool->input_pool[start+index]);
        printf("Holding %d : %d\t", start + index, pool->holding_pool[start+index]);
        printf("InputReg %d : %d\t", start + index, pool->inputreg_pool[start+index]);
        printf("\n");
    }
}

int pool_set_coil(Pool *pool, int start, int length, uint8_t value)
{
    if(!pool)
        return -1;

    if(check_addr(start, length))
        return -1;

    for(int index = 0; index < length; ++index)
        pool->coil_pool[start+index] = value;

    return 0;
}

int pool_set_input(Pool *pool, int start, int length, uint8_t value)
{
    if(!pool)
        return -1;

    if(check_addr(start, length))
        return -1;

    for(int index = 0; index < length; ++index)
        pool->input_pool[start+index] = value;
        
    return 0;
}

int pool_set_holding(Pool *pool, int start, int length, uint16_t value)
{
    if(!pool)
        return -1;

    if(check_addr(start, length))
        return -1;

    for(int index = 0; index < length; ++index)
        pool->holding_pool[start+index] = value;

    return 0;
}

int pool_set_inputreg(Pool *pool, int start, int length, uint16_t value)
{
    if(!pool)
        return -1;

    if(check_addr(start, length))
        return -1;

    for(int index = 0; index < length; ++index)
        pool->inputreg_pool[start+index] = value;

    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Call Help\n");
        exit(EXIT_FAILURE);
    }

    int opt = 0;

    while((opt = getopt(argc, argv, "p:")) != -1) {

        switch(opt) {
            case 'p':
                dprint("Port = %d\n", atoi(optarg));
                break;
            default:
                fprintf(stderr, "Call Help\n");
                exit(EXIT_FAILURE);
        }
    }

    my_pool = pool_new();
    if(!my_pool)
        goto exit;

    pool_set_coil(my_pool, 0, 100, 1);
    pool_set_input(my_pool, 0, 100, 1);
    pool_set_holding(my_pool, 0, 100, 1);
    pool_set_inputreg(my_pool, 0, 100, 1);

    pool_show_section(my_pool, 30, 40);

exit:
    
    pool_free(my_pool);

    exit(EXIT_SUCCESS);
}
