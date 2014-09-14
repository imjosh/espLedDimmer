#include <SoftwareSerial.h>
#define SSID "linksys"
#define PASS "ffffffffff" //your wep key here

#define DST_IP  "74.125.25.141" //appspot.google.com
#define HOST "yourappspotinstance.appspot.com"

SoftwareSerial dbgSerial(10, 11); // RX, TX
int const led = 9;
int const debugLed = 13;
char myChar;

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(debugLed, OUTPUT);

  //blink debugLed to indicate power up
  for (int i = 0; i < 15; i++)
  {
    digitalWrite(debugLed, HIGH);
    delay(50);
    digitalWrite(debugLed, LOW);
    delay(50);
  }

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.setTimeout(5000);

  //dbgSerial.begin(19200); //can't be faster than 19200 for softserial
  dbgSerial.begin(38400);  //38400 works fine for me

  dbgSerial.println("ESP8266 Demo");
  delay(100);

  //test if the module is ready
  Serial.println("AT+RST");
  if (Serial.find("ready"))
  {
    dbgSerial.println("Module is ready");
    delay(1000);
    //connect to the wifi
    boolean connected = false;
    for (int i = 0; i < 5; i++)
    {
      if (connectWiFi())
      {
        connected = true;
        break;
      }
    }
    if (!connected)
    {
      //die
      while (1);
    }

    delay(5000);
    //set the single connection mode
    Serial.println("AT+CIPMUX=0");
  }
  else
  {
    dbgSerial.println("Module didn't respond.");
    delay(100);

    //serial loop mode for diag
    while (1) {
      while (dbgSerial.available()) {
        myChar = dbgSerial.read();
        Serial.print(myChar);
        digitalWrite(13, HIGH);
        delay(50);
        digitalWrite(13, LOW);
        delay(50);
      }

      while (Serial.available()) {
        myChar = Serial.read();
        delay(25);
        dbgSerial.print(myChar);
      }
    }
  }
}

void loop()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial.println(cmd);
  dbgSerial.println(cmd);
  if (Serial.find("Error")) return;
  cmd = "GET /status HTTP/1.0\r\nHost: ";
  cmd += HOST;
  cmd += "\r\n\r\n";

  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if (Serial.find(">"))
  {
    // dbgSerial.print(">");
  }
  else
  {
    Serial.println("AT+CIPCLOSE");
    dbgSerial.println("connect timeout");
    delay(1000);
    return;
  }

  int  charCount = 0;
  String statusStr = "";
  Serial.print(cmd);

  if ( Serial.find("status: ")) {
    char c;
    while (Serial.available() == 0); //wait for data
    while (Serial.available())
    {
      c = Serial.read();
      if (charCount < 3)
        statusStr += c;
      else if (charCount > 99) //avoid reading noise forever just in case
        break;

      charCount++;
      delay(50); //wait for more data. fixme: can this be smaller?
    }

    if (charCount > 0)
    {
      //Adjust the LED brightness
      analogWrite(led, statusStr.toInt() );

      dbgSerial.print("status=");
      dbgSerial.println(statusStr);
    }
  }
  dbgSerial.println();
  dbgSerial.println("====");
  delay(1000);
}

boolean connectWiFi()
{
  Serial.println("AT+CWMODE=1");
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  dbgSerial.println(cmd);
  Serial.println(cmd);
  delay(2000);
  if (Serial.find("OK"))
  {
    dbgSerial.println("OK, Connected to WiFi.");
    return true;
  }
  else
  {
    dbgSerial.println("Can not connect to the WiFi.");
    return false;
  }
}

