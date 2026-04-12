#include <I2CEventManager.h>

#include <I2C.h>
#include <MemoryManager.h>
#include <CommandRequestHandler.h>

#include <DebugPrintManager.h>

using namespace peripherals;

I2C* i2cInterface;

void initializeCommunications() {
  i2cInterface = &I2C::getInstance(
    I2C_ADDRESS, requestEvent,receiveEvent
  );
}


void requestEvent(){
  if(getCtxOpMode() == READ) {
      Wire.write(I2C_ACK_OK); // Write ack
      Wire.write(getCtxDataLength()); // Write return length
      uint8_t value;
      for (int i=0; (i<16 && isCtxValid()); i++) {
        readFromRegisterBank(value);
        Wire.write(value);
      }
  }
  else if (getCtxOpMode() == WRITE)
  {
      Wire.write(I2C_ACK_OK); // Write ack
      Wire.write(0x00); // Write return length
  }
    invalidateCtx(); 
}


long start_of_transmission = millis();
// Expects one complete frame at a time
void receiveEvent(int count){

    uint8_t checksum = I2C_CHKSUM_SEED;

    long end_of_last_transmission = millis();
    
    // Wait for atleast the Start Byte, Command Byte and Length Byte
    while((millis() - end_of_last_transmission) < I2C_COMMAND_HEADER_TIME_OUT_MS){

      if(Wire.available()<3) {
        delay(1); 
      }
      else {
        if (Wire.read()!=I2C_SOF) {
          STATUS_SET_I2C_ERR_STATE(I2C_CORRUPTED_DATA);
        }
        else {
          checksum ^= I2C_SOF;
          uint8_t header_high = Wire.read();
          checksum ^= header_high;
          uint8_t header_low = Wire.read();
          checksum ^= header_low;
          MEMORY_OPERATION_MODE mode = REG_OP_MODE(header_high);
          uint8_t length = DATA_LENGTH(header_high);
          uint16_t address = REG_ADDRESS(header_high, header_low);
          setCurrentContext(address, length, mode);
          break;
        }
      }
    }

    if (isCtxValid()) {
      uint8_t expected_data_length = getCtxDataLength()+2; // DATA_LENGTH and CHKSUM + EOF
      end_of_last_transmission = millis();
      while(Wire.available() < expected_data_length && millis()-end_of_last_transmission < I2C_COMMAND_TRANSMISSION_TIME_OUT_MS){
        delay(1); 
      }

      if (Wire.available() >= expected_data_length) {

        if(getCtxOpMode() == WRITE) {
          uint8_t* data_buffer = getCtxDataBuffer();
          for(int i=0; i< getCtxDataLength(); i++) {
            data_buffer[i] = Wire.read();
            checksum ^= data_buffer[i];
          }
        }

        uint8_t received_chksum = Wire.read();
        if(I2C_EOF == Wire.read() && received_chksum == checksum) {
          if(getCtxOpMode() == WRITE) {
            uint8_t* data_buffer = getCtxDataBuffer();
            for(int i=0; i< getCtxDataLength(); i++) {
              writeToRegisterBank(data_buffer[i]);
            }
            if(getCtxBlockType() == COMMAND){
                serveCommands();
            }
            return;
          }
        }
       
      }
  }

}
