/*http://192.168.4.1/in?value1=1&value2=2&value3=3
*/

//---------------- БИБЛИОТЕКИ START ---------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
//----------------- БИБЛИОТЕКИ END-----------------------

//-------------------- DEFINE START ---------------------
#define WiFi_Status D0 // пин на котором висит светодиод индикации работы МК
//-------------------- DEFINE END -----------------------

//----------------- CONSTANTS START ---------------------
// const char *ssid = "Keenetic-9461";
// const char *password = "VAsbNoxP";
const char *ssid = "Samsung 8S";
const char *password = "Acer5560g!";

//----------------- CONSTANTS END -----------------------

//------------ СОЗДАНИЕ ЭКЗЕМПЛЯРА START ----------------
ESP8266WebServer server(80); // Объект сервера на порту 80
//------------- СОЗДАНИЕ ЭКЗЕМПЛЯРА END -----------------

//---------------- ПЕРЕМЕННЫЕ START----------------------
//----------------- ПЕРЕМЕННЫЕ END ----------------------

//----------------- СПИСОК ФУНКЦИЙ START-----------------
void OutMesage();              // функция передачи JSON
void InMesage();               // функция приёма JSON
void blinks(int, int, int8_t); // функция промигивания работы МК
void valueIn();
//----------------- СПИСОК ФУНКЦИЙ END ------------------

//--------------------- SETUP START ---------------------
void setup(void)
{
  Serial.begin(115200); // открываем порт для связи с ПК

  pinMode(WiFi_Status, OUTPUT);

  digitalWrite(WiFi_Status, LOW);

  // *** инициализация WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    blinks(150, 50, 1);
  }
  digitalWrite(WiFi_Status, HIGH);
  Serial.println();
  Serial.println("WiFi connected");
  // ***
  // *** формат получения запросов из WiFi
  server.on("/out", OutMesage);          // Регистрируем обработчик главной страницы
  server.on("/in1", HTTP_POST, InMesage); // Определение обработчика для пути "/upload"
  server.on("/in", valueIn); // Настраиваем обработчик для пути /in
                                         // ***
  server.begin(); // Запускаем сервер

  Serial.println("HTTP server started");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//--------------------- SETUP END -----------------------

//------------------- LOOP START ------------------------
void loop(void)
{
  server.handleClient(); // Постоянно слушаем WiFi
}
//--------------------- LOOP END ------------------------

// функция ответа на запрос от сервера
void OutMesage()
{
  int values_out_1 = 1;
  int values_out_2 = 2;
  int values_out_3 = 3;

  String jsonData = "{\"values_out_1\": " + String(values_out_1) + ", \"values_out_2\": " + String(values_out_2) + ", \"values_out_3\": " + String(values_out_3) + "}"; // формируем JSON пакет
  server.send(200, "application/json", jsonData);                                                                                                                       // Отправляем данные в JSON формате

  Serial.printf("values_out_1: %.1f, values_out_2: %.1f, values_out_3:%.1f\n", values_out_1, values_out_2, values_out_3); // выводим в терминал сформированный JSON
}

// функция принимающая значения от сервера
void InMesage()
{
  blinks(50, 50, 1);
  if (!server.hasArg("plain"))
  {
    server.send(400, "text/plain", "400: Invalid request");
    return;
  }
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error)
  {
    server.send(500, "text/plain", "500: Could not decode JSON");
    return;
  }

  int values_in_1 = doc["valIn1"];
  int values_in_2 = doc["valIn2"];
  int values_in_3 = doc["valIn3"];
  // float value2 = doc["value2"];
  // String value3 = doc["value3"].as<String>();

  Serial.printf("values_in_1: %.1f, values_in_2: %.1f, values_in_3:%.1f\n", values_in_1, values_in_2, values_in_3); // выводим в терминал сформированный JSON

  server.send(200, "text/plain", "Data received successfully"); // ответ серверу 200
}

// функция промигивания светодиодом работы МК
void blinks(int del, int del2, int8_t coun = 1)
{
  for (size_t i = 0; i < coun; i++)
  {
    digitalWrite(WiFi_Status, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(del);                      // wait for a second
    digitalWrite(WiFi_Status, LOW);  // turn the LED off by making the voltage LOW
    delay(del2);
  }
}


void valueIn() {
  if (server.method() == HTTP_GET) {
    // Извлекаем параметры из URL
    String value1 = server.arg("value1");
    String value2 = server.arg("value2");
    String value3 = server.arg("value3");

int trans1 = value1.toInt();
float trans2 = value2.toFloat();

    // Выводим значения в терминал
    Serial.print("Received values: ");
    Serial.print("trans1=");
    Serial.print(trans1);
    Serial.print(", trans2=");
    Serial.print(trans2,1);
    Serial.print(", value3=");
    Serial.println(value3);

    // Отправляем ответ "OK" клиенту
    server.send(200, "text/plain", "OK");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}