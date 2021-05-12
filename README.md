# evap-duct-sense

В этом репозитории представлен следующий набор программ для прототипа системы беспроводных интеллектуальных датчиков для мониторинга параметров волновода испарения:


* передатчик LoRa

    * [BME280_LoRa_Transmitter](BME280_LoRa_Transmitter) - программа для передачи данных с одиночного датчика BME280 через протокол LoRa. Использует библиотеки *SPI*, *RH_RF95* и *Adafruit_BME280*.
    * [BME280_LoRa_Transmitter_8ch](BME280_LoRa_Transmitter_8ch) - программа для одновременной передачи данных с 8 датчиков BME280 через протокол LoRa. Использует библиотеки *SPI*, *RH_RF95*, *DeepSleepScheduler* и *Adafruit_BME280*.
    
* приемник LoRa

    * [BME280_LoRa_Receiver](BME280_LoRa_Receiver) - программа для приема данных через протокол LoRa с одиночного датчика и их отображения на OLED экране. Использует библиотеки *SPI*, *RH_RF95*, *U8glib*.
    * [BME280_LoRa_Receiver_8ch](BME280_LoRa_Receiver_8ch) - программа для приема данных через протокол LoRa одновременно с 8 датчиков и отображения данных на OLED экране. Использует библиотеки *SPI*, *RH_RF95*, *U8glib*.

* вспомогательные тестовые программы для шлюза LoRa-4G

    * [Uno_ThingSpeak_send_emu](Uno_ThingSpeak_send_emu) - программа для микроконтроллера Atmega328P LoRa модуля внутри шлюза LG01 для подготовки тестовых данных, сохранения их в локальный файл и их отправки на сервер ThinkSpeak из OpenWRT с помощью программы Curl. Использует библиотеки *Process* и *FileIO*.
    * [ThingSpeak_send_LG01](ThingSpeak_send_LG0) - измененный вариант программы [Uno_ThingSpeak_send_emu](Uno_ThingSpeak_send_emu). Дополнительно использует библиотеку *Console* для вывода отладочной информации в консоль.

* шлюз LoRa-4G

    * [BME280_LoRa_Receiver_LG01](BME280_LoRa_Receiver_LG01) - программа для приема данных через протокол LoRa на шлюз LG01 с одиночного датчика, их отображения на экране и отправки на сервер ThinkSpeak из OpenWRT с помощью программы Curl  и записи в CSV-файл на USB-накопителе. Использует библиотеки *SPI*, *RH_RF95*, *U8glib*, *Console*, *Process* и *FileIO*.
    * [BME280_LoRa_Receiver_LG01_8ch](BME280_LoRa_Receiver_LG01_8ch) - программа для приема данных через протокол LoRa на шлюз LG01 одновременно с 8 датчиков, их опционального отображения на OLED экране и отправки на сервер ThinkSpeak из OpenWRT с помощью программы Curl и записи в CSV-файл на USB-накопителе. Использует библиотеки *SPI*, *RH_RF95*, *U8glib*, *Console*, *Process* и *FileIO*.

Аппаратное обеспечение прототипа передатчика:

* Интегрированный датчик температуры, давления, влажности BME280
* [SX1278 LoRa ATmega328P RFM98 433 MHz Radio Node Module](https://www.elecrow.com/sx1278-lora-atmega328p-rfm98-433-915mhz-radio-node-module.html)
* I2C мультиплексор TCA9548 (для реализации 8 каналов)
* Аккумулятор формата 14500 или большего размера и емкости с разъемом KF2510
* Герметичный корпус IP66

Аппаратное обеспечение прототипа приемника:

* [SX1278 LoRa ATmega328P RFM98 433 MHz Radio Node Module](https://www.elecrow.com/sx1278-lora-atmega328p-rfm98-433-915mhz-radio-node-module.html)
* OLED экран на контроллере SSD1306
* Аккумулятор формата 14500 или большего размера и емкости с разъемом KF2510
* Прозрачный герметичный корпус IP66

Аппаратное обеспечение LoRa-4G шлюза:

* [LG01-P IoT LoRa Gateway](https://www.dragino.com/products/lora/item/117-lg01-p.html)
* OLED экран на контроллере SSD1306
* SIM-карта для мобильного интернета
* Адаптер питания сети 220В или аккумулятор большой емкости.
