/*
Read Voltcraft multimeter data via RS232 von R. Pleger für ct Hacks
Digital in: Arduino digitalpin 6;
*/

//baud rate
#define bit2400Delay 396
#define halfBit2400Delay 198

const byte rx = 6;
byte SWval;
const int IARRAY =14; //Voltcraft sends 14 bytes per message
byte d[IARRAY]; //14 byte data message
 

void setup() {
  pinMode(rx, INPUT);
  Serial.begin(9600);
  Serial.println("start");
}

void loop()
{
    byte i;
    String strout = String("");
    //wait for start byte 01nn and read IARRAY bytes
    SWval=SWread();
    if ((SWval & 0xf0)== 0x10){ 
      // MSB = 01, so assume valid bytes will follow 
      d[0]=SWval;
      for (i=1; i<IARRAY; ++i){
        d[i]=SWread();
      }
      strout= String(getnumber() + getsymbols());
      Serial.println(strout);
    }
}

String lookupnumber(byte d){
 // translate byte code into number
 // unknown code (eg "overflow") translated into "L"
        switch(d){
          case 0x7d: return "0";
          case 0x05: return "1";
          case 0x5b: return "2";
          case 0x1f: return "3";
          case 0x27: return "4";
          case 0x3e: return "5";
          case 0x7e: return "6";
          case 0x15: return "7";
          case 0x7f: return "8";
          case 0x3f: return "9";
          case 0x00: return " ";
          default: return "L";
        }  
}

String getsymbols(){
  //translate bit sequence into string of symbols
  const String symb[]= {"AC ", "DC ", "Auto ", "RS232 ", "u", "n", "k", "Diode ", "m", "% ", "M", "Beep ", "F ", "Ohm ", "Delta ", "Hold ", "A ", "V ", "Hz ", "Batt ", "", "", "", "C "};
  //                     0      1      2        3 RS232   4    5    6    7         8    9     10    11      12     13      14       15       16    17     18    19      20   21  22  23                 
  String strsymbols= String("");
  unsigned long int nd;
  int i;
  
  nd = 0;
  nd = d[0]&0x0f;
  for (i=0; i<5; ++i){
    nd = nd <<4;
    nd |= (d[i+9]&0x0f);
  }
  for (i=23; i>=0; --i){
    if (nd & 0x01){
      strsymbols = String(symb[i] + strsymbols);
    }
    nd = nd >>1;
  }  
  strsymbols = String( " " + strsymbols);
  return strsymbols;
}

String getnumber(){
  String strsymbols= String("");
  byte nd[4]; //4 digits
  int i;
  for (i=0; i<4; ++i){ // get numbers
    nd[i]=(d[2*i+1]<<4)&0xf0;
    if (nd[i]& 0x80){
       if (i==0)
         strsymbols = String(strsymbols +"-");
       else
         strsymbols = String(strsymbols +".");
    }  
    nd[i] &= 0x7f;
    nd[i] |= (d[2*i+2] & 0x0f);
    strsymbols = String(strsymbols + lookupnumber(nd[i]));
  }
  return strsymbols;
}

/*
int SWread()
 // original version see: http://arduino.cc/en/Tutorial/ArduinoSoftwareRS232
 // read digital input
 // inverted compared to RS232 definition, diode with pull-down
{
  byte val = 0;
  while (!digitalRead(rx));
  //wait for start bit
  if (digitalRead(rx) == HIGH) {
    delayMicroseconds(halfBit2400Delay);
    for (int offset = 0; offset < 8; offset++) {
     delayMicroseconds(bit2400Delay);
     val |= (! digitalRead(rx)) << offset;
    }
    //wait for stop bit 
    delayMicroseconds(bit2400Delay); 
    //waiting for another stop bit does not matter,
    //RS232 transmits asychonly.
    //delayMicroseconds(bit2400Delay);
    return val;
  }
}
*/

int SWread()
 // original version from http://arduino.cc/en/Tutorial/ArduinoSoftwareRS232
 // read digital input
 // non-inverted 
{
  byte val = 0;
  while (digitalRead(rx));
  //wait for start bit
  if (digitalRead(rx) == LOW) {
    delayMicroseconds(halfBit2400Delay);
    for (int offset = 0; offset < 8; offset++) {
     delayMicroseconds(bit2400Delay);
     val |= digitalRead(rx) << offset;
    }
    //wait for stop bit + extra
    delayMicroseconds(bit2400Delay); 
    delayMicroseconds(bit2400Delay);
    return val;
  }
}
