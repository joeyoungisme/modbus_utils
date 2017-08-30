#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include<modbus.h>

int main(int argc, char *argv[])
{


    /*******************************
     *      offical sample 
     * ****************************/

    modbus_t *mb;
    //uint16_t tag_reg[32];
    uint8_t coil_data[10];


    //mb = modbus_new_tcp("192.168.10.2", 502);
    mb = modbus_new_tcp("192.168.10.100", 502);
    modbus_connect(mb);

    /* Read 5 registers from the address 0 */
    //modbus_read_registers(mb, 0, 5, tag_reg);

    for(int count = 0; count < 200; ++count) {
        
        /* Read 5 coil from the address 0 */
        memset(coil_data, 0, sizeof(uint8_t) * 10);
        modbus_read_bits(mb, 0, 10, coil_data);
 
        printf("-----------------------------\n");
        for(int index = 0; index < 10; ++index)
            printf("Coil %d = %d\n", index, coil_data[index]);

        printf("-----------------------------\n");
        sleep(2);
    }

    modbus_close(mb);
    modbus_free(mb);

    exit(EXIT_SUCCESS);
}
