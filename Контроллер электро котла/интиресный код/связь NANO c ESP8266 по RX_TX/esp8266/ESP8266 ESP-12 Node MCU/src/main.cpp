/**/

//---------------- БИБЛИОТЕКИ START ---------------------
#include <Arduino.h>
//----------------- БИБЛИОТЕКИ END-----------------------


//-------------------- DEFINE START ---------------------

//-------------------- DEFINE END -----------------------


//------------ СОЗДАНИЕ ЭКЗЕМПЛЯРА START ----------------

//------------- СОЗДАНИЕ ЭКЗЕМПЛЯРА END -----------------


//---------------- ПЕРЕМЕННЫЕ START----------------------

//----------------- ПЕРЕМЕННЫЕ END ----------------------


//----------------- СПИСОК ФУНКЦИЙ START-----------------
void process_data(String data);
//----------------- СПИСОК ФУНКЦИЙ END ------------------


//--------------------- SETUP START ---------------------
void setup(void) {
  Serial.begin(57600);  // открываем порт для связи с ПК
pinMode(A0, INPUT_PULLDOWN_16)
}
//--------------------- SETUP END ----------------------


//------------------- LOOP START -----------------------
void loop(void) {
  delay(1000);
if (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // Чтение данных до переноса строки
    process_data(data);
  }
}
//--------------------- LOOP END ----------------------

void process_data(String data) {
  // Разбор строки на компоненты
  int firstCommaIndex = data.indexOf(',');
  int secondCommaIndex = data.indexOf(',', firstCommaIndex + 1);

  // Извлечение каждого числа из строки
  int a = data.substring(0, firstCommaIndex).toInt();
  int b = data.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
  int c = data.substring(secondCommaIndex + 1).toInt();


  // Вывод результата
   Serial.println();Serial.print(a);  Serial.print(",");  Serial.print(b);  Serial.print(","); Serial.print(c);
}