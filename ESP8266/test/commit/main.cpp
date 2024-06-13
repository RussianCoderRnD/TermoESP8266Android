//---------------- БИБЛИОТЕКИ START ---------------------
#include <Arduino.h>          // Основная библиотека для Arduino
#include <ESP8266WiFi.h>      // Библиотека для работы с WiFi на ESP8266
#include <ESP8266WebServer.h> // Библиотека для создания web сервера на ESP8266
#include <ArduinoJson.h>      // Библиотека для работы с JSON
#include <EEPROM.h>           // библиотека EEPROM
#include "GyverRelay.h"       // библиотека управления реле
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <GyverNTC.h>
#include <GyverButton.h> // библиотека кнопки
//----------------- БИБЛИОТЕКИ END-----------------------

//-------------------- DEFINE START ---------------------
#define WIFI_STATUS_PIN D0         // Пин для индикации статуса WiFi
#define INIT_ADDR 1023             // номер резервной ячейки. EEPROM
#define INIT_KEY 50                // ключ первого запуска. 0-254, на выбор. EEPROM
#define MOS_PIN D7                 // Пин выхода реле
#define PUMP_PIN D8                // Пин выхода реле
#define NTC_PIN A0                 // ПИН термо датчика NTC A0
#define WIFI_CONNECT_TIMEOUT 30000 // Таймаут подключения к WiFi
#define TEMP_HYSTERESIS 5          // Гистерезис температуры
#define FEEDBACK_COEFFICIENT_K 1   // коэффициент обратной связи (подбирается по факту)
#define TEMP_DT 500                // Время итерации регулятора
#define ALARM_TEMP 95              // Температура тревоги
#define EEPROM_WRITE_DELAY 1000    // Задержка перед записью в EEPROM
#define PUMP_SET_TEMP 20           //
#define MONITOR_SPEED 115200
#define INTERVAL 5000 // Интервал проскальзования в миллисекундах (10 секунд)
#define keyUP D5      // ПИН кнопки
#define keyDOWN D6    // ПИН кнопки
#define NUM_READ 10   // количество усреднений для средних арифм. фильтров
//-------------------- DEFINE END -----------------------

//----------------- CONSTANTS START ---------------------
const char *ssid = "Samsung 8S";     // SSID для подключения к WiFi
const char *password = "Acer5560g!"; // Пароль для подключения к WiFi
// const char *ssid = "Keenetic-9461";
// const char *password = "VAsbNoxP";

//----------------- CONSTANTS END -----------------------

//------------ СОЗДАНИЕ ЭКЗЕМПЛЯРА START ----------------
ESP8266WebServer server(80);
GyverRelay regulator(NORMAL);       // направление регулирования, либо GyverRelay regulator(); без указания направления (будет
LiquidCrystal_I2C lcd(0x27, 20, 4); // Устанавливаем дисплей
GyverNTC therm(NTC_PIN, 9870, 3950, 27, 10000);
GButton UP(keyUP);     // активация ПИНов кнопок для библиотеки
GButton DOWN(keyDOWN); // активация ПИНов кнопок для библиотеки
//------------- СОЗДАНИЕ ЭКЗЕМПЛЯРА END -----------------

//---------------- ПЕРЕМЕННЫЕ START----------------------
int f1 = 0, f2 = 0, f3 = 0, pomp = 0, hot = 0, temp = 0, in2 = 0, in3 = 0, in4 = 0;
float memtemp = 0.0;
uint32_t eepromTimer = 0;   // EEPROM таймер
boolean eepromFlag = false; // EEPROM флаг = 0
boolean check = false;
boolean onoff = false;
uint8_t alarmTemp = 95;
unsigned long previousMillis = 0; // Предыдущее время
//----------------- ПЕРЕМЕННЫЕ END ----------------------

