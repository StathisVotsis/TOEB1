    #include <SPI.h>
    #include "PN532_SPI.h"
    #include "PN532.h"
    #include "NfcAdapter.h"
    #include <Wire.h>
    #include "DFRobot_RGBLCD.h"
    #include <Keypad.h>
    #include "Countimer.h"

    PN532_SPI interface(SPI, 10); // create a SPI interface for the shield with the SPI CS terminal at digital pin 10

    NfcAdapter nfc = NfcAdapter(interface); // create an NFC adapter object
    DFRobot_RGBLCD lcd(16,2);  //16 characters and 2 lines of show

    const byte ROWS = 4; //four rows
    const byte COLS = 4; //four columns
    char keys[ROWS][COLS] = {
       {'1','2','3','A'},
       {'4','5','6','B'},
       {'7','8','9','C'},
       {'*','0','#','D'}
                            };

    byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
    byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
     
    Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
    Countimer timer;

    int val;
    float volt;
    char id[30];
    int tim=0;
    
   

    void setup(void)
    {
        Serial.begin(115200); // start serial comm
        Serial.println("NDEF Reader");
        nfc.begin(); // begin NFC comm
        // initialize
        lcd.init();
        // Print a message to the LCD.
        lcd.print("Welcome");
        pinMode(A1, OUTPUT); 
        pinMode(A2, OUTPUT);      
        buzer2();
        digitalWrite(A1,LOW);
         //breath2(REG_GREEN);
         //lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("Created by");
         lcd.setCursor(0, 1);
         lcd.print("S.Votsis");
         delay(2000);
        
         
    }

    void loop(void)
    {
      
      val=analogRead(0);//This divider module will divide the measured voltage by 5, the maximum voltage it can measure is 25V.
      volt=val/40.92; // 
      val=(int)volt;//
      
      //Ean eisai off
      if(val<0.5)
      {
         lcd.setCursor(0, 0);
         lcd.print("System off");
         char key = keypad.getKey();
        if (key=='1'){
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Wait card");
                  buzer2();
                  tagcheck();
                     }
        //tagcheck();
      }
      //Ean eisai on
      else
      {
        
      }   
    }

    void tagcheck()//skanarw kai grafw 0
    {
      while(1)
      {
        char key = keypad.getKey();
        if (key=='#'){
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Cancel");
                  buzer();
                  delay(2000);
                  break;
                 
                     }
       //lcd.println("Place a formatted Mifare Classic NFC tag on the reader.");
      if(nfc.tagPresent())
      {
      String payloadAsString = "" ;
       NfcTag tag = nfc.read(); // read the NFC tag
          if(tag.hasNdefMessage())
          {
            NdefMessage message = tag.getNdefMessage();
            //String payloadAsString = "" ;
           
            for(int i=0;i<message.getRecordCount();i++)
            {
              NdefRecord record = message.getRecord(i);
              int payloadLength = record.getPayloadLength();
              byte payload[payloadLength];
              record.getPayload(payload);
              Serial.write(payload,payloadLength);
              //PrintHexChar(payload, payloadLength);
              for (int c = 1; c < payloadLength; c++) {//the first 0 is \n
              payloadAsString += (char)payload[c];
              //Serial.println((char)payload[c]);
             }
             
             //Serial.println(payloadAsString);
            }
             
             
          }
       
            
       
        ////////////////////////////////////////////////////
        NdefMessage message = NdefMessage();
        message.addUriRecord("0");

        bool success = nfc.write(message);
        if(success)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Armed");
          buzer();
          timer.restart();
          int x = payloadAsString.toInt();
          digitalWrite(A1,HIGH);
          timer.setCounter(0, x, 0, timer.COUNT_UP, onComplete);

          // Print current time every 1s on serial port by calling method refreshClock().
          timer.setInterval(refreshClock, 1000);
          while(1)
          {
         char key = keypad.getKey();
         if (key=='#')
         {
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Cancel");
                  buzer();
                  digitalWrite(A1,LOW);
                  delay(2000);
                  break;
                 
          }   
             // Run timer
              timer.run();

         // Now timer is running and listening for actions.
         // If you want to start the timer, you have to call start() method.
         if(!timer.isCounterCompleted()) 
         {
               timer.start();
               
         }                                      
         else
         {
                digitalWrite(A1,LOW);
                break;
         }                      
         }
          
          delay(2000);
          lcd.clear();
          break;
          
        }
        else{
          lcd.print("failed.");
          break;
        }
      }
      }

    }

      
    void refreshClock() {
   
   Serial.println("Current time is: ");
   
    Serial.println(timer.getCurrentTime());
    }

   void onComplete() {
   Serial.print("Complete!!!");
   }

   void buzer()
   {
    int i;
        for (i = 0; i <80; i++) // When a frequency sound
        {
        digitalWrite (A2, HIGH) ; //send tone
        delay (1) ;
        digitalWrite (A2, LOW) ; //no tone
        delay (1) ;
        } 
        for (i = 0; i <100; i++) 
        {
        digitalWrite (A2, HIGH) ;
        delay (2) ;
        digitalWrite (A2, LOW) ;
        delay (2) ;
        } 
   }

   void buzer2()
   {
    int i;
        for (i = 0; i <100; i++) 
        {
        digitalWrite (A2, HIGH) ;
        delay (2) ;
        digitalWrite (A2, LOW) ;
        delay (2) ;
        } 

        for (i = 0; i <80; i++) // When a frequency sound
        {
        digitalWrite (A2, HIGH) ; //send tone
        delay (1) ;
        digitalWrite (A2, LOW) ; //no tone
        delay (1) ;
        } 
   }

    void breath(unsigned char color){
    for(int i=0; i<255; i++){
        lcd.setPWM(color, i);
        delay(5);
    }

    delay(500);
    for(int i=254; i>=0; i--){
        lcd.setPWM(color, i);
        delay(5);
    }

    delay(500);
    }
    
    void breath2(unsigned char color){
    for(int i=0; i<255; i++){
        lcd.setPWM(color, i);
        delay(5);
    }
    delay(500);
    }
