#include <MemoryManager.h>

STATUS_REGISTER_STRUCTURE *STATUS_REGISTERS;
COMMAND_REGISTER_STRUCTURE *COMMAND_REGISTERS;
DATA_REGISTER_STRUCTURE *DATA_REGISTERS;

Buffer& dataBank = Buffer::getInstance(
    IO_MEMEROY_BASE_ADDRESS,
    COMMAND_MEMEROY_BASE_ADDRESS,
    STATUS_MEMEROY_BASE_ADDRESS,
    DATA_MEMEROY_BASE_ADDRESS,
    EEPROM_MEMEROY_BASE_ADDRESS,

    IO_MEMEROY_SIZE,
    COMMAND_MEMEROY_SIZE,
    STATUS_MEMEROY_SIZE,
    DATA_MEMEROY_SIZE,
    EEROM_MEMEROY_SIZE
);
AccessScope& current_context = AccessScope::getInstance();

void initializeDataBank() {
    COMMAND_REGISTERS = (COMMAND_REGISTER_STRUCTURE*) dataBank.getCommandBuffer();
    STATUS_REGISTERS = (STATUS_REGISTER_STRUCTURE*) dataBank.getStatusBuffer();
    DATA_REGISTERS = (DATA_REGISTER_STRUCTURE*) dataBank.getDataBuffer();

    current_context.invalidateContext();
}

// I2C Frame Context Interface
bool setCurrentContext(const uint16_t& address, 
        const uint8_t& length, 
        const MEMORY_OPERATION_MODE& mode) {
    
    current_context.initContext(address,length,mode);
    dataBank.setTheBlockType(current_context);
    
    if (!current_context.isValid()) {
        current_context.invalidateContext();
        return false;
    }
    
    return true;
}

bool isCtxValid(){
    return current_context.isValid();
}

uint8_t getCtxDataLength(){
    return current_context.getLength();
}

MEMORY_BLOCK_TYPE getCtxBlockType(){
    return current_context.getBlockType();
}

MEMORY_OPERATION_MODE getCtxOpMode(){
    return current_context.getMode();
}

uint8_t* getCtxDataBuffer(){
    return current_context.getBuffer();
}

void invalidateCtx(){
    current_context.invalidateContext();
}

void writeToRegisterBank(const uint8_t &value){
    dataBank.writeByte(current_context, value);
}

void readFromRegisterBank(uint8_t &value){
    dataBank.readByte(current_context, value);
}