#include <stdint.h>
#include "stm8.h"

static uint8_t data[5];// = {0x13, 0x37, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setupOutGPIO()
{
	//Onboard LED
	PB_DDR = 0x20;
	PB_CR1 = 0x20;
	PB_ODR = 1 << 5;
}

void setupSPI()
{
	SPI_CR2 = 0x00;
//	SPI_CR1 = 0x74; //SPI Enable, BR=f/128, Master
//	SPI_CR1 = 0x64; //SPI Enable, BR=f/32, Master
	SPI_CR1 = 0x54; //SPI Enable, BR=f/8, Master

	PA_DDR |= (1 << 3);
	PA_CR1 |= (1 << 3);
	PA_ODR |= (1 << 3);
}

void setupUART()
{
	UART1_CR2 = UART_CR2_TEN;
	UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2);
//	UART1_BRR2 = 0x03;
//	UART1_BRR1 = 0x68;
	UART1_BRR1 = 0x02;
	UART1_BRR2 = 0x00;
}

void uartTX(char c)
{
	while (!(UART1_SR & UART_SR_TXE));
	UART1_DR = c;
}

void spi_begin()
{
	PA_ODR &= ~(1 << 3);
}

void spi_end()
{
	while((SPI_SR & (SPI_SR_BSY)));
	PA_ODR |= (1 << 3);
}

void spi_tx(uint8_t b)
{
	SPI_DR = b;
	while( !(SPI_SR & SPI_SR_TXE));
}

uint8_t spi_rx()
{
	spi_tx(0xff);
	while(!(SPI_SR & SPI_SR_RxNE));
	return SPI_DR;
}

int main(void)
{
	/* Set clock to full speed (16 Mhz) */
	CLK_CKDIVR = 0;
	CLK_PCKENR1 = 0xFF;
	setupOutGPIO();
	setupSPI();
	setupUART();

	for(uint16_t n = 0; n < 16000; n++)
	{
		nop();
		nop();
		nop();
		nop();
		nop();
		nop();
		nop();
		nop();
	}

	while(1) {
		data[3]++;
		spi_begin();
		/*spi_tx(0x9F);
		while(!(SPI_SR & SPI_SR_RxNE));
		data[4] = SPI_DR;
		//data[5] = spi_rx();
		//data[6] = spi_rx();
		//data[7] = spi_rx();*/

		spi_tx(0x03);
		spi_tx(0);
		spi_tx(0);
		spi_tx(0);
		while(!(SPI_SR & SPI_SR_RxNE));
		data[4] = SPI_DR;

		for(uint16_t n = 0; n < 1024; n++)
		for(uint16_t o = 0; o < 512; o++)
		{
			uint8_t rb = spi_rx();
//			data[n] = rb;
			uartTX(rb);
			uartTX(~rb);
		}
		spi_end();

		uartTX(0xff);
		uartTX(0xff);		

		for(uint16_t n = 0; n < 16000; n++)
		{
			nop();
			n = 0;
		}
	}
}

