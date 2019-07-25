

/*
  PINOUT:
  RC522 MODULE    Uno/Nano     MEGA
  SDA             D10          D9
  SCK             D13          D52
  MOSI            D11          D51
  MISO            D12          D50
  IRQ             N/A          N/A
  GND             GND          GND
  RST             D9           D8
  3.3V            3.3V         3.3V
*/
/* Include the standard Arduino SPI library */
/*
 error 1 : This error will show up when user have already entered premises
 error 2 : This error will show up when user is not registered on main door
 error 3 : This error shows up when user is already in one room and trying to enter other room
 error 4 : This error will show up when user try to exit room without entering the room 
 */
#include <SPI.h>
/* Include the RFID library */
//#include <RFID.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
//3,169,82,66,226
/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SS_PIN 9
#define RST_PIN 8
#define CARD1 "03 EE F3 1B"
#define KEY1 "52 9D 58 A5"
#define KEY2 "22 29 41 9B"
#define KEY3 "E2 4D 7D A5"
#define KEY4 "42 BD CA 39"
#define KEY5 "22 26 4F A5"
#define KEY6 "A9 F7 5B 63"
#define TOTALID 8
/* Create an instance of the RFID library */
//RFID RC522(SDA_DIO, RESET_DIO);
MFRC522 mfrc522(SS_PIN, RST_PIN);

String ID;
int myID[8] = {0,CARD1, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6};
//int userName[3]={'SANKET PANCHOLI', "BHOOMI PATEL", "SUMAIR DAGIYA"};
int userMainDoorFlag[TOTALID] = {0};        //This flag is used check wether user has registered on main door or not. 0:not registered, 1: registered
int userRoom1Flag[TOTALID] = {0};        //This flag is used check wether user has registered on Room1 or not. 0:not registered, 1: registered
int userRoom2Flag[TOTALID] = {0};        //This flag is used check wether user has registered on Room1 or not. 0:not registered, 1: registered
int userRoom3Flag[TOTALID] = {0};        //This flag is used check wether user has registered on Room1 or not. 0:not registered, 1: registered
int doorNo = 0, visitorFlag = 0;                       //This variable is used to identify door through switches
const int rs = 22, en = 24, d4 = 23, d5 = 25, d6 = 27, d7 = 29;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//void identifyDoor();

void setup()
{
  Serial.begin(9600);
  /* Enable the SPI interface */
  SPI.begin();
  lcd.begin(16, 2);
  /* Initialise the RFID reader */
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(13, INPUT);
  pinMode(12, INPUT);
  pinMode(11, INPUT);
  pinMode(2, OUTPUT); // Set buzzer pin to an Output pin
  digitalWrite(2, LOW);
  
}

void loop()
{
  unsigned int i=0, idNo=0;

  /* Has a card been detected? */
  if (mfrc522.PICC_IsNewCardPresent())
  {
    /* If so then get its serial number*/
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    } 
    Serial.println("Card detected:");
   // RC522.readCardSerial();
   // Serial.println("Card detected:");
    //Serial.print(RC522.serNum[0], DEC);
    //ID = RC522.serNum[0];
    //Serial.print(RC522.serNum[i],HEX); //to print card detail in Hexa Decimal format
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    Serial.println();

    ID = content.substring(1);
    
    identifyDoor();
    visitorIdCheck(ID);

    for(i=0;i<TOTALID;i++)
    {
      if(ID == myID[i])
      {
        idNo = i;
      }
    }

    if(idNo != 0)
    {
      switch(doorNo)
      {
        case 0:
          if(userMainDoorFlag[idNo] == 0)
          {
            identifyUser();
            accessGranted();
            userMainDoorFlag[idNo] = 1;
            delay(1500);
            break;
          }
          else
          {
            displayError1();
            break;
          }
         case 1:
          if(userMainDoorFlag[idNo] == 1)
          {
            Serial.print(userRoom1Flag[idNo], DEC);
            Serial.println();
            Serial.print(userRoom2Flag[idNo], DEC);
            Serial.println();
            Serial.print(userRoom3Flag[idNo], DEC);
            Serial.println();
            if(userRoom1Flag[idNo] == 0 && userRoom2Flag[idNo] == 0 && userRoom3Flag[idNo] == 0)
            {
              identifyUser();
              accessGranted();
              userMainDoorFlag[idNo] = 0;
              delay(1500);
              break;
            }
            else
            {
              displayError3(idNo);
              break;
            }
          }
          else
          {
            displayError2();
            break;
          }
         case 2:
          if(userMainDoorFlag[idNo] == 1)
          {
            if(userRoom1Flag[idNo] == 0 && userRoom2Flag[idNo] == 0 && userRoom3Flag[idNo] == 0 )
            {
              identifyUser();
              accessGranted();
              userRoom1Flag[idNo] = 1;
              delay(1500);
              break;
            }
            else
            {
              displayError3(idNo);
              break;
            }
          }
          else
          {
            displayError2();
            break;
          }
         case 3:
          if(userMainDoorFlag[idNo] == 1)
          {
            if(userRoom1Flag[idNo] == 1)
            {
              identifyUser();
              accessGranted();
              userRoom1Flag[idNo] = 0;
              delay(1500);
              break;
            }
            else
            {
              displayError4();
              break;
            }
          }
          else
          {
            displayError2();
            break;
          }
         case 4:
          if(userMainDoorFlag[idNo] == 1)
          {
            if(idNo == 6 || idNo == 7)
            {
              accessDenied();
              break;
            }
            if(userRoom1Flag[idNo] == 0 && userRoom2Flag[idNo] == 0 && userRoom3Flag[idNo] == 0 )
            {
              identifyUser();
              accessGranted();
              userRoom2Flag[idNo] = 1;
              delay(1500);
              break;
            }
            else
            {
              displayError3(idNo);
              break;
            }
          }
          else
          {
            displayError2();
            break;
          }
         case 5:
          if(userMainDoorFlag[idNo] == 1)
          {
            if(userRoom2Flag[idNo] == 1)
            {
              identifyUser();
              accessGranted();
              userRoom2Flag[idNo] = 0;
              delay(1500);
              break;
            }
            else
            {
              displayError4();
              break;
            }
          }
          else
          {
            displayError2();
            break;
          }
         case 6:
          if(userMainDoorFlag[idNo] == 1)
          {
            if(idNo == 1 || idNo == 2)
            {
              if(userRoom1Flag[idNo] == 0 && userRoom2Flag[idNo] == 0 && userRoom3Flag[idNo] == 0 )
              {
                identifyUser();
                accessGranted();
                userRoom3Flag[idNo] = 1;
                delay(1500);
                break;
              }
              else
              {
                displayError3(idNo);
                break;
              }
            }
            else
            {
              accessDenied();
              break;
            }
          }
          else
          {
            displayError2();
            break;
          }
         case 7:
          if(userMainDoorFlag[idNo] == 1)
          {
            if(userRoom3Flag[idNo] == 1)
            {
              identifyUser();
              accessGranted();
              userRoom3Flag[idNo] = 0;
              delay(1500);
              break;
            }
            else
            {
              displayError4();
              break;
            }
          }
          else
          {
            displayError2();
            break;
          } 
         default:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("    DOOR NOT");
          lcd.setCursor(0, 1);
          lcd.print("   REGISTERED");
          delay(1500);
          break;
      }
    }
    else
    {
      accessDenied();
    }
  Serial.println();
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" ACCESS CONTROL");
    lcd.setCursor(0, 1);
    lcd.print("  & RECORDING");
    delay(1500);
  }
}

