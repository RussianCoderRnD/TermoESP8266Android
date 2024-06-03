package com.example.termoesp

// Импорты необходимых классов
import android.os.Bundle // Импорт класса Bundle для передачи данных между составляющими приложения
import android.widget.Button // Импорт класса Button для работы с кнопками на пользовательском интерфейсе
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity // Импорт базового класса для создания активности
import okhttp3.* // Импорт всех классов из библиотеки OkHttp для работы с сетевыми запросами
import okhttp3.MediaType.Companion.toMediaTypeOrNull // Импорт метода для преобразования строки в MediaType для указания типа содержимого в теле запроса
import okhttp3.RequestBody.Companion.toRequestBody // Импорт метода для создания объекта RequestBody из строки
import org.json.JSONObject // Импорт класса JSONObject для создания и работы с JSON объектами
import java.io.IOException // Импорт класса IOException для обработки исключений ввода-вывода
import java.util.concurrent.TimeUnit // Импорт класса TimeUnit для указания единиц измерения времени (используется в настройках тайм-аутов)
import kotlin.random.Random
import android.content.pm.ActivityInfo // Класс для работы с информацией об активности, в данном случае для задания ориентации экрана
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.widget.ImageButton
import android.widget.ImageView


var jsonString="" // Глобальная переменная для хранения JSON строки
var count = 0f //установленная температура
var memtemp = 0.0f

class MainActivity : AppCompatActivity() {

    // Инициализация переменных для значений JSON объекта
    private var vale2: Int = 0
    private var vale3: Int = 0
    private var vale4: Int = 0

    private var temp = 0f
    private var f1: Int = 0
    private var f2: Int = 0
    private var f3: Int = 0
    private var pomp: Int = 0
    private var hot: Int = 0

    // URL к вашему ESP8266
    private val espUrl = "http://192.168.14.83/in"
    private val espUrlOut = "http://192.168.14.83/out"

    private lateinit var imageViewFaza1: ImageView
    private lateinit var imageViewFaza2: ImageView
    private lateinit var imageViewFaza3: ImageView
    private lateinit var imageViewHotOnOff: ImageView
    private lateinit var imageViewPompOnOff: ImageView
    private lateinit var imageButtonPowerOnOff: ImageView

    private lateinit var textViewOut1: TextView // Объявление переменной для TextView
    private lateinit var textViewOut2: TextView // Объявление переменной для TextView
    private lateinit var textViewOut3: TextView // Объявление переменной для TextView
    private lateinit var textViewOut4: TextView // Объявление переменной для TextView
    private lateinit var textViewOut5: TextView // Объявление переменной для TextView

    private lateinit var textViewIn1: TextView // Объявление переменной для TextView
    private lateinit var textViewIn2: TextView // Объявление переменной для TextView
    private lateinit var textViewIn3: TextView // Объявление переменной для TextView
    private lateinit var textViewIn4: TextView // Объявление переменной для TextView
    private lateinit var textViewTemp: TextView // Объявление переменной для TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main) // Установка layout для активности
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE // Устанавливаем ориентацию на ландшафтную


        // Инициализация TextView
        textViewOut1 = findViewById(R.id.textViewOut1)
        textViewOut2 = findViewById(R.id.textViewOut2)
        textViewOut3 = findViewById(R.id.textViewOut3)
        textViewOut4 = findViewById(R.id.textViewOut4)
        textViewOut5 = findViewById(R.id.textViewOut5)

        textViewIn1 = findViewById(R.id.textViewIn1)
        textViewIn2 = findViewById(R.id.textViewIn2)
        textViewIn3 = findViewById(R.id.textViewIn3)
        textViewIn4 = findViewById(R.id.textViewIn4)

        textViewTemp = findViewById(R.id.textViewTemp)


        // Пример вызова функции для отображения подвыборки изображения
       imageViewFaza1 = findViewById<ImageView>(R.id.imageViewFaza1)
        imageViewFaza2 = findViewById<ImageView>(R.id.imageViewFaza2)
        imageViewFaza3 = findViewById<ImageView>(R.id.imageViewFaza3)
        imageViewHotOnOff = findViewById<ImageView>(R.id.imageViewFazaOnOff)
        imageViewPompOnOff = findViewById<ImageView>(R.id.imageViewPompOnOff)
        imageButtonPowerOnOff = findViewById<ImageView>(R.id.imageButtonPowerOnOff)

