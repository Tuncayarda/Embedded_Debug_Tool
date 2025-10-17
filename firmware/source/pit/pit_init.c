#include "fsl_pit.h"
#include "pit.h"

/**
 * @brief Global tick sayacı.
 *
 * - Her PIT kesmesinde 1 artırılır.
 * - Sistemde zaman ölçümü için ortak sayaçtır.
 */
volatile uint32_t g_tick = 0;

/**
 * @brief PIT kanal 0 kesme fonksiyonu.
 *
 * - Kesme bayrağını temizler (yoksa sürekli kesmeye girer).
 * - g_tick değişkenini 1 artırır → her PIT_TICK_US µs’de bir sayım.
 * - __DSB() → memory barrier. Bellek erişimlerinin ISR çıkmadan
 *   önce tamamlanmasını garanti eder.
 */
void PIT0_IRQHandler(void)
{
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag); // Kesme bayrağını temizle
    g_tick += 1;                                           // Global tick sayacını artır
    __DSB();                                               // Veri senkronizasyon bariyeri
}

/**
 * @brief PIT donanımını başlatır.
 *
 * - PIT default ayarlarla init edilir.
 * - Bus clock frekansına göre periyot hesaplanır (cycles).
 * - PIT periyodu ayarlanır, kesmeler açılır ve NVIC’e enable edilir.
 * - PIT sayacı hemen başlatılır.
 *
 * Not: Bu fonksiyon pit_start()/pit_stop() kullanılmadan önce mutlaka çağrılmalıdır.
 */
void pit_init(void)
{
    pit_config_t cfg;
    PIT_GetDefaultConfig(&cfg);     // Varsayılan PIT ayarlarını al
    PIT_Init(PIT, &cfg);

    uint32_t clk = CLOCK_GetFreq(kCLOCK_BusClk);
    // Bus clock frekansı (ör: 48 MHz, 72 MHz vb.)

    // cycles = (clk frekansı * tick süresi (µs)) / 1.000.000
    // Yuvarlama için +500000 eklenmiş.
    // 35.995648 MHz bus hızında +500000 eklenmediğinde cycle belirlenirken oluşan hata düşüyor.
    uint32_t cycles = (uint32_t)(((uint64_t)clk * (uint64_t)PIT_TICK_US + 500000u) / 1000000u);

    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, cycles);              // Hesaplanan cycle değerini yükle
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable); // Kesme aç
    NVIC_SetPriority(PIT0_IRQn, 0);                            // Öncelik ayarı
    EnableIRQ(PIT0_IRQn);                                      // NVIC enable
    PIT_StartTimer(PIT, kPIT_Chnl_0);                          // Timer’ı başlat
}

void pit_start(void)
{
    PIT_StartTimer(PIT, kPIT_Chnl_0);
}

void pit_stop(void)
{
    PIT_StopTimer(PIT, kPIT_Chnl_0);
}