//----------------- СПИСОК ФУНКЦИЙ START-----------------
void handlePost(); // Функция для обработки POST запросов
void handleGet();  // Функция для обработки GET запросов
void initWiFi();
void blinks(int, int, int8_t); // Функция для мигания светодиодом
void setBright();
void checkEEPROM();
void EEPROMRead();
void Pump();
void Mos();
void OTA();
void logicTermo();
float midArifm();
void Keys();
void setTempViev();
void Check();
//----------------- СПИСОК ФУНКЦИЙ END ------------------

//--------------------- SETUP START ---------------------
void setup(void)
{
  Serial.begin(MONITOR_SPEED); // Начало серийной связи с бодрейтом 115200

  pinMode(WIFI_STATUS_PIN, OUTPUT); // Настройка пина статуса WiFi как выход
  pinMode(MOS_PIN, OUTPUT);         // пин реле
  pinMode(PUMP_PIN, OUTPUT);        // пин реле

  digitalWrite(WIFI_STATUS_PIN, LOW); // Инициализация пина в низкое состояние

  lcd.backlight(); // Включаем подсветку дисплея
  lcd.init();
  Check();

  EEPROM.begin(2);          // активация функции EEPROM
  memtemp = EEPROM.read(0); // считывание из EEPROM значений

  EEPROMRead();

  initWiFi(); // Инициализация WiFi соединения

  OTA();

  server.on("/out", HTTP_GET, handleGet);  // Регистрация обработчика для GET запросов
  server.on("/in", HTTP_POST, handlePost); // Регистрация обработчика для POST запросов
  server.begin();                          // Запуск сервера
  Serial.println("HTTP server started");   // Вывод сообщения о запуске сервера
}
//--------------------- SETUP END -----------------------

//------------------- LOOP START ------------------------
void loop(void)
{
  checkEEPROM();         // проверка EEPROM
  server.handleClient(); // Обработка подключений к серверу

  ArduinoOTA.handle();

  UP.tick();
  DOWN.tick();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL)
  {
    // Сохраняем текущее время
    previousMillis = currentMillis;
    temp = midArifm(); // преобразуем в цельсии
  }

  logicTermo();

  lcd.setCursor(11, 0);
  lcd.print("Temp ");
  lcd.print(temp, 0);
  Keys();

  Check();
}
//--------------------- LOOP END ------------------------

//=====================blinks()==========================
void blinks(int onTime, int offTime, int8_t repeat)
{
  for (int i = 0; i < repeat; i++)
  {

    lcd.setCursor(19, 0);
    lcd.print("*");
    digitalWrite(WIFI_STATUS_PIN, HIGH); // Включение светодиода
    delay(onTime);
    lcd.setCursor(19, 0);
    lcd.print(" ");                     // Задержка включенного состояния
    digitalWrite(WIFI_STATUS_PIN, LOW); // Выключение светодиода
    delay(offTime);                     // Задержка выключенного состояния
  }
}

//====================setBright==========================
void setBright(void)
{
  memtemp = constrain(memtemp, 0, 100); // ограничили от 0 до 100
  eepromFlag = true;                    // поднять флаг
  eepromTimer = millis();               // сбросить таймер
}

//===================checkEEPROM=========================
void checkEEPROM(void)
{
  if (eepromFlag && (millis() - eepromTimer >= EEPROM_WRITE_DELAY))
  {                           // если флаг поднят и с последнего нажатия прошло 10 секунд (10 000 мс)
    eepromFlag = false;       // опустили флаг
    EEPROM.write(0, memtemp); // записали в EEPROM
    EEPROM.commit();
    Serial.print("\ncheckEEPROM(");
    Serial.print(memtemp);
    Serial.println(")\n");
    setTempViev();
  }
}

