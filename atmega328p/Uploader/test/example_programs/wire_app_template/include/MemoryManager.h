#pragma once

#include <Buffer.h>

using namespace memory;

#define SET 1
#define CLEAR 0

extern Buffer& dataBank;
enum TOGGLE_SWITCH_STATES { BUTTON_OPEN = 0, SINGLE_PRESS, DOUBLE_PRESS, LONG_PRESS};

// ABSTRACT STRUCTURES MEMORY DATA ORGANIZATION
struct STATUS_REGISTER_STRUCTURE
{
    //0x01:STATUS BYTE
    unsigned int I2C_ERROR_STATE:8;         //Relative address from base: 0
};  
struct COMMAND_REGISTER_STRUCTURE
{
    //0x01:BOOTLOADER
    unsigned int REQ_BOOTLOADER:8;      //Relative address from base: 0
};

struct DATA_REGISTER_STRUCTURE
{
    unsigned int DATABYTE:8;            //Relative address from base: 0
};

extern STATUS_REGISTER_STRUCTURE *STATUS_REGISTERS;
extern COMMAND_REGISTER_STRUCTURE *COMMAND_REGISTERS;
extern DATA_REGISTER_STRUCTURE *DATA_REGISTERS;

// Given 11 Bytes of Address Space in I2C command 0x000 - 0x7FF
#define IO_MEMEROY_BASE_ADDRESS        0x0000
#define COMMAND_MEMEROY_BASE_ADDRESS   0x0100
#define STATUS_MEMEROY_BASE_ADDRESS    0x0101
#define DATA_MEMEROY_BASE_ADDRESS      0x0102
#define EEPROM_MEMEROY_BASE_ADDRESS    0x0103

#define IO_MEMEROY_SIZE        2303
#define COMMAND_MEMEROY_SIZE   1
#define STATUS_MEMEROY_SIZE    1
#define DATA_MEMEROY_SIZE      1
#define EEROM_MEMEROY_SIZE     1024



// STATUS REGISTER PRIMITIVES
//======================================================================
// Power-On Status Interface
#define STATUS_SET_DEVICE_ON (STATUS_REGISTERS->POWER_ON = SET)
#define STATUS_CLEAR_DEVICE_ON (STATUS_REGISTERS->POWER_ON = CLEAR)
#define STATUS_IS_DEVICE_ON (STATUS_REGISTERS->POWER_ON == SET)


// In-Button-Event Status Interface
#define STATUS_SET_IN_BTN_EVENT (STATUS_REGISTERS->IN_BTN_EVNT = SET)
#define STATUS_CLEAR_IN_BTN_EVENT (STATUS_REGISTERS->IN_BTN_EVNT =CLEAR)
#define STATUS_IS_IN_BTN_EVENT (STATUS_REGISTERS->IN_BTN_EVNT == SET)

// Attention Switch Pin interface
#define STATUS_SET_ATN_BTN_STATE(state) STATUS_REGISTERS->ATN_BTN_STATE=state
#define STATUS_ATN_BTN_STATE STATUS_REGISTERS->ATN_BTN_STATE

// Attention LED interface
#define STATUS_SET_ATN_LED_ON (STATUS_REGISTERS->ATN_LED_STATE = SET)
#define STATUS_SET_ATN_LED_OFF (STATUS_REGISTERS->ATN_LED_STATE = CLEAR)

// Power Switch Pin Interface
#define STATUS_SET_PWR_BTN_STATE(state) (STATUS_REGISTERS->PWR_BTN_STATE=state)
#define STATUS_PWR_BTN_STATE STATUS_REGISTERS->PWR_BTN_STATE

// Power LED interface
#define STATUS_SET_PWR_LED_ON (STATUS_REGISTERS->PWR_LED_STATE = SET)
#define STATUS_SET_PWR_LED_OFF (STATUS_REGISTERS->PWR_LED_STATE = CLEAR)

// I2C Frame Error Record
#define STATUS_SET_I2C_ERR_STATE(state) (STATUS_REGISTERS->I2C_ERROR_STATE=state)
#define STATUS_I2C_ERR_STATE STATUS_REGISTERS->I2C_ERROR_STATE


// COMMAND REGISTER PRIMITIVES
//======================================================================
// Bootloader request
#define CMD_REQ_BOOTLOADER ((COMMAND_REGISTERS->REQ_BOOTLOADER & 0x01) != 0x00)


void initializeDataBank();

// I2C Frame Context Interface
bool setCurrentContext(const uint16_t& address, 
        const uint8_t& length, 
        const MEMORY_OPERATION_MODE& mode);

bool isCtxValid();
uint8_t getCtxDataLength();
MEMORY_BLOCK_TYPE getCtxBlockType();
MEMORY_OPERATION_MODE getCtxOpMode();
uint8_t* getCtxDataBuffer();
void invalidateCtx();
void writeToRegisterBank(const uint8_t &value);
void readFromRegisterBank(uint8_t &value);