        // Получение TextView по ID
        val textViewSetTemp = findViewById<TextView>(R.id.textViewSetTemp)

        // Установите изображение из ресурсов
        imageViewFaza1.setImageResource(R.drawable.off)
        imageViewFaza2.setImageResource(R.drawable.off)
        imageViewFaza3.setImageResource(R.drawable.off)
        imageViewHotOnOff.setImageResource(R.drawable.lampoff)
        imageViewPompOnOff.setImageResource(R.drawable.lampoff)

        var counPower = 0;
        mains(f1, f2, f3, pomp, hot)
        findViewById<Button>(R.id.button1).setOnClickListener {
            if (counPower == 1) {
                mains(f1, f2, f3, pomp, hot)
                // receiveJsonFromEsp8266() // Получение JSON строки от ESP8266 при клике
                // Установка значения переменной в TextView
                textViewTemp.text = "$temp"
                generateAndPrintJson() // Генерация и вывод JSON строки при клике

            }

        }

        findViewById<ImageButton>(R.id.imageButtonPowerOnOff).setOnClickListener {
            count = memtemp
            // Установка значения переменной в TextView
            textViewSetTemp.text = "$count"
            counPower++
            if (counPower ==1 ){imageButtonPowerOnOff.setImageResource(R.drawable.on1)
                mains(f1, f2, f3, pomp, hot)
            }
            else {imageButtonPowerOnOff.setImageResource(R.drawable.off1)
                counPower = 0}
        }


        findViewById<ImageButton>(R.id.imageButtonUp).setOnClickListener {
            if (counPower == 1) {
                count++
                // Установка значения переменной в TextView
                textViewSetTemp.text = "$count"
            }
        }

