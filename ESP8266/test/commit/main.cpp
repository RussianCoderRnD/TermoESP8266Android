//------------ Включение необходимых библиотек ------------
#include <Arduino.h>          // Основная библиотека для Arduino
#include <ESP8266WiFi.h>      // Библиотека для работы с WiFi на ESP8266
#include <ESP8266WebServer.h> // Библиотека для создания web сервера на ESP8266
#include <ArduinoJson.h>      // Библиотека для работы с JSON

//------------ Определение констант и настройка пинов ------------
#define WiFi_Status D0 // Определение пина для индикации статуса WiFi

//------------ Настройка параметров WiFi соединения ------------
//const char *ssid = "Samsung 8S";     // SSID для подключения к WiFi
//const char *password = "Acer5560g!"; // Пароль для подключения к WiFi
const char *ssid = "Keenetic-9461";
const char *password = "VAsbNoxP";
//------------ Создание объекта сервера на порту 80 ------------
ESP8266WebServer server(80);

//------------ Переменные для демонстрации ответа на GET запрос ------------

  int f1 = 1, f2 = 0, f3 = 1, pomp = 1, hot = 1;
  float temp=33.5; 
  int in1=33, in2 = 1, in3 = 0, in4 = 1;
  float setTemp = 0.0;
  float memTemp = 56.0;
//------------ Объявление прототипов функций ------------
void handlePost(); // Функция для обработки POST запросов
void handleGet();  // Функция для обработки GET запросов
void initWiFi();
void blinks(int, int, int8_t); // Функция для мигания светодиодом

//------------ Настройка устройства ------------
void setup(void)
{
  Serial.begin(115200);         // Начало серийной связи с бодрейтом 115200
  pinMode(WiFi_Status, OUTPUT); // Настройка пина статуса WiFi как выход
  digitalWrite(WiFi_Status, LOW); // Инициализация пина в низкое состояние

  initWiFi(); // Инициализация WiFi соединения

  server.on("/out", HTTP_GET, handleGet);  // Регистрация обработчика для GET запросов
  server.on("/in", HTTP_POST, handlePost); // Регистрация обработчика для POST запросов
  server.begin();                          // Запуск сервера
  Serial.println("HTTP server started");   // Вывод сообщения о запуске сервера
}

//------------ Функция для обработки POST запросов ------------
void handlePost()
{
  if (server.hasArg("plain") == false) // Проверка наличия данных в запросе
  {
    server.send(400, "text/plain", "Bad Request: Data not received"); // Ответ сервера с кодом 400
    return;
  }

  DynamicJsonDocument doc(1024);             // Создание JSON документа
  deserializeJson(doc, server.arg("plain")); // Десериализация данных JSON

  setTemp = doc["settemp"]; // Получение данных из JSON
  in2 = doc["vale2"];
  in3 = doc["vale3"];
  in4 = doc["vale4"];

  Serial.println("Received JSON data:"); // Вывод полученных данных в Serial Monitor
  Serial.print("setTemp: "); Serial.println(setTemp);
  Serial.print("in2: "); Serial.println(in2);
  Serial.print("in3: "); Serial.println(in3);
  Serial.print("in4: "); Serial.println(in4);

  server.send(200, "application/json", "{\"success\":true}"); // Отправка ответа с кодом 200
}

//------------ Функция для обработки GET запросов ------------
void handleGet()
{
  DynamicJsonDocument doc(1024);
  doc["temp"] = temp;
  doc["f1"] = f1;
  doc["f2"] = f2;
  doc["f3"] = f3;
  doc["pomp"] = pomp;
  doc["hot"] = hot;
  doc["memtemp"] = memTemp;
   Serial.println();
  Serial.print("temp: "); Serial.print(temp);
  Serial.print(", f1: "); Serial.print(f1);
  Serial.print(", f2: "); Serial.print(f2);
  Serial.print(", f3: "); Serial.print(f3);
  Serial.print(", pomp: "); Serial.print(pomp);
  Serial.print(", hot: "); Serial.print(hot);
  Serial.print(", memtemp: "); Serial.print(memTemp);
 String jsonResponse;
  if (serializeJson(doc, jsonResponse) == 0) // Сериализация JSON и проверка успешности
  {
    server.send(500, "text/plain", "Внутренняя ошибка сервера: Не удалось сериализовать JSON");
    return;
  }
  server.send(200, "application/json", jsonResponse); // Отправка сериализованного JSON
}

//------------ Функция для инициализации WiFi ------------
void initWiFi()
{
  unsigned long startTime = millis(); // Запись времени начала попытки подключения
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - startTime > 30000) // Проверка, не прошло ли 30 секунд
    {
      Serial.println("Не удалось подключиться к WiFi"); // Сообщение об ошибке
      return;
    }
    Serial.print(".");
    blinks(150, 50, 1); // Мигание светодиодом в процессе подключения
  }
  digitalWrite(WiFi_Status, HIGH);
  Serial.println("\nWiFi подключен");
    Serial.print("IP адрес: "); // Добавляем вывод IP-адреса
  Serial.println(WiFi.localIP()); // Выводим IP-адрес устройства
}

//------------ Петля обработки запросов к серверу ------------
void loop()
{
  server.handleClient(); // Обработка подключений к серверу
}

//------------ Функция для мигания светодиодом ------------
void blinks(int onTime, int offTime, int8_t repeat)
{
  for (int i = 0; i < repeat; i++)
  {
    digitalWrite(WiFi_Status, HIGH); // Включение светодиода
    delay(onTime);                   // Задержка включенного состояния
    digitalWrite(WiFi_Status, LOW);  // Выключение светодиода
    delay(offTime);                  // Задержка выключенного состояния
  }
}
