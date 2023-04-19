// SAFEGUARDER
// ECE 1022


#include <SPI.h>
#include <RFID.h>
#define SS_PIN 10
#define RST_PIN 9
RFID rfid(SS_PIN, RST_PIN);
String rfidCard;

#include <Servo.h>;
const int lockPin = 8;
Servo lockServo;

#include <Keypad.h>
const byte ROWS = 4; // Define the number of rows and columns in the keypad
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {3, 2, A1, A0}; // Connect row pins to these Arduino pins
byte colPins[COLS] = {7, 6, 5, 4}; // Connect column pins to these Arduino pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Create a Keypad object with the specified pins and keys

int code[4] = {1, 0, 2, 2}; // Set the correct code as an array of integers
int enteredCode[4]; // Create an array to hold the entered code
int codeIndex = 0; // Set the index for the entered code to 0

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() 
{
  lockServo.attach(lockPin); // Attach the servo to the specified pin
  lockServo.write(0);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Scan Card");
  lcd.setCursor(0,1);
  lcd.print("Enter Code");
  SPI.begin();
  rfid.init();
}




void loop() 
{
  char key = keypad.getKey(); // Get the pressed key from the keypad
  if (key != NO_KEY) { // If a key was pressed
    if (key == '#') { // If the # key was pressed, clear the entered code
      for (int i = 0; i < 4; i++) {
        enteredCode[i] = 0; // Set each element of the entered code array to 0
      }
      codeIndex = 0; // Reset the index for the entered code to 0
    } else if (codeIndex < 4) { // If the maximum number of code digits hasn't been entered yet
      enteredCode[codeIndex] = key - '0'; // Convert the pressed key to an integer and store it in the entered code array
      codeIndex++; // Increment the index for the entered code
      if (codeIndex == 4) { // If the maximum number of code digits has been entered
        if (checkCode()) { // Check if the entered code matches the correct code
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Access Granted");
          lockServo.write(90); // If the code is correct, turn the servo 90 degrees
          delay(15000);
          lockServo.write(0);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Scan Card");
          lcd.setCursor(0,1);
          lcd.print("Enter Code");
        }
        codeIndex = 0; // Reset the index for the entered code to 0
      }
    }
  }
  if (rfid.isCard()) 
  {
    if (rfid.readCardSerial()) 
    {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);
      if (rfidCard == "163 62 169 137") 
      {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Access Granted");
          lockServo.write(90); // If the code is correct, turn the servo 90 degrees
          delay(15000);
          lockServo.write(0);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Scan Card");
          lcd.setCursor(0,1);
          lcd.print("Enter Code");
      }
      else
      {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Access Denied");
          delay(5000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Scan Card");
          lcd.setCursor(0,1);
          lcd.print("Enter Code");
      }
    }
    rfid.halt();
  }

}

bool checkCode() 
{
  for (int i = 0; i < 4; i++) 
  {
    if (enteredCode[i] != code[i]) 
    { // Check if each entered code digit matches the corresponding correct code digit
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Access Denied");
      delay(5000);
      for (int i = 0; i < 4; i++) 
      {
        enteredCode[i] = 0; // Set each element of the entered code array to 0
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Scan Card");
      lcd.setCursor(0,1);
      lcd.print("Enter Code");
      return false; // If any digit doesn't match, the code is incorrect
      
    }
  }
  return true; // If all digits match, the code is correct
}
