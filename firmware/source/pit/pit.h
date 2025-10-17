#ifndef PIT_PIT_H_
#define PIT_PIT_H_

/** PIT’in her tick süresi (µs cinsinden). */
#define PIT_TICK_US    		10u

/** 1 saniyedeki toplam tick sayısı (örnek: 10 µs → 100000 tick). */
#define PIT_TICKS_PER_S		(1000000u / PIT_TICK_US)

/** 1 milisaniyedeki toplam tick sayısı (örnek: 10 µs → 100 tick). */
#define PIT_TICKS_PER_MS  	(1000u / PIT_TICK_US)

/** PIT donanımını ayarlar (çözünürlük ve kesme). */
void pit_init(void);

/** PIT sayacını başlatır (kesmeler aktif). */
void pit_start(void);

/** PIT sayacını durdurur (kesmeler pasif). */
void pit_stop(void);

#endif /* PIT_PIT_H_ */
