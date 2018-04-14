#include <Servo.h>
#include "macros.h"

Servo myServo;

int YELLOW_LED        = 48; // kk
int YELLOW_BUTTON     = 46; // kk
int SERVO_PIN         = 44; // kk

int GREEN_BUTTON      = 34; // kk
int GREEN_LED         = 32;  // kk

int RED_LED           = 30; // kk
int RED_BUTTON        = 28; // kk

int PURPLE_BUTTON     = 36; // kk
int PURPLE_RED        = 38; // kk
int PURPLE_GREEN      = 40; // kk
int PURPLE_BLUE       = 42; // kk

int BLUE_LED          = 49; // kk
int BLUE_BUTTON       = 47; // kk

int WHITE_BUTTON      = 45; //k k

int RED_WHITE         = 43; //k k
int GREEN_WHITE       = 41; //k k
int BLUE_WHITE        = 39; //k k

int RED_POTENTIOMETER   = A0;
int GREEN_POTENTIOMETER = A1;
int BLUE_POTENTIOMETER  = A2;
int TEMPERATURE_SENSOR  = A3;
int SERVO_POTENTIOMETER = A4;
int LDR             = A5;

int MAIN_LED_RED        = 7;
int MAIN_LED_BLUE       = 6;
int MAIN_LED_GREEN      = 5;

int arr[] = {0,1,2};

int minLDRVal = 10000;
int maxLDRVal = 0;
int midLDRVal = 40;

void blink(){
  digitalWrite(13,LOW);
  delay(1000);
  digitalWrite(13,HIGH);
  delay(1000);  
}

int readTemperature() {
  int sensorVal = analogRead(TEMPERATURE_SENSOR);
  int voltaje = 5.0 / 1024 * sensorVal;
  int temperatura = (voltaje - 0.5) * 100;
  return temperatura;
}

void rotateServo() {
  int potentiometerValue = analogRead(SERVO_POTENTIOMETER);
  int val = map(potentiometerValue, 0, 1024, 0, 180);
  myServo.write(val);
}

void writeMainRGB() {
    int redColor = analogRead(RED_POTENTIOMETER);
    int greenColor = analogRead(GREEN_POTENTIOMETER);
    int blueColor = analogRead(BLUE_POTENTIOMETER);

    redColor = map(redColor,0,1024,0,255);
    greenColor = map(greenColor,0,1024,0,255);
    blueColor = map(blueColor,0,1024,0,255);

    analogWrite(MAIN_LED_RED,   redColor);
    analogWrite(MAIN_LED_GREEN, greenColor);
    analogWrite(MAIN_LED_BLUE,  blueColor);
}

void readOneValue(int index){
  while(Serial.available() == 0);

  if(Serial.available()){
    String res = "";
    delay(100);
    while(Serial.available()){
        res += (char)Serial.read();  
    }

    arr[index] = res.toInt();

    Serial.println("OK: " + String(arr[index]));
  }
}

void readInitialValues(){
  readOneValue(0);
  readOneValue(1);
  readOneValue(2);


}

void turnOnWhiteRGB(){
  digitalWrite(RED_WHITE,HIGH);   
  digitalWrite(GREEN_WHITE,HIGH);
  digitalWrite(BLUE_WHITE,HIGH);
}

void turnOffWhiteRGB(){
  digitalWrite(RED_WHITE,LOW);   
  digitalWrite(GREEN_WHITE,LOW);
  digitalWrite(BLUE_WHITE,LOW);
}

void turnOnPurpleRGB(){
  digitalWrite(PURPLE_RED,LOW); 
  digitalWrite(PURPLE_GREEN,HIGH);  
  digitalWrite(PURPLE_BLUE,LOW);
}

void turnOffPurpleRGB(){
  digitalWrite(PURPLE_RED,HIGH); 
  digitalWrite(PURPLE_GREEN,HIGH);  
  digitalWrite(PURPLE_BLUE,HIGH);  
}

