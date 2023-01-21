/*  
*                        
*    ______________      ____                                _____    _  _     _
*   |   __     __  |    / __ \ _________ _________   ____   |  __ \  | | \\   //  
*   |  |__|   |__| |   | |  | |___   ___ ___   ___  / __ \  | |  | | | |  \\ //  
*   |_    _________|   | |  | |   | |       | |    | |  | | | |  | | | |   | |
*   | \__/         |   | |__| |   | |       | |    | |__| | | |__| | | |   | |
*   |              |    \____/    |_|       |_|     \____/  |_____/  |_|   |_|
*   |_    _________|
*     \__/            
*
*    This Sketch was created to control Otto Starter with the Offical Web Bluetooth Controller for Otto DIY Robots.
*    For any question about this script you can contact us at education@ottodiy.com
*    By: Iván R. Artiles
*/

#include <SoftwareSerial.h>
#include <Otto.h>

#define LEFTLEG 2
#define RIGHTLEG 3
#define LEFTFOOT 4
#define RIGHTFOOT 5
#define TRIG 8
#define ECHO 9
#define BLE_TX 11
#define BLE_RX 12
#define BUZZER 13

int move_speed[] = {3000, 2000, 1000, 750, 500, 250};
int n = 2;
int ultrasound_threeshold = 15;
String command = "";

SoftwareSerial bluetooth(BLE_TX, BLE_RX);
Otto Ottobot;

long ultrasound_distance() {
   long duration, distance;
   digitalWrite(TRIG,LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG, LOW);
   duration = pulseIn(ECHO, HIGH);
   distance = duration/58;
   return distance;
}

void setup() {
  Ottobot.init(LEFTLEG, RIGHTLEG, LEFTFOOT, RIGHTFOOT, true, BUZZER);
  pinMode(TRIG, OUTPUT); 
  pinMode(ECHO, INPUT);
  bluetooth.begin(9600);
  Serial.begin(9600);
  
  Ottobot.home();
  bluetooth.print("AT+NAMEKevinsArduino");//found this here: ftp://imall.iteadstudio.com/Modules/IM130614001_Serial_Port_BLE_Module_Master_Slave_HM-10/DS_IM130614001_Serial_Port_BLE_Module_Master_Slave_HM-10.pdf
  
}

void loop() {
  checkBluetooth();//if something is coming at us
  if (command == "forward") {
    Forward();
  }
  else if (command == "backward") {
    Backward();
  }
  else if (command == "right") {
    Right();
  }
  else if (command == "left") {
    Left();
  }
  else if (command == "avoidance") {
    Avoidance();
  }
  else if (command == "force") {
    UseForce();
  }
}

void checkBluetooth() {
  char charBuffer[20];//most we would ever see
  
  if (bluetooth.available() > 0) {
    int numberOfBytesReceived = bluetooth.readBytesUntil('\n', charBuffer, 19);
    charBuffer[numberOfBytesReceived] = NULL;
    Serial.print("Received: ");
    Serial.println(charBuffer);
    
    n = charBuffer[numberOfBytesReceived-1]-'0';
    
    if (strstr(charBuffer, "forward") == &charBuffer[0]) {
      command = "forward";
    }   
    else if (strstr(charBuffer, "backward") == &charBuffer[0]) {
      command = "backward";
    }
    else if (strstr(charBuffer, "right") == &charBuffer[0]) {
      command = "right";
    }
    else if (strstr(charBuffer, "left") == &charBuffer[0]) {
      command = "left";
    }
    else if (strstr(charBuffer, "stop") == &charBuffer[0]) {
      command = "stop";
      Stop();
    }
    else if (strstr(charBuffer, "ultrasound") == &charBuffer[0]) {
      Stop();
      bluetooth.print(ultrasound_distance());
    }
    else if (strstr(charBuffer, "avoidance") == &charBuffer[0]) {
      command = "avoidance";
    }
    else if (strstr(charBuffer, "force") == &charBuffer[0]) {
      command = "force";
    }
    else if (strstr(charBuffer, "happy") == &charBuffer[0]) {
      command = "";
      Ottobot.playGesture(OttoSuperHappy);
    }
    else if (strstr(charBuffer, "victory") == &charBuffer[0]) {
      command = "";
      Ottobot.playGesture(OttoVictory);
    }
    else if (strstr(charBuffer, "sad") == &charBuffer[0]) {
      command = "";
      Ottobot.playGesture(OttoSad);
    }
    else if (strstr(charBuffer, "sleeping") == &charBuffer[0]) {
      command = "";
      Ottobot.playGesture(OttoSleeping);
    }
    else if (strstr(charBuffer, "confused") == &charBuffer[0]) {
      command = "";
      Ottobot.playGesture(OttoConfused);
    }
    else if (strstr(charBuffer, "fail") == &charBuffer[0]) {
      command = "";
      Ottobot.playGesture(OttoFail);
    }
    else if (strstr(charBuffer, "fart") == &charBuffer[0]) {
      command = "";
      Ottobot.playGesture(OttoFart);
    }
  }
}

void Forward() {
  Ottobot.walk(1, move_speed[n], FORWARD);
}

void Backward() {
  Ottobot.walk(1, move_speed[n], BACKWARD);
}

void Right() {
  Ottobot.walk(1, move_speed[n], RIGHT);
}

void Left() {
  Ottobot.walk(1, move_speed[n], LEFT);
}

void Stop() {
  Ottobot.home();
}

void Avoidance() {
  if (ultrasound_distance() <= ultrasound_threeshold) {
    Ottobot.playGesture(OttoConfused);
    for (int count=0 ; count<2 ; count++) {
      Ottobot.walk(1,move_speed[n],-1); // BACKWARD
    }
    for (int count=0 ; count<4 ; count++) {
      Ottobot.turn(1,move_speed[n],1); // LEFT
    }
  }
  Ottobot.walk(1,move_speed[n],1); // FORWARD
}

void UseForce() {
  if (ultrasound_distance() <= ultrasound_threeshold) {
      Ottobot.walk(1,move_speed[n],-1); // BACKWARD
    }
    if ((ultrasound_distance() > 10) && ( ultrasound_distance() < 15)) {
      Ottobot.home();
    }
    if ((ultrasound_distance() > 15) && ( ultrasound_distance() < 30)) {
      Ottobot.walk(1,move_speed[n],1); // FORWARD
    }
    if (ultrasound_distance() > 30) {
      Ottobot.home();
    }  
}

void Settings(String ts_ultrasound) {
  ultrasound_threeshold = ts_ultrasound.toInt();
}