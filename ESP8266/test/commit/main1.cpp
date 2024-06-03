/*
полностью настроиный код и прокоментирован даля приёма JSON данных, преобразование их в переменные и вывод в терминал
*/
//---------------- БИБЛИОТЕКИ START ---------------------
#include <Arduino.h>             // Основная библиотека для Arduino
#include <ESP8266WiFi.h>         // Библиотека для работы с WiFi на ESP8266
#include <ESP8266WebServer.h>    // Библиотека для создания web сервера на ESP8266
#include <ArduinoJson.h>         // Библиотека для работы с JSON
//----------------- БИБЛИОТЕКИ END-----------------------

//-------------------- DEFINE START ---------------------
#define WiFi_Status D0           // Определение пина для индикации статуса WiFi
//-------------------- DEFINE END -----------------------

//----------------- CONSTANTS START ---------------------
// const char *ssid = "Keenetic-9461"; // Закомментированные данные WiFi
// const char *password = "VAsbNoxP"; 
const char *ssid = "Samsung 8S";     // SSID для подключения к WiFi
const char *password = "Acer5560g!"; // Пароль для подключения к WiFi
//----------------- CONSTANTS END -----------------------

//------------ СОЗДАНИЕ ЭКЗЕМПЛЯРА START ----------------
ESP8266WebServer server(80);         // Создание экземпляра веб-сервера на порту 80
//------------- СОЗДАНИЕ ЭКЗЕМПЛЯРА END -----------------

//----------------- СПИСОК ФУНКЦИЙ START-----------------
void handlePost();                   // Объявление функции для обработки POST запросов
void blinks(int, int, int8_t);       // Объявление функции для мигания светодиодом
//----------------- СПИСОК ФУНКЦИЙ END ------------------

//--------------------- SETUP START ---------------------
void setup(void)
{
  Serial.begin(115200);              // Начало серийной связи с бодрейтом 115200
  pinMode(WiFi_Status, OUTPUT);      // Настройка пина статуса WiFi как выход

  digitalWrite(WiFi_Status, LOW);    // Устанавливаем низкий уровень сигнала на старт

  // *** инициализация WiFi
  WiFi.begin(ssid, password);        // Подключение к WiFi
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");               // Вывод точки в Serial Monitor пока не подключимся
    blinks(150, 50, 1);              // Мигание светодиодом во время попытки подключения
  }
  digitalWrite(WiFi_Status, HIGH);   // Устанавливаем высокий уровень сигнала после подключения
  Serial.println();
  Serial.println("WiFi connected");  // Сообщение о подключении к WiFi
  // ***
  
  server.on("/in", HTTP_POST, handlePost); // Регистрация обработчика для пути /in и типа запроса POST
  server.begin();                          // Запуск сервера
  Serial.println("HTTP server started");   // Сообщение о запуске сервера
}
//--------------------- SETUP END -----------------------

void handlePost() {
  if (server.hasArg("plain") == false) { // Проверка наличия данных в запросе
    server.send(400, "text/plain", "Bad Request: Data not received"); // Ответ 400, если данных нет
    return;
  }

  DynamicJsonDocument doc(1024);         // Создание JSON документа
  deserializeJson(doc, server.arg("plain")); // Разбор JSON
  
  double vale1 = doc["vale1"];           // Получение данных из JSON
  double vale2 = doc["vale2"];
  double vale3 = doc["vale3"];
  double vale4 = doc["vale4"];

  Serial.println("Received JSON data:"); // Вывод полученных данных в Serial Monitor
  Serial.print("vale1: "); Serial.println(vale1);
  Serial.print("vale2: "); Serial.println(vale2);
  Serial.print("vale3: "); Serial.println(vale3);
  Serial.print("vale4: "); Serial.println(vale4);

  server.send(200, "application/json", "{\"success\":true}"); // Отправка ответа 200 с JSON
}

void loop() {
  server.handleClient();                 // Обработка подключений клиентов
}

void blinks(int onTime, int offTime, int8_t repeat) {
  for (int i = 0; i < repeat; i++) {
    digitalWrite(WiFi_Status, HIGH);     // Включение светодиода
    delay(onTime);                       // Задержка включенного состояния
    digitalWrite(WiFi_Status, LOW);      // Выключение светодиода
    delay(offTime);                      // Задержка выключенного состояния
  }
}
