/**/

//---------------- БИБЛИОТЕКИ START ---------------------
#include <Arduino.h>
#include <SoftwareSerial.h>
//----------------- БИБЛИОТЕКИ END-----------------------


//-------------------- DEFINE START ---------------------


SoftwareSerial mySerial(10, 11); // RX, TX
//-------------------- DEFINE END -----------------------


//------------ СОЗДАНИЕ ЭКЗЕМПЛЯРА START ----------------

//------------- СОЗДАНИЕ ЭКЗЕМПЛЯРА END -----------------


//---------------- ПЕРЕМЕННЫЕ START----------------------

//----------------- ПЕРЕМЕННЫЕ END ----------------------


//----------------- СПИСОК ФУНКЦИЙ START-----------------

//----------------- СПИСОК ФУНКЦИЙ END ------------------


//--------------------- SETUP START ---------------------
void setup(void) {
  Serial.begin(57600);  // открываем порт для связи с ПК
  mySerial.begin(57600);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
}
//--------------------- SETUP END ----------------------


//------------------- LOOP START -----------------------
void loop(void) {
  // Подготовьте данные к отправке
  int value1 = analogRead(A0);delay(1);
  int value2 = analogRead(A1);delay(1);
  int value3 = analogRead(A2);delay(1);

if(value1<=260)value1=0;
if(value2<=260)value2=0;
if(value3<=260)value3=0;

  // Отправьте данные на ESP8266
  mySerial.print(value1);
  mySerial.print(',');
  mySerial.print(value2);
  mySerial.print(',');
  mySerial.print(value3);

 /* Serial.print(value1);
  Serial.print(',');
  Serial.print(value2);
  Serial.print(',');
  Serial.println(value3);*/

  // Пауза между отправками

  // Задержка перед следующей отправкой
  delay(500);
}
//--------------------- LOOP END ----------------------
