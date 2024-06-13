package com.example.termoesp8266android

import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import okhttp3.*
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.RequestBody.Companion.toRequestBody
import org.json.JSONObject
import java.io.IOException
import java.util.concurrent.TimeUnit
import android.content.pm.ActivityInfo
import android.widget.ImageButton
import android.widget.ImageView
import android.os.Handler
import android.os.Looper
import android.widget.ProgressBar
import android.animation.ArgbEvaluator
import android.graphics.PorterDuff
import androidx.core.content.ContextCompat
import android.widget.Toast

var jsonString = "" // Глобальная переменная для хранения JSON строки
var count = 0 // установленная температура
var memtemp = 0.0f
var counPower = 0

class MainActivity : AppCompatActivity() {

    private lateinit var progressBar: ProgressBar

    // Инициализация переменных для значений JSON объекта
    private var vale2: Int = 0
    private var vale3: Int = 0
    private var vale4: Int = 0
    private var temp: Int = 0
    private var f1: Double = 0.0
    private var f2: Int = 0
    private var f3: Int = 0
    private var pomp: Int = 0
    private var hot: Int = 0

    companion object {
        // Статические URL для вашего ESP8266
        const val espUrl = "http://192.168.153.247/in"
        const val espUrlOut = "http://192.168.153.247/out"
    }

    private lateinit var imageViewFaza1: ImageView
    private lateinit var imageViewFaza2: ImageView
    private lateinit var imageViewFaza3: ImageView
    private lateinit var imageViewHotOnOff: ImageView
    private lateinit var imageViewPompOnOff: ImageView
    private lateinit var imageButtonPowerOnOff: ImageView

    private lateinit var textViewOut1: TextView
    private lateinit var textViewOut2: TextView
    private lateinit var textViewOut3: TextView
    private lateinit var textViewOut4: TextView
    private lateinit var textViewOut5: TextView

    private lateinit var textViewIn1: TextView
    private lateinit var textViewIn2: TextView
    private lateinit var textViewIn3: TextView
    private lateinit var textViewIn4: TextView
    private lateinit var textViewTemp: TextView
    private lateinit var textViewSetTemperature: TextView

    // Handler для выполнения задач с задержкой
    private val handler = Handler(Looper.getMainLooper())

    // Runnable для периодического выполнения fetchDataFromESP8266
    private val fetchDataRunnable = object : Runnable {
        override fun run() {
            receiveJsonFromEsp8266()
//            if (hot == 1) imageViewHotOnOff.setImageResource(R.drawable.lampon)
//            if (hot == 0)   imageViewHotOnOff.setImageResource(R.drawable.lampoff)
//
//            if (pomp == 1) imageViewPompOnOff.setImageResource(R.drawable.lampon)
//            if (pomp == 0)   imageViewPompOnOff.setImageResource(R.drawable.lampoff)

            handler.postDelayed(this, 30000)
        }
    }