void calibrateLDR(){
    long current  = millis();

    while(millis() < (current + 5000)){
        int val = analogRead(LDR);
        if(val < minLDRVal){
            minLDRVal = val;
        }
        if(val > maxLDRVal){
            maxLDRVal = val;
        }
    }
    //Get the distance between them
    midLDRVal = maxLDRVal - minLDRVal;
}

int assertRedLed(){
    if(digitalRead(RED_BUTTON) == HIGH){
        digitalWrite(RED_LED, LOW);
        return RED_LED_MACRO;
    }
    return -1;
}

int assertGreenLed(){
    if(digitalRead(GREEN_BUTTON) == HIGH){
        digitalWrite(GREEN_LED, LOW);
        return GREEN_LED_MACRO;
    }
    return -1;
}

int assertYellowLed(){
    if(digitalRead(YELLOW_BUTTON) == HIGH){
        digitalWrite(YELLOW_LED,LOW);
        return YELLOW_LED_MACRO;
    }
    return -1;
}

int assertBlueLed(){
    if(digitalRead(BLUE_BUTTON) == HIGH){
        digitalWrite(BLUE_LED,LOW);
        return BLUE_LED_MACRO;
    }
    return -1;
}

int assertPurpleLed(){
    if(digitalRead(PURPLE_BUTTON) == HIGH){
        turnOffPurpleRGB();
        return PURPLE_LED_MACRO;
    }
    return -1;
}

int assertWhiteLed(){
    if(digitalRead(WHITE_BUTTON) == HIGH){
        turnOffWhiteRGB();
        return WHITE_LED_MACRO;
    }
    return -1;
}

int assertLDR(){
    if(analogRead(LDR) < (minLDRVal + (midLDRVal/4))){
        return COVER_LDR_MACRO;
    }
    return -1;
}

int assertServo0Degrees(){
    int angle  = myServo.read();
    if(angle >= 0 && angle <= 1){
        return SERVO_0_MACRO;
    }
    return -1;
}

int assertServo90Degrees(){
    int angle  = myServo.read();
    if(angle >= 89 && angle <= 91){
        return SERVO_90_MACRO;
    }
    return -1;
}

int assertServo180Degrees(){
    int angle  = myServo.read();
    if(angle >= 179 && angle <= 180){
        return SERVO_180_MACRO;
    }
    return -1;
}

int assertRGB_RedColor(){
    if(analogRead(RED_POTENTIOMETER) > 200 && analogRead(BLUE_POTENTIOMETER) < 15 && analogRead(GREEN_POTENTIOMETER) < 15){
        return MAIN_R_MACRO;
    }
    return -1;
}

int assertRGB_GreenColor(){
    if(analogRead(GREEN_POTENTIOMETER) > 200 && analogRead(RED_POTENTIOMETER) < 15 && analogRead(BLUE_POTENTIOMETER) < 15){
        return MAIN_G_MACRO;
    }
    return -1;
}

int assertRGB_BlueColor(){
    if(analogRead(BLUE_POTENTIOMETER) > 200 && analogRead(RED_POTENTIOMETER) < 15 && analogRead(GREEN_POTENTIOMETER) < 15){
        return MAIN_B_MACRO;
    }
    return -1;
}

int assertRGB_Red_Green_Color(){
    if(analogRead(RED_POTENTIOMETER) > 200 && analogRead(BLUE_POTENTIOMETER) < 15 && analogRead(GREEN_POTENTIOMETER) > 200){
        return MAIN_RG_MACRO;
    }
    return -1;
}

int assertRGB_Red_Blue_Color(){
    if(analogRead(RED_POTENTIOMETER) > 200 && analogRead(BLUE_POTENTIOMETER) > 200 && analogRead(GREEN_POTENTIOMETER) < 15){
        return MAIN_RB_MACRO;
    }
    return -1;
}

