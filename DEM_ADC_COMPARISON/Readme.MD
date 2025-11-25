# STM32 ADC Okuma Modları - Algoritma Akışı
# STM32 ADC Reading Modes - Algorithm Flow

---

## 🇹🇷 Türkçe

Bu STM32 uygulaması, ADC'den (Analog-Dijital Çevirici) veri okumak için dört farklı yöntemi gösterir.
Hangi yöntemin kullanılacağı, `ADC_MODE_SELECTION` adlı bir makro ile derleme zamanında seçilir.
Tüm modlar, dört ADC kanalını okur ve sonuçları bir diziye (ADC_VAL) kaydeder.

**Mod 1 (Polling - Sorgulama):**
Ana döngü (`while(1)`) tamamen engellenir (blocking).
Döngü, dört kanalın her birini tek tek başlatır.
Her bir dönüşümün bitmesini aktif olarak bekler ve sonucu diziye yazar.

**Mod 2 (DMA Normal):**
Ana döngü, 4 kanallık bir DMA transferi başlatır.
DMA, verileri arka planda alırken ana döngü serbesttir.
Transfer tamamlandığında, bir kesme (interrupt) fonksiyonu `isADCFinished` bayrağını ayarlar.
Ana döngü bu bayrağı görür, veriyi işler ve bir sonraki okuma için DMA'yı manuel olarak yeniden başlatır.

**Mod 3 (DMA Dairesel - Circular):**
DMA sadece bir kez başlatılır.
Dört kanalı okur ve işlemi tamamladığında otomatik olarak yeniden başlar.
Ana döngü bu sürece hiç müdahale etmez ve her zaman güncel verileri diziden okuyabilir.
Bir kesme fonksiyonu, her tamamlandığında sadece bir LED yakıp söndürür.

**Mod 4 (Zamanlayıcı Tetiklemeli - Timer Triggered):**
Bir donanım zamanlayıcısı (TIM1), ADC'yi tetiklemek için ayarlanır.
Ana döngü veya CPU müdahalesi olmadan, zamanlayıcı periyodik olarak ADC dönüşümünü başlatır.
DMA, bu tetiklenen verileri otomatik olarak arka planda diziye aktarır.
Ana döngü tamamen serbest kalır.

---

## 🇬🇧 English

This STM32 application demonstrates four different methods for reading data from the ADC (Analog-to-Digital Converter).
Which method is used is selected at compile-time with a macro named `ADC_MODE_SELECTION`.
All modes read four ADC channels and save the results into an array (ADC_VAL).

**Mode 1 (Polling):**
The main loop (`while(1)`) is completely blocked.
The loop starts each of the four channels one by one.
It actively waits for each conversion to finish and writes the result to the array.

**Mode 2 (DMA Normal):**
The main loop starts a 4-channel DMA transfer.
The main loop is free while the DMA fetches data in the background.
When the transfer is complete, an interrupt function sets the `isADCFinished` flag.
The main loop sees this flag, processes the data, and must manually restart the DMA for the next reading.

**Mode 3 (DMA Circular):**
The DMA is started only once.
It reads the four channels and automatically restarts when finished.
The main loop does not interfere with this process and can read fresh data from the array at any time.
An interrupt function just toggles an LED on each completion.

**Mode 4 (Timer Triggered):**
A hardware timer (TIM1) is configured to trigger the ADC.
The timer periodically starts the ADC conversion without any main loop or CPU intervention.
The DMA automatically transfers this triggered data to the array in the background.
The main loop remains completely free.