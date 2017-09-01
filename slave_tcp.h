#ifndef SLAVE_TCP_H
#define SLAVE_TCP_H

#ifdef DEBUG_MSG
#define MYDEBUG                 1
#else
#define MYDEBUG                 0
#endif


#define dprint(fmt, ...) \
    do { if(MYDEBUG) printf(fmt, ##__VA_ARGS__); }while(0)









#endif