//====================EEPROMRead=========================
void EEPROMRead(void)
{
  if (EEPROM.read(INIT_ADDR) != INIT_KEY)
  {                                    // первый запуск (ЕСЛИ INIT_ADDR (1023)не равен INIT_KEY (50) то записать EEPROM.write(INIT_ADDR, INIT_KEY);EEPROM.put(0, izmenenieTemp);
    EEPROM.write(INIT_ADDR, INIT_KEY); // записали ключ
    EEPROM.write(0, memtemp);          // записали стандартное значение температуры. в данном случае это значение переменной, объявленное выше

    EEPROM.commit();
  }
  memtemp = EEPROM.read(0); // прочитали температуру
  lcd.setCursor(0, 0);
  lcd.print("setTemp ");
  lcd.print(memtemp, 0);
}

//===================Pump()==============================
void Pump(void)
{
  if (onoff)
  {
  if (check != 0)
  {
    pomp = 0;
  }

  if (temp > PUMP_SET_TEMP)
  {
    digitalWrite(PUMP_PIN, HIGH);
    pomp = 1;
    lcd.setCursor(8, 3);
    lcd.print("PUMP ON ");
  }
  else
  {
    digitalWrite(PUMP_PIN, LOW);
    pomp = 0;
    lcd.setCursor(8, 3);
    lcd.print("PUMP OFF");
  }
  }
}

//=====================Mos()=============================
void Mos(void)
{
  if (temp > alarmTemp)
  {
    digitalWrite(MOS_PIN, LOW);
    onoff=false;
    hot = 0;
    lcd.setCursor(0, 0);
    lcd.print("#####################");
    lcd.setCursor(0, 2);
    lcd.print("#                   #");
    lcd.setCursor(0, 3);
    lcd.print("#####################");
    lcd.setCursor(0, 1);
    lcd.print("# Temp ");
    lcd.print(midArifm(), 0);
    lcd.setCursor(19, 1);
    lcd.print("#");
    lcd.setCursor(0, 2);
    lcd.print("#      ALARM!!!    #");
    Check();
    delay(500);
    lcd.noBacklight(); // Выключаем подсветку дисплея

    delay(500);
    lcd.backlight(); // Включаем подсветку дисплея
  }
  else
  {
    onoff=true;
    digitalWrite(MOS_PIN, !regulator.getResult());
    hot = !regulator.getResult();
    if (!regulator.getResult())
    {
      lcd.setCursor(0, 3);
      lcd.print("HOT ON ");
    }
    else
    {
      lcd.setCursor(0, 3);
      lcd.print("HOT OFF");
    }
  }
}

//===================initWiFi()==========================
void initWiFi(void)
{
  int i = 0;

  unsigned long startTime = millis(); // Запись времени начала попытки подключения
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - startTime > WIFI_CONNECT_TIMEOUT) // Проверка, не прошло ли 30 секунд
    {
      Serial.println("Не удалось подключиться к WiFi"); // Сообщение об ошибке
      return;
    }
    Serial.print(".");
    blinks(150, 50, 1); // Мигание светодиодом в процессе подключения
    i++;
    lcd.setCursor(0, 1);
    lcd.print("WiFi ");
    lcd.print(i);
  }
  digitalWrite(WIFI_STATUS_PIN, HIGH);
  lcd.print(" OK");
  lcd.setCursor(0, 2);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());
  Serial.println("\nWiFi подключен");
  Serial.print("IP адрес: ");     // Добавляем вывод IP-адреса
  Serial.println(WiFi.localIP()); // Выводим IP-адрес устройства
}

//==================handlePost() in======================
void handlePost(void)
{
  if (server.hasArg("plain") == false) // Проверка наличия данных в запросе
  {
    server.send(400, "text/plain", "Bad Request: Data not received"); // Ответ сервера с кодом 400
    return;
  }

  DynamicJsonDocument doc(1024);             // Создание JSON документа
  deserializeJson(doc, server.arg("plain")); // Десериализация данных JSON

  memtemp = doc["memtemp"]; // Получение данных из JSON
  in2 = doc["vale2"];
  in3 = doc["vale3"];
  in4 = doc["vale4"];

  Serial.println("\nReceived JSON data:"); // Вывод полученных данных в Serial Monitor
  Serial.print("memtemp: ");
  Serial.println(memtemp);
  Serial.print("in2: ");
  Serial.println(in2);
  Serial.print("in3: ");
  Serial.println(in3);
  Serial.print("in4: ");
  Serial.println(in4);
  setBright();
  blinks(150, 50, 1);                                         // Мигание светодиодом в процессе подключения
  server.send(200, "application/json", "{\"success\":true}"); // Отправка ответа с кодом 200
}

