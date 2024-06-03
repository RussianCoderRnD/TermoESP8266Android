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
  Serial.begin(115200);  // открываем порт для связи с ПК

}
//--------------------- SETUP END ----------------------


//------------------- LOOP START -----------------------
void loop(void) {
 if (Serial.available()) {
    // Читайте строку до новой строки
    String data = Serial.readStringUntil('\n');
    
    // Разбиение строки на части
    float firstCommaIndex = data.indexOf(',');
    float value1 = data.substring(0, firstCommaIndex).toInt();
    
    float secondCommaIndex = data.indexOf(',', firstCommaIndex + 1);
    float value2 = data.substring(firstCommaIndex + 1, secondCommaIndex).toFloat();
    
    float thirdCommaIndex = data.indexOf(',', secondCommaIndex + 1);
    float value3 = data.substring(secondCommaIndex + 1, thirdCommaIndex).toInt();
    
    float fourthCommaIndex = data.indexOf(',', thirdCommaIndex + 1);
    float value4 = data.substring(thirdCommaIndex + 1, fourthCommaIndex).toFloat();
    
    float fifthCommaIndex = data.indexOf(',', fourthCommaIndex + 1);
    float value5 = data.substring(fourthCommaIndex + 1, fifthCommaIndex).toInt(); 

    float sexCommaIndex = data.indexOf(',', fourthCommaIndex + 1);
    float value6 = data.substring(fifthCommaIndex + 1, sexCommaIndex).toInt();

    // Выведите полученные значения
    Serial.println("Принятые значения:");
    Serial.println(value1);
    Serial.println(value2);
    Serial.println(value3);
    Serial.println(value4);
    Serial.println(value5);
    Serial.println(value6);
  }

  // Вывод результата
  Serial.println();Serial.print("value1");  Serial.print(",");  Serial.print("value2");  Serial.print(","); Serial.print("value3");Serial.print(",");  Serial.print("value4");  Serial.print(","); Serial.print("value5");Serial.print(","); Serial.print("value6");
}