    // Таймауты для HTTP соединений
    private val connectTimeout: Long = 10 // секунд
    private val readTimeout: Long = 15 // секунд
    private val writeTimeout: Long = 15 // секунд

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main) // Установка layout для активности

        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE // Устанавливаем ориентацию на ландшафтную

        // Инициализация элементов интерфейса
        progressBar = findViewById(R.id.progressBar)
        imageButtonPowerOnOff = findViewById(R.id.imageButtonPowerOnOff)
        imageButtonPowerOnOff.setBackgroundColor(android.graphics.Color.TRANSPARENT)

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
        textViewSetTemperature = findViewById(R.id.textViewSetTemp)

        imageViewFaza1 = findViewById(R.id.imageViewFaza1)
        imageViewFaza2 = findViewById(R.id.imageViewFaza2)
        imageViewFaza3 = findViewById(R.id.imageViewFaza3)
        imageViewHotOnOff = findViewById(R.id.imageViewHotOnOff)
        imageViewPompOnOff = findViewById(R.id.imageViewPompOnOff)

        imageViewFaza1.setImageResource(R.drawable.off)
        imageViewFaza2.setImageResource(R.drawable.off)
        imageViewFaza3.setImageResource(R.drawable.off)
        imageViewHotOnOff.setImageResource(R.drawable.lampoff)
        imageViewPompOnOff.setImageResource(R.drawable.lampoff)



        // Запуск Runnable
        handler.post(fetchDataRunnable)

        findViewById<Button>(R.id.button1).setOnClickListener {
            if (counPower == 1) {
                receiveJsonFromEsp8266()

                textViewTemp.text = temp.toString()
                generateAndPrintJson()
            }
        }

        imageButtonPowerOnOff.setOnClickListener {
            textViewSetTemperature.text = "$count"
            counPower++
            if (counPower == 1) {
                imageButtonPowerOnOff.setImageResource(R.drawable.on1)
                receiveJsonFromEsp8266()
            } else {
                imageButtonPowerOnOff.setImageResource(R.drawable.off1)
                counPower = 0
            }
        }

        findViewById<ImageButton>(R.id.imageButtonUp).setOnClickListener {
            if (counPower == 1) {
                count++
                textViewSetTemperature.text = "$count"
                if (count > 129) {
                    count = 130
                }
            }
        }

        findViewById<ImageButton>(R.id.imageButtonDown).setOnClickListener {
            if (counPower == 1) {
                count--
                if (count < 1) {
                    count = 0
                }
                textViewSetTemperature.text = "$count"
            }
        }
    }

    private fun generateAndPrintJson() {
        jsonString = createJsonString()
        printJson(jsonString)
        sendJsonToEsp8266(jsonString)
    }

    private fun createJsonString(): String {
        return JSONObject().apply {
            put("memtemp", count)
            put("vale2", vale2)
            put("vale3", vale3)
            put("vale4", vale4)
        }.toString()
    }

    private fun printJson(jsonString: String) {
        println("Строка JSON: $jsonString")
    }

    private fun sendJsonToEsp8266(jsonString: String) {
        val requestBody = jsonString.toRequestBody("application/json; charset=utf-8".toMediaTypeOrNull())
        val client = OkHttpClient.Builder()
            .connectTimeout(connectTimeout, TimeUnit.SECONDS)
            .readTimeout(readTimeout, TimeUnit.SECONDS)
            .writeTimeout(writeTimeout, TimeUnit.SECONDS)
            .build()

        val request = Request.Builder()
            .url(espUrl)
            .post(requestBody)
            .build()

        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                println("Ошибка отправки JSON: ${e.message}")
            }

            override fun onResponse(call: Call, response: Response) {
                response.use { resp ->
                    if (resp.isSuccessful) {
                        val responseBody = resp.body?.string()
                        println("JSON успешно отправлен, получен ответ: $responseBody")
                    } else {
                        println("Не удалось отправить JSON, код ответа: ${resp.code}")
                    }
                }
            }
        })

        updateProgressBarColor(temp.toFloat())
    }

    private fun receiveJsonFromEsp8266() {
        if (counPower == 1) {
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
                    println("Ошибка получения JSON: ${e.message}")
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
                                val f1in = jsonObject.getDouble("f1")
                                val f2in = jsonObject.getInt("f2")
                                val f3in = jsonObject.getInt("f3")
                                val pomps = jsonObject.getInt("pomp")
                                val hots = jsonObject.getInt("hot")
                                temp = tempin.toInt()
                                f1 = f1in
                                f2 = f2in
                                f3 = f3in
                                pomp = pomps
                                hot = hots
                                memtemp = memtemps.toFloat()
                                runOnUiThread {
                                    count = memtemp.toInt()
                                    progressBar.progress = temp.toInt()
                                    receiveJsonFromEsp8266TextView()
                                }
                            }
                            } else {
                            println("Не удалось получить JSON, код ответа: ${resp.code}")
                            runOnUiThread {
                                showErrorNotification("Ошибка: код ответа ${resp.code}")
                            }
                        }
                    }
                }
            })
        }
    }

    private fun receiveJsonFromEsp8266TextView() {
        textViewIn1.text = temp.toString()
        textViewIn2.text = memtemp.toString()
        textViewIn3.text = f2.toString()
        textViewIn4.text = f3.toString()
        textViewTemp.text = temp.toString()
        textViewSetTemperature.text = memtemp.toString()
        if (hot == 1) imageViewHotOnOff.setImageResource(R.drawable.lampon)
        if (hot == 0)   imageViewHotOnOff.setImageResource(R.drawable.lampoff)

        if (pomp == 1) imageViewPompOnOff.setImageResource(R.drawable.lampon)
        if (pomp == 0)   imageViewPompOnOff.setImageResource(R.drawable.lampoff)
        println("hot/pomp")
    }

    private fun updateProgressBarColor(progress: Float) {
        val colorFrom = ContextCompat.getColor(this, android.R.color.holo_blue_light)
        val colorTo = ContextCompat.getColor(this, android.R.color.holo_red_light)
        val evaluator = ArgbEvaluator()
        val fraction = progress / 130f
        val color = evaluator.evaluate(fraction, colorFrom, colorTo) as Int
        progressBar.progressDrawable.setColorFilter(color, PorterDuff.Mode.SRC_IN)
    }

    private fun showErrorNotification(message: String) {
        // Visual notification
        Toast.makeText(this, message, Toast.LENGTH_LONG).show()
    }
}
