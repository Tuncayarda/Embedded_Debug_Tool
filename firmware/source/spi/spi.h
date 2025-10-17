#ifndef SPI_SPI_H_
#define SPI_SPI_H_

#include "fsl_dspi.h"

//Kullanılan spi arayüzü
#define SPIx SPI0

void spi_init(t_cspi_fields *C);


size_t cspi_tx_push(t_cspi_fields *C, const uint8_t *data, size_t len);

#endif /* SPI_SPI_H_ */
