#include <Arduino.h>
#include <MemoryManager.h>
#include <I2CEventManager.h>
// #include <PeriodicEventHandler.h>

#include <DebugPrintManager.h>

void setup() {
  DDRB |= (1 << PB5); 

  initializeDebugPrint();

  DbgPrint->println("Application Started.");
  initializeDataBank();

  initializeCommunications();

  // initializePeriodicEvents();
}

void loop() {
  delay(500);

   // Turn LED on (set Pin B0 high)
        PORTB |= (1 << PB5);
        _delay_ms(100); // Delay for 500 milliseconds

        // Turn LED off (set Pin B0 low)
        PORTB &= ~(1 << PB5);
        _delay_ms(100); // Delay for 500 milliseconds
  // HandlePeriodicEvents();
}