        findViewById<ImageButton>(R.id.imageButtonDown).setOnClickListener {
            if (counPower == 1) {
            count--
            textViewSetTemp.text = "$count"
        }
}
    }

    private fun generateAndPrintJson() {
        jsonString = createJsonString() // Создание JSON строки
        printJson(jsonString) // Вывод JSON строки в консоль
        sendJsonToEsp8266(jsonString) // Отправка JSON строки на ESP8266 при клике
    }

    private fun createJsonString(): String {
        textViewOut1.text = count.toString() // Отображение JSON строки в TextView
        textViewOut2.text = vale2.toString() // Отображение JSON строки в TextView
        textViewOut3.text = vale3.toString() // Отображение JSON строки в TextView
        textViewOut4.text = vale4.toString() // Отображение JSON строки в TextView
        return JSONObject().apply {
            put("settemp", count) // Добавление значения count
            put("vale2", vale2) // Добавление значения vale2
            put("vale3", vale3) // Добавление значения vale3
            put("vale4", vale4) // Добавление значения vale4
        }.toString() // Преобразование в строку JSON

    }

    private fun printJson(jsonString: String) {
        println("Строка JSON: $jsonString") // Вывод строки JSON в консоль
    }

    private fun sendJsonToEsp8266(jsonString: String) {
        val requestBody = jsonString.toRequestBody("application/json; charset=utf-8".toMediaTypeOrNull()) // Создание тела запроса

        val client = OkHttpClient.Builder() // Настройка клиента OkHttp
            .connectTimeout(30, TimeUnit.SECONDS) // Установка таймаута соединения
            .readTimeout(30, TimeUnit.SECONDS) // Установка таймаута чтения
            .writeTimeout(30, TimeUnit.SECONDS) // Установка таймаута записи
            .build()

        val request = Request.Builder() // Создание запроса
            .url(espUrl) // Установка URL
            .post(requestBody) // Установка метода POST и тела запроса
            .build()

        client.newCall(request).enqueue(object : Callback { // Асинхронный вызов запроса
            override fun onFailure(call: Call, e: IOException) { // Обработка ошибки запроса
                println("Ошибка отправки JSON: ${e.message}") // Вывод сообщения об ошибке
            }

            override fun onResponse(call: Call, response: Response) { // Обработка ответа сервера
                response.use { resp ->
                    if (resp.isSuccessful) { // Проверка успешности ответа
                        val responseBody = resp.body?.string() // Чтение тела ответа

                        println("JSON успешно отправлен, получен ответ: $responseBody") // Вывод успешного сообщения
                    } else {
                        println("Не удалось отправить JSON, код ответа: ${resp.code}") // Вывод ошибки ответа
                    }
                }
            }
        })

    }

    private fun receiveJsonFromEsp8266() {
        val client = OkHttpClient.Builder()
            .connectTimeout(30, TimeUnit.SECONDS)
            .readTimeout(30, TimeUnit.SECONDS)
            .writeTimeout(30, TimeUnit.SECONDS)
            .build()

        val request = Request.Builder()
            .url(espUrlOut)
            .get()
            .build()

        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                println("Ошибка получения JSON: ${e.message}") // Вывод сообщения об ошибке
            }

            override fun onResponse(call: Call, response: Response) {
                response.use { resp ->
                    if (resp.isSuccessful) {
                        val responseBody = resp.body?.string()
                        println("JSON успешно получен: $responseBody")

                        responseBody?.let {
                            val jsonObject = JSONObject(it)
                            val tempin = jsonObject.getDouble("temp")
                            val memtemps = jsonObject.getDouble("memtemp")
                            val f1in = jsonObject.getInt("f1")
                            val f2in = jsonObject.getInt("f2")
                            val f3in = jsonObject.getInt("f3")
                            val pomps = jsonObject.getInt("pomp")
                            val hots = jsonObject.getInt("hot")
                            temp=tempin.toFloat()
                            f1=f1in
                            f2=f2in
                            f3=f3in
                            pomp=pomps
                            hot=hots
                            memtemp=memtemps.toFloat()
                            runOnUiThread {
                                receiveJsonFromEsp8266TextViwe()
                            }
                        }

                    } else {
                        println("Не удалось получить JSON, код ответа: ${resp.code}")
                    }
                }

            }
        })

    }
    private fun receiveJsonFromEsp8266TextViwe() {
        textViewIn1.text = temp.toString() // Отображение JSON строки в TextView
        textViewIn2.text = f1.toString() // Отображение JSON строки в TextView
        textViewIn3.text = f2.toString() // Отображение JSON строки в TextView
        textViewIn4.text = f3.toString() // Отображение JSON строки в TextView
    }

    /**
     * Функция для отображения подвыборки изображения в ImageView.
     * @param imageView Элемент интерфейса, в котором будет отображаться подвыборка изображения.
     * @param imageResId ID ресурса изображения.
     * @param x Начальная координата X подвыборки.
     * @param y Начальная координата Y подвыборки.
     * @param width Ширина подвыборки.
     * @param height Высота подвыборки.
     */
    private fun displayImageRegionFaza1(imageView: ImageView, imageResId: Int, x: Int, y: Int, width: Int, height: Int) {
        val options = BitmapFactory.Options()
        options.inJustDecodeBounds = true

        // Получаем размеры исходного изображения
        BitmapFactory.decodeResource(resources, imageResId, options)

        // Убеждаемся, что заданная область находится в пределах изображения
        val imageWidth = options.outWidth
        val imageHeight = options.outHeight

        if (x + width > imageWidth || y + height > imageHeight) {
            throw IllegalArgumentException("Область подвыборки выходит за пределы изображения.")
        }

        // Загружаем изображение
        options.inJustDecodeBounds = false
        val fullImage = BitmapFactory.decodeResource(resources, imageResId, options)

        // Создаем подвыборку изображения
        val region = Bitmap.createBitmap(fullImage, x, y, width, height)

        // Отображаем подвыборку в ImageView
        imageView.setImageBitmap(region)
    }

    private fun mains(f1: Int, f2:Int,f3:Int, pomp:Int, hot:Int) {
        receiveJsonFromEsp8266() // Получение JSON строки от ESP8266 при клике
        // Установка значения переменной в TextView
        textViewTemp.text = "$temp"
        if (f1 == 1) {imageViewFaza1.setImageResource(R.drawable.on)}
        else {imageViewFaza1.setImageResource(R.drawable.off)}

        if (f2 ==1){ imageViewFaza2.setImageResource(R.drawable.on) }
        else{ imageViewFaza2.setImageResource(R.drawable.off) }

        if (f3 ==1){imageViewFaza3.setImageResource(R.drawable.on)}
        else {imageViewFaza3.setImageResource(R.drawable.off)}

        if (pomp ==1){imageViewHotOnOff.setImageResource(R.drawable.lampon)}
        else{imageViewHotOnOff.setImageResource(R.drawable.lampoff)}

        if (hot ==1){imageViewPompOnOff.setImageResource(R.drawable.lampon)}
        else {imageViewPompOnOff.setImageResource(R.drawable.lampoff)}

//
//
    }

}