void setUpBoard(){
  for(int i = 0; i < 3; i++){
    switch(arr[i]){
        case RED_LED_MACRO:
            digitalWrite(RED_LED,HIGH);
        break;
        case GREEN_LED_MACRO:
            digitalWrite(GREEN_LED,HIGH);
        break;
        case YELLOW_LED_MACRO:
            digitalWrite(YELLOW_LED,HIGH);
        break;
        case PURPLE_LED_MACRO:
            turnOnPurpleRGB();
        break;
        case BLUE_LED_MACRO:
            digitalWrite(BLUE_LED,HIGH);
        break;
        case WHITE_LED_MACRO:
            turnOnWhiteRGB();
        break;
    }  
  }  
}

void flash(int pin){
  digitalWrite(pin,HIGH);
  delay(100);
  digitalWrite(pin,LOW);
  
}

void setup() {
  Serial.begin(9600);

  pinMode(GREEN_LED,    OUTPUT); 
  pinMode(RED_LED,      OUTPUT); 
  pinMode(YELLOW_LED,   OUTPUT); 
  pinMode(PURPLE_RED,   OUTPUT);
  pinMode(PURPLE_GREEN, OUTPUT);
  pinMode(PURPLE_BLUE,  OUTPUT);
  pinMode(BLUE_LED,     OUTPUT);
  
  pinMode(BLUE_WHITE,   OUTPUT);
  pinMode(GREEN_WHITE,  OUTPUT);
  pinMode(RED_WHITE,    OUTPUT);

  pinMode(MAIN_LED_RED,   OUTPUT);
  pinMode(MAIN_LED_GREEN, OUTPUT);
  pinMode(MAIN_LED_BLUE,  OUTPUT);

  pinMode(GREEN_BUTTON,      INPUT);
  pinMode(RED_BUTTON,          INPUT);
  pinMode(YELLOW_BUTTON,     INPUT);
  pinMode(PURPLE_BUTTON,     INPUT);
  pinMode(BLUE_BUTTON,       INPUT);
  pinMode(WHITE_BUTTON,        INPUT);
  pinMode(RED_POTENTIOMETER,   INPUT);
  pinMode(GREEN_POTENTIOMETER, INPUT);
  pinMode(BLUE_POTENTIOMETER,  INPUT);
  pinMode(TEMPERATURE_SENSOR,  INPUT);
  pinMode(SERVO_POTENTIOMETER, INPUT);
  pinMode(LDR, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  turnOffPurpleRGB();
  calibrateLDR();

  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  Serial.println("HIT ME UP");
  readInitialValues();

  flash(RED_LED);
  flash(GREEN_LED);
  flash(YELLOW_LED);
  flash(BLUE_LED);

}

int findInArr(int value){
    for(int i = 0; i < 3; i++){
        if(arr[i] == value)
            return i;
    }
    return -1;
}

void updateArray(int index){
    Serial.println(String(arr[index]));
    readOneValue(index);
}

String res = "";
int evaluation = 0;
int index = 0;

void writeData(){
  String res = "";
  res +="Red: ";
  res += String(analogRead(RED_POTENTIOMETER));  

  res +=". Green: ";
  res += String(analogRead(GREEN_POTENTIOMETER));  

  res +=". Blue: ";
  res += String(analogRead(BLUE_POTENTIOMETER));  

  Serial.println(res);
  
}

void loop() {
     writeMainRGB();
    rotateServo();
    setUpBoard();

    
    evaluation = assertRedLed();
    if(evaluation != -1){
        //means it was pressed: must look up in the array 
        index = findInArr(evaluation);
        if(index != -1){
            //was found in task list...must get another one: send arr[index] to Serial
            updateArray(index);
        }
    }

    evaluation = assertGreenLed();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertYellowLed();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertBlueLed();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertPurpleLed();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertWhiteLed();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertLDR();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertServo0Degrees();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertServo90Degrees();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertServo180Degrees();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertRGB_RedColor();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertRGB_GreenColor();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertRGB_BlueColor();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertRGB_Red_Green_Color();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }

    evaluation = assertRGB_Red_Blue_Color();
    if(evaluation != -1){
        index = findInArr(evaluation);
        if(index != -1) updateArray(index);
    }
    
}
