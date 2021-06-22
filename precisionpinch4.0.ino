/*

PrecisionPinch 4.0

This code is used to measure the resistance through a Force Sensitive Resistor, display 
it on the serial monitor, a 16x2 LCD display, and record it to a micro SD card. 
It must be calibrated to accurately to function properly. 

The LCD display weight in kgs and pressure in Pa, the serial monitor displays weight
in kgs, and the weight is saved to the micro SD card in kgs. The units  can be 
altered by changing the calibration equation to convert to another unit of mass or 
pressure. Based on the current equations, the needle drivers must be centered accross
the FSR.

The circuit:
 * LCD RS pin to D7
 * LCD Enable pin to D6
 * LCD D4 pin to D5
 * LCD D5 pin to D4
 * LCD D6 pin to D3
 * LCD D7 pin to D2
 * LCD R/W pin to ground
 * LCD VSS pin to ground of potentiometer
 * LCD VCC pin to 5V
 * LCD V0 pin to wiper of 10K potentiometer
 * LCD 15 pin to 220 Ohm resistor then +5V
 * LCD 16 pin to ground
 * 
 * FSR 1 pin to +5V
 * FSR 2 pin to 10 KOhm resistor to ground
        change 10K resistor to range of sensitivity 
 * FSR 2 pin to A0
 * 
 * MicroSD card adapter GND to ground
 * MicroSD card adapter VCC to +5V
 * MicroSD card adapter MISO to D12
 * MicroSD card adapter MOSI to D11
 * MicroSD card adapter SCK to D13
 * MicroSD card adapter CS to D10

Created by Grant D Wandling
Penn State College of Medicine
January 2019

*/

    // include the library codes:
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>

    // Pin connected to SD memory unit
const int chipSelect = 10; 
    // Pin connected to FSR/resistor divider
const int FSR_PIN = A0; 

    // Measure the voltage at 5V and resistance of your 3.3k resistor, 
    //    and enter their value's below:
    // Measured voltage of Ardunio 5V line
const float VCC = 4.98; 
    // Measured resistance of 10kOhm resistor
const float R_DIV = 10000.0; 
    // set period in milliseconds
int period = 50;
    // set current time counter
unsigned long time_now = 0;

    // initialize the library with the numbers of the interface pins
LiquidCrystal lcd( 7, 6, 5, 4, 3, 2);


void setup() {
  Serial.begin(9600);
  
  pinMode(FSR_PIN, INPUT);
      // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
      // Print a message to the LCD.
  lcd.print("Start");

      // Print start card message to lcd and monitor
  Serial.print("Initializing SD card...");
      // set cursor next row
    lcd.setCursor(0, 1);
    lcd.println("SD card...");
        // delay 2.5s to read
      delay(2500);

      // Print failed card message to lcd and monitor
  if (!SD.begin()) {
    Serial.println("initialization failed!");
        // set cursor next row
      lcd.setCursor(0, 1);
      lcd.println("Card failed!");
          // delay 100s 
        delay(100000);
          

    return;
  }

      // Print card done message to lcd and monitor
  Serial.println("initialization done.");
      // set cursor next row
    lcd.setCursor(0, 1);
    lcd.println("Card done.");
        // delay 2.5s to read
      delay(2500);
      lcd.clear();
        
      // create file
    File dataFile;  
        // open SD card and add file
      dataFile = SD.open("datalog.txt", FILE_WRITE);
        // identify new data on SD card file
      dataFile.println("New data"); 
        // close file
      dataFile.close(); 
  
}



void loop() {
  
  time_now = millis();

      // define FSR current as your analog FSR pin
  int fsrADC = analogRead(FSR_PIN);
      // Create file
  File dataFile;  
      // Open SD card add created text file
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  
    // If the FSR has no pressure, the resistance will be
    //    near infinite. So the current should be near 0.
  if (fsrADC != 0) // If the analog reading is non-zero
  {
      // Use ADC reading to calculate voltage:
    float fsrV = fsrADC * VCC / 1023.0;  // Volts
      // Use voltage and static resistor value to 
      // Calculate FSR resistance:
    float fsrR = R_DIV * (VCC / fsrV - 1.0) / 1000; // kOhms
      // Calculate weight based on calibration
    float fsrW1 = 1113.4 * (1 / (fsrR)); // lbs       <- CALIBRATION EQUATION; R^2=0.982
      // Convert lbs to kg
    float fsrW2 = fsrW1 * 4.45; // kg
      // Calculate force based on mass and gavitational acceleration 
    float fsrM = fsrW2 * 9.81; // Newtons
      // Calculate pressure in MPa based on needle driver width & placement on sensor
      //    needle drivers should be centered through sensor for accuracy 
      //    needle driver width = 3mm; sensor diameter = 10mm
      //    area = 30 mm^2 = 3*10^-5 m^2
      //    1 MPa = 1000000 Pa
    // float fsrP = (fsrM / 0.00003) / 1000000;  // MPa
      // Print force to file
    dataFile.println(String(fsrM));
      // close file
    dataFile.close();
      // clear LCD
    lcd.clear();
      // Print force detected to LCD
    lcd.print("F = " + String(fsrM) + " N");
    
      while (millis() < time_now + period){
        //wait approx. [period] ms
      }
  }

  else
  {
      // No pressure detected
      // Print zero to file
    dataFile.println("0");
      // close file
    dataFile.close();
      // clear lcd
    lcd.clear();
      // print LCD
    lcd.print("F = 0 N");  
    
      while (millis() < time_now + period){
          //wait approx. [period] ms
        }
  }

}
