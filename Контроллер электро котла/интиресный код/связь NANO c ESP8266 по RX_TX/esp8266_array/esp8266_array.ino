/*ESP8266 ПРИНИМАЕТ*/

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
  Serial.begin(115200);  // открываем порт для связи с ПК

}
//--------------------- SETUP END ----------------------


//------------------- LOOP START -----------------------
void loop(void) {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // Чтение строки до символа новой строки
    process_data(data);
  }
}
//--------------------- LOOP END ----------------------

void process_data(String data) {
  int nums[5]; // Массив для хранения чисел
  int lastComma = -1;
  int idx = 0;

  // Разбираем строку на числа
  for (int i = 0; i < 5; i++) {
    int commaIndex = data.indexOf(',', lastComma + 1);
    if (commaIndex == -1) commaIndex = data.length();
    nums[i] = data.substring(lastComma + 1, commaIndex).toInt();
    lastComma = commaIndex;
  }

  // Вычисляем сумму и среднее
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += nums[i];
  }
  float average = sum / 2.0;

  // Выводим результат
  Serial.print("Sum / 2 = ");
  Serial.println(average);
}
