#define _GNU_SOURCE
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include<modbus.h>
#include<pthread.h>

#include "slave_tcp.h"

#define MAPPING_SIZE            1024

typedef enum {
    COIL = 0,
    INPUT,
    HOLDING,
    INPUTREG
}FUNC_CODE;

void *thread_change_data(void *arg)
{
    modbus_mapping_t *map = (modbus_mapping_t *)arg;

    char msg[] = 
        " (0) Coil\n (1) Input\n (2) Holding Register\n (3) Input Register\n (4) Quit\n";

    char input_buf[256];

    while(1) {

        printf("%s", msg);
        printf("Select Function Code : ");
        int num = 0;
        if(fgets(input_buf, sizeof(input_buf), stdin)) {
            if(1 != sscanf(input_buf, "%d", &num)) {
                perror("select func sscanf");
                continue;
            }
        }
        dprint("Select Func %d\n", num);

        if(num == 4)
            pthread_exit(NULL);

        if(num < 0 || num > 3) {
            printf("Invaild Function Code !\n");
            continue;
        }

        int start = 0;
        printf("Start Address : ");
        if(fgets(input_buf, sizeof(input_buf), stdin)) {
            if(1 != sscanf(input_buf, "%d", &start)) {
                perror("set start sscanf");
                continue;
            }
        }
        dprint("Start Addr %d\n", start);

        int quantity = 0;
        printf("Length : ");
        if(fgets(input_buf, sizeof(input_buf), stdin)) {
            if(1 != sscanf(input_buf, "%d", &quantity)) {
                perror("set length sscanf");
                continue;
            }
        }
        dprint("Quantity : %d\n", quantity);

        unsigned char addr_err = 0;
        if(start > MAPPING_SIZE || start < 0)
            addr_err = 1;
        if(quantity > MAPPING_SIZE || quantity < 0)
            addr_err = 1;
        if(start + quantity > MAPPING_SIZE)
            addr_err = 1;

        if(addr_err) {
            printf("Invaild Address!\n");
            continue;
        }

        int set_value;
        printf("Set Value : ");
        if(fgets(input_buf, sizeof(input_buf), stdin)) {
            int res = sscanf(input_buf, "%d", &set_value);
            if(res != 1) {
                if(res == 0)
                    fprintf(stderr, "Invaild Value!\n");
                else
                    perror("set value sscanf");
                continue;
            }
        }
        dprint("Value : %d\n", set_value);

        for(int index = 0; index < quantity; ++index) {
            switch(num) {
                case 0:
                    dprint("Address %d -> %d\n", start+index, map->tab_bits[start + index]);
                    break;
                case 1:
                    dprint("Address %d -> %d\n", start+index, map->tab_input_bits[start + index]);
                    break;
                case 2:
                    dprint("Address %d -> %d\n", start+index, map->tab_registers[start + index]);
                    break;
                case 3:
                    dprint("Address %d -> %d\n", start+index, map->tab_input_registers[start + index]);
                    break;
            }
        }
        dprint("Function : %d\n", num);
        dprint("Start : %d, Length : %d\n", start, quantity);
        dprint("Set Value : %d\n", set_value);

        for(int index = 0; index < quantity; ++index) {
            switch(num) {
                case 0:
                    map->tab_bits[start + index] = set_value;
                    break;
                case 1:
                    map->tab_input_bits[start + index] = set_value;
                    break;
                case 2:
                    map->tab_registers[start + index] = set_value;
                    break;
                case 3:
                    map->tab_input_registers[start + index] = set_value;
                    break;
            }
        }

    }
       

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    modbus_t *mb = modbus_new_tcp("192.168.10.101", 502);
    if(!mb) {
        perror("modbus tcp new");
        exit(EXIT_FAILURE);
    }
    dprint("modbus new tcp success!\n");

    int sockfd = modbus_tcp_listen(mb, 10);
    if(sockfd < 0) {
        perror("modbus tcp listen");
        goto mbexit;
    }
    dprint("modbus tcp listen Success!\n");

    modbus_tcp_accept(mb, &sockfd);

    modbus_mapping_t *map = modbus_mapping_new(MAPPING_SIZE, MAPPING_SIZE, MAPPING_SIZE, MAPPING_SIZE);
    if(!map)    goto tcpexit;
    dprint("mapping new success!\n");

    pthread_t thread_change;
    if(pthread_create(&thread_change, NULL, thread_change_data, (void *)map)) {
        perror("pthread create error");
        goto mapexit;
    }
    dprint("Create Thread Success!\n");

    while(1) {
        uint8_t request[MODBUS_TCP_MAX_ADU_LENGTH];

        int rc = modbus_receive(mb, request);
        if(rc > 0)
            modbus_reply(mb, request, rc, map);
        if(rc < 0) goto mapexit;

        if(!pthread_tryjoin_np(thread_change, NULL)) {
            dprint("Thread Terminate!\n");
            break;
        }
    }

mapexit:
    modbus_mapping_free(map);
tcpexit:
    close(sockfd);
mbexit:
    modbus_close(mb);
    modbus_free(mb);

    return 0;
}