void identifyDoor()
{
  doorNo = (digitalRead(13)<<2)|(digitalRead(12)<<1)|(digitalRead(11));
  Serial.print(doorNo, DEC);
  Serial.println();
  return;
}

int visitorIdCheck(String userID)
{
  // if(userID == VISITORKEY)
   {
      visitorFlag = 1;
   }
   return 0;
}

void identifyUser()
{
  lcd.clear();
  if(ID == CARD1)
  {
    lcd.setCursor(1, 0);
    lcd.print("SANDRA FRENCH");
  }
  else if(ID == KEY1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SANKET PANCHOLI");
  }
  else if(ID == KEY2)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("BHOOMI PATEL");
  }
  else if(ID == KEY3)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("SUMAIR DAGIYA");
  }
  else if(ID == KEY4)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("PARTH PATEL");
  }
  else if(ID == KEY5)
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("VISITOR 1");
  }
  else
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("VISITOR 2");
  }
  return;
}

void accessGranted()
{
  lcd.setCursor(1, 1);
  lcd.print("ACCESS GRANTED");
  soundBuzzer();
  return;
}

void accessDenied()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UNAUTHORIZD USER");
  lcd.setCursor(2, 1);
  lcd.print("ACCESS DENIED");
  for(char j=0;j<3;j++)
  {
    soundBuzzer();
    digitalWrite (2, LOW) ;// Buzzer Off
    delay (500) ;// delay 1ms
  }
  //delay(1500);
  return;
}

void displayError1()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("USER HAS ALREADY");
  lcd.setCursor(0, 1);
  lcd.print("ENTERED PREMISES");
  for(char j=0;j<3;j++)
  {
    soundBuzzer();
    digitalWrite (2, LOW) ;// Buzzer Off
    delay (500) ;// delay 1ms
  }
}

void displayError2()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("USER NOT REGIS-");
  lcd.setCursor(0, 1);
  lcd.print("TER ON MAIN DOOR");
  for(char j=0;j<3;j++)
  {
    soundBuzzer();
    digitalWrite (2, LOW) ;// Buzzer Off
    delay (500) ;// delay 1ms
  }
}

void soundBuzzer()
{
  for(int i=0;i<200;i++)
  {
    digitalWrite (2, HIGH) ;// Buzzer On
    delay (1) ;// Delay 1ms 
    digitalWrite (2, LOW) ;// Buzzer Off
    delay (1) ;// delay 1ms
  }
}

int displayError3(int userIdNo)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("USER NOT PUNCHED");
  lcd.setCursor(0, 1);
 // lcd.print(" OUT OF DOOR ");
  if(userRoom1Flag[userIdNo] == 1)
  {
    lcd.print(" OUT OF DOOR 1");
  }
  else if(userRoom2Flag[userIdNo] == 1)
  {
    lcd.print(" OUT OF DOOR 2");
  }
  else
  {
    lcd.print(" OUT OF DOOR 3");
  }
  for(char j=0;j<3;j++)
  {
    soundBuzzer();
    digitalWrite (2, LOW) ;// Buzzer Off
    delay (500) ;// delay 1ms
  }
  return;
}

void displayError4()
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("USER HAS NOT");
  lcd.setCursor(0, 1);
  lcd.print("ENTERED IN ROOM");
  for(char j=0;j<3;j++)
  {
    soundBuzzer();
    digitalWrite (2, LOW) ;// Buzzer Off
   delay (500) ;// delay 1ms
  }      
}
