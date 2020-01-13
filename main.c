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

void stringTx(const char *str)
{
	uint8_t i = 0;
	while(str[i] != 0x00)
	{
		uartTX(str[i]);
		i++;
	}
}

void byteBinTx(uint8_t b)
{
	for(uint8_t n = 7; n < 8; n--)
	{
		uartTX(0x30 + ((b >> n) & 0x01));
	}
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

uint8_t spi_tx(uint8_t b)
{
	SPI_DR = b;
	while( !(SPI_SR & SPI_SR_TXE));
	while((SPI_SR & (SPI_SR_BSY)));
	return SPI_DR;
}

uint8_t spi_rx()
{
/*	spi_tx(0xff);
	while(!(SPI_SR & SPI_SR_RxNE)) {}
	return SPI_DR;*/

	return spi_tx(0xff);
}

void dumpFlash()
{
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

uint8_t p = 0;
uint8_t printStatus()
{

	spi_begin();
	spi_tx(0x05); //RDSR
//	while(!(SPI_SR & SPI_SR_RxNE)) {}
	//data[4] = SPI_DR;
	uint8_t flaStatus = spi_rx();
	spi_end();
	
	stringTx("Status: ");
	byteBinTx(p++);
	uartTX('\t');
	byteBinTx(flaStatus);
	stringTx("\r\n");
	return flaStatus;
}

int main(void)
{
	/* Set clock to full speed (16 Mhz) */
	CLK_CKDIVR = 0;
	CLK_PCKENR1 = 0xFF;
	setupOutGPIO();
	setupSPI();
	setupUART();

	uint16_t n;

	for(n = 0; n < 16000; n++)
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

	dumpFlash();
	printStatus();

	spi_begin();
	spi_tx(0x06); //WREN
	spi_end();

	nop(); nop(); nop();
	stringTx("WREN\r\n");

	/*if(printStatus() == 0x02)
	{
		stringTx("CHIP ERASE ...");
		spi_begin();
		spi_tx(0xC7); //Chip Erase
		spi_end();
		stringTx(" SENT\r\n");
	}*/

	while(1) {

		printStatus();
		for(n = 0; n < 60000; n++)
		{
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
			nop();
		}
	}
}

