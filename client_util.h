#ifndef CLIENT_UTIL_H
#define CLIENT_UTIL_H

#define DEVICENAME              "ttyUSB0"
#define BAUDRATE                38400
#define PARITY                  0
#define DATABIT                 8
#define STOPBIT                 1

#define SERVERIP                "192.168.10.2"
#define SERVERPORT              502

#define MB_COIL                 0x01
#define MB_INPUT                0x02
#define MB_HOLDING              0x04
#define MB_INPUTREG             0x08
#define FUNCTION_AMOUNT         4

typedef enum {
    MB_RTU,
    MB_TCP,
}MB_TYPE;

typedef struct __mb_ent {
    modbus_t *mb;
    MB_TYPE type;

    uint8_t *coil_data;
    uint8_t *input_data;
    uint16_t *holding_data;
    uint16_t *inputreg_data;

}modbus_entity;


struct __mb_ent *modbus_entity_rtu_New(void);
struct __mb_ent *modbus_entity_tcp_New(void);
void modbus_entity_free(struct __mb_ent *);

#endif
