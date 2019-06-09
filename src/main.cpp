// -------------------------------------------------------------
// Honda Civic CAN bus sniffer
// Sends command to garage door opener
// on receiving CANCEL command for Cruise Control

#include <FlexCAN.h>

int led_pin = 13;
int door_sw = 22;

FlexCAN CANbus( 500000 );
static CAN_message_t msg;
static uint8_t hex[17] = "0123456789ABCDEF";

unsigned long key_press_time = 0;
unsigned long current_time = 0;
boolean switch_enabled = false;

// function declarations
void enableSwitch();
void disableSwitch();

// -------------------------------------------------------------
static void hexDump(uint8_t dumpLen, uint8_t *bytePtr)
{
  while( dumpLen-- )
  {
    uint8_t working = *bytePtr++;
    Serial.write( hex[ working>>4 ] );
    Serial.write( hex[ working&15 ] );
    Serial.write(' ');
  }
  Serial.write('\r');
  Serial.write('\n');
}

// -------------------------------------------------------------
void setup(void)
{
  CANbus.begin( );
  pinMode(led_pin, OUTPUT);
  pinMode(door_sw, OUTPUT);
  digitalWrite(led_pin, 0);
  digitalWrite(door_sw, 0);

  delay(1000);
  Serial.println(F("Started!"));

}


// -------------------------------------------------------------
void loop(void)
{
  while ( CANbus.read(msg) )
  {
    if( ( msg.id == 0x0164 ) && ( ( msg.buf[0] & 0xC0 ) == 0xC0 ) )
    {
      Serial.print( msg.id, HEX );
      Serial.print(' ');
      hexDump( msg.len, msg.buf );
      
      enableSwitch( );      
    }
  }

  if( switch_enabled )
  {
    current_time = millis( );
  
    if( current_time < key_press_time )
    {
      key_press_time = 0;
    }
    
    if( ( current_time - key_press_time ) > 200 )
    {
      disableSwitch( );
    }
  }
}

void enableSwitch( )
{
  digitalWrite( led_pin, 1 );
  digitalWrite( door_sw, 1 );
  key_press_time = millis( );
  switch_enabled = true;
}

void disableSwitch( )
{
  digitalWrite( led_pin, 0 );
  digitalWrite( door_sw, 0 );
  switch_enabled = false;
}

