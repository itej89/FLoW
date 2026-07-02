#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <stdint.h>

// Programmer Request Commands
#define PRG_INT_BEGIN           0x01
#define PRG_INT_SET_PAGE        0x02
#define PRG_INT_WRITE_PAGE      0x03
#define PRG_INT_EXIT            0x05

// Programmer Response Commands
#define PRG_INT_BEGIN_ACK       0xAA
#define PRG_INT_SET_PAGE_ACK    0xAB
#define PRG_INT_WRITE_PAGE_ACK  0xAC
#define PRG_INT_EXIT_ACK        0xEE


// Programmer States
enum PRG_STATES {
    PRG_RESET,
    PRG_BEGIN,
    PRG_END,
    PRG_TIMEOUT
};
extern enum PRG_STATES PRG_STATE;
#define PRG_RETRY_COUNT 5

// Programmer Frame
struct PRG_FRAME {
    uint8_t Command;
    uint8_t length;
    uint8_t* data;
};

struct PRG_PAGE_DATA {
    uint32_t address;
    uint8_t length;
    uint8_t* data;
};


void prg_try_flashing(void);

#endif // PROGRAMMER_H