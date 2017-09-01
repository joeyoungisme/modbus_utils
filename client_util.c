#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<modbus.h>

#include "client_util.h"

struct __mb_ent *modbus_entity_rtu_New(void) {
    struct __mb_ent *ent;

    ent = (struct __mb_ent *)malloc(sizeof(struct __mb_ent));
    memset(ent, 0, sizeof(struct __mb_ent)); ent->type = MB_RTU; /* modbus_t* modbus_new_rtu(const char *device, int baud, char parity,
                                    int data_bit, int stop_bit);
    */

    ent->mb = modbus_new_rtu(DEVICENAME, BAUDRATE, PARITY, DATABIT, STOPBIT);

    return ent;
}

struct __mb_ent *modbus_entity_tcp_New(void) {
    struct __mb_ent *ent;

    ent = (struct __mb_ent *)malloc(sizeof(struct __mb_ent));
    memset(ent, 0, sizeof(struct __mb_ent));

    ent->type = MB_TCP;

    /*
    modbus_t* modbus_new_tcp(const char *ip_address, int port);
    */
    ent->mb = modbus_new_tcp(SERVERIP, SERVERPORT);
    if(!ent->mb)
        perror("modbus new tcp");

    return ent;
}

void modbus_entity_free(struct __mb_ent *ent) {

    if(!ent || !ent->mb)    return;

    modbus_close(ent->mb);
    modbus_free(ent->mb);

    if(ent->coil_data)
        free(ent->coil_data);
    if(ent->input_data)
        free(ent->input_data);
    if(ent->holding_data)
        free(ent->holding_data);
    if(ent->inputreg_data)
        free(ent->inputreg_data);

    free(ent);
}

int main(int argc, char *argv[])
{
    const char message[] = 
        "Usage : %s [functions] [offset] [quantity]\n"
        "functions : \n"
        "   -c      coil\n"
        "   -i      input\n"
        "   -h      holding register\n"
        "   -r      input register\n"
        "offset : \n"
        "   -o      Reference Address (1~65535)\n"
        "quantity :\n"
        "   -l      length\n\n";

    int opt;

    int ref_addr = 0;
    int quantity = 0;

    unsigned char func_code = 0;

    while((opt = getopt(argc, argv, "cihro:l:")) != -1) {
        switch(opt) {
            case 'c':
                func_code |= MB_COIL;
                break;
            case 'i':
                func_code |= MB_INPUT;
                break;
            case 'h':
                func_code |= MB_HOLDING;
                break;
            case 'r':
                func_code |= MB_INPUTREG;
                break;
            case 'o':
                ref_addr = atoi(optarg);
                break;
            case 'l':
                quantity = atoi(optarg);
                break;
            default:
                printf(message, argv[0]);
        }
    }

    dprint("Function Code = %d\n", func_code);

    if(ref_addr < 0 || ref_addr > 65535) {
        printf(message, argv[0]);
        exit(EXIT_FAILURE);
    }

    if(quantity == 0 || (ref_addr + quantity) > 65535) {
        printf(message, argv[0]);
        exit(EXIT_FAILURE);
    }

    modbus_entity *ent = modbus_entity_tcp_New();
    if(!ent) {
        perror("entity tcp new");
        exit(EXIT_FAILURE);
    }

    dprint("New entity Success.... \n");

    if(modbus_connect(ent->mb) == -1) {
        perror("modbus connect");
        goto exit;
    }

    dprint("Modbus Connected!!\n");

    for(int index = 0; index < FUNCTION_AMOUNT; ++index) {
        switch(func_code & (0x01 << index)) {
            case MB_COIL:
                dprint("Function : Coil ...\n");
                ent->coil_data = (uint8_t *)malloc(sizeof(uint8_t) * quantity);
                memset(ent->coil_data, 0, sizeof(uint8_t) * quantity);
                ent->coil_res = modbus_read_bits(ent->mb, ref_addr, quantity, ent->coil_data);
                if(ent->coil_res == -1) 
                    perror("Coil Read");
                //else open & write & close
                break;
            case MB_INPUT:
                dprint("Function : Input .... \n");
                ent->input_data = (uint8_t *)malloc(sizeof(uint8_t) * quantity);
                memset(ent->input_data, 0, sizeof(uint8_t) * quantity);
                ent->input_res = modbus_read_input_bits(ent->mb, ref_addr, quantity, ent->input_data);
                if(ent->input_res == -1) {
                    perror("Input Read");
                    goto exit;
                }
                //else open & write & close
                break;
            case MB_HOLDING:
                dprint("Function : Holding .... \n");
                ent->holding_data = (uint16_t *)malloc(sizeof(uint16_t) * quantity);
                memset(ent->holding_data, 0, sizeof(uint16_t) * quantity);
                ent->holding_res = modbus_read_registers(ent->mb, ref_addr, quantity, ent->holding_data);
                if(ent->holding_res == -1) { 
                    perror("Holding Read");
                    goto exit;
                }
                //else open & write & close
                break;
            case MB_INPUTREG:
                dprint("Function : InputReg .... \n");
                ent->inputreg_data = (uint16_t *)malloc(sizeof(uint16_t) * quantity);
                memset(ent->inputreg_data, 0, sizeof(uint16_t) * quantity);
                ent->inputreg_res = modbus_read_input_registers(ent->mb, ref_addr, quantity, ent->inputreg_data);
                if(ent->inputreg_res == -1) {
                    perror("InputReg Read");
                    goto exit;
                }
                //else open & write & close
                break;
        }
    }

    for(int index = 0; index < quantity; ++index) {
        int curr_addr = ref_addr + index;
        if(ent->coil_data && ent->coil_res != -1)
            printf("Coil %d : %d\t", curr_addr, ent->coil_data[index]);
        if(ent->input_data && ent->input_res != -1)
            printf("Input %d : %d\t", curr_addr, ent->input_data[index]);
        if(ent->holding_data && ent->holding_res != -1)
            printf("Holding %d : %d\t", curr_addr, ent->holding_data[index]);
        if(ent->inputreg_data && ent->inputreg_res != -1)
            printf("InputReg %d : %d\t", curr_addr, ent->inputreg_data[index]);
        printf("\n");
    }

exit:
    //sleep(3);
    dprint("Goodbye!\n");

    modbus_entity_free(ent);

    /*
     * lib modbus api
     *
    int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest);
    int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest);
    int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);
    int modbus_read_input_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);
    int modbus_write_bit(modbus_t *ctx, int coil_addr, int status);
    int modbus_write_register(modbus_t *ctx, int reg_addr, int value);
    int modbus_write_bits(modbus_t *ctx, int addr, int nb, const uint8_t *data);
    int modbus_write_registers(modbus_t *ctx, int addr, int nb, const uint16_t *data);
    */

    exit(EXIT_SUCCESS);
}