//===================handleGet() out=====================
void handleGet(void)
{
  DynamicJsonDocument doc(1024);
  doc["temp"] = temp;
  doc["f1"] = f1;
  doc["f2"] = f2;
  doc["f3"] = f3;
  doc["pomp"] = pomp;
  doc["hot"] = hot;
  doc["memtemp"] = memtemp;
  Serial.println();
  Serial.print("temp: ");
  Serial.print(temp);
  Serial.print(", f1: ");
  Serial.print(f1);
  Serial.print(", f2: ");
  Serial.print(f2);
  Serial.print(", f3: ");
  Serial.print(f3);
  Serial.print(", pomp: ");
  Serial.print(pomp);
  Serial.print(", hot: ");
  Serial.print(hot);
  Serial.print(", memtemp: ");
  Serial.print(memtemp);
  String jsonResponse;
  if (serializeJson(doc, jsonResponse) == 0) // Сериализация JSON и проверка успешности
  {
    server.send(500, "text/plain", "Внутренняя ошибка сервера: Не удалось сериализовать JSON");
    return;
  }
  server.send(200, "application/json", jsonResponse); // Отправка сериализованного JSON
  blinks(50, 150, 1);                                 // Мигание светодиодом в процессе подключения
}

//=======================OTA()===========================
void OTA()
{
  ArduinoOTA.onStart([]()
                     {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();
}

//===================logicTermo()========================
void logicTermo()
{
  //////////////////////////////////////////////////////////////////
  //                    логика работы реле                        //
  //////////////////////////////////////////////////////////////////
  regulator.hysteresis = TEMP_HYSTERESIS; // ширина гистерезиса
  regulator.k = FEEDBACK_COEFFICIENT_K;   // коэффициент обратной связи (подбирается по факту)
  regulator.dT = TEMP_DT;                 // установить время итерации для getResultTimer
  regulator.setpoint = memtemp;           // установка температуры
  regulator.input = temp;                 // сообщаем регулятору текущую температуру

  Mos();
  Pump();
}

//===================midArifm()==========================
// обычное среднее арифметическое для float
float midArifm()
{
  float sum = 0;                     // локальная переменная sum
  for (int i = 0; i < NUM_READ; i++) // согласно количеству усреднений
    sum += therm.getTempAverage();   // суммируем значения с любого датчика в переменную sum
  return (sum / NUM_READ);
}

//======================Keys()===========================
void Keys()
{

  if (UP.isRelease())
  {
    memtemp++;
    setTempViev();
    setBright();
  }
  if (DOWN.isRelease())
  {
    memtemp--;
    setTempViev();
    setBright();
  }
}

//===================setTempViev()=======================
void setTempViev()
{
  lcd.setCursor(0, 0);
  lcd.print("setTemp ");
  lcd.print(memtemp, 0);
}

//===================setTempViev()=======================
void Check(void)
{
  int testNTC = analogRead(NTC_PIN);

  if (testNTC >= 50 && testNTC <= 600)
  {
    check = 0;
    Serial.println(testNTC);

    lcd.setCursor(12, 1);
    lcd.print("NTC OK ");
  }

  if (testNTC > 1000)
  {
    check = 2;
    Serial.println(testNTC);

    lcd.setCursor(12, 1);
    lcd.print("NTC OBR");
  }
  if (testNTC < 50)
  {
    check = 1;
    Serial.println(testNTC);

    lcd.setCursor(12, 1);
    lcd.print("NTC KZ ");
  }
}

//===================== END =============================