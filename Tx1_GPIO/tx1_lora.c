// exampleApp.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "jetsonGPIO.h"
#include "tx1_lora.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define REG_VERSION              0x42

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

int fd = -1;
int _frequency=0;

jetsonTX1GPIONumber gpio_cs = gpio187 ;
jetsonTX1GPIONumber gpio_lora = gpio219 ;     // Output

void SPI_Init(void)
{
	int ret;
	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
}

static uint8_t spi_transfer(int byte_val)
{
	int ret;
	uint8_t tx[] = {
		byte_val
	};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");
	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("%.2X ", rx[ret]);
	}
	puts("");
	return rx[0];
}


uint8_t singleTransfer(uint8_t address, uint8_t value)
{
  uint8_t response=0;

  //digitalWrite(_ss, LOW);
  gpioSetValue(gpio_cs, off);  //CHIP_SELECT();
  //SPI.beginTransaction(_spiSettings);
  response = spi_transfer(address);
  // printf("address response %x\n",response);
  response = spi_transfer(value);
  // printf("value response %x\n",response);
  gpioSetValue(gpio_cs, on);   //CHIP_DESELECT();

  //digitalWrite(_ss, HIGH);

  return response;
}

uint8_t readRegister(uint8_t address)
{
  return singleTransfer(address & 0x7f, 0x00);
}

void writeRegister(uint8_t address, uint8_t value)
{
  singleTransfer(address | 0x80, value);
}

void idle()
{
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

void lora_sleep()
{
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void setTxPower(int level)
{
	if (level < 2)
	{
		level = 2;
	}
	else if (level > 17)
	{
		level = 17;
	}

	writeRegister(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

void setFrequency(long frequency)
{
	_frequency = frequency;
	printf("frequency is %ld,%d\n",frequency,_frequency);

	uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

	writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
	writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
	writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

int LoRabegin(long frequency)
{
  // setup pins
  int i;
  uint8_t version =0;

  // Initialize GPIO for resetting LORA 
  gpioExport(gpio_lora) ;
  gpioSetDirection(gpio_lora,outputPin) ;
  
  // Initialize GPIO for chip select

  gpioExport(gpio_cs) ;
  gpioSetDirection(gpio_cs,outputPin) ;

  // perform reset

  gpioSetValue(gpio_lora, off);
  for(i=0;i<100000;i++);
  gpioSetValue(gpio_lora, on);
  for(i=0;i<10000000;i++);

  // set SS high
  gpioSetValue(gpio_cs, on); //CHIP_DESELECT();
 
  SPI_Init();

  // start SPI
  //SPI.begin();

  // check version
  version = readRegister(REG_VERSION);
  printf("Version is %x\n",version);
  if (version != 0x12) {
    return 0;
  }

  // put in sleep mode
  lora_sleep();
  // set frequency
  setFrequency(frequency);

  close(fd);
  return 1;
}

int main(int argc, char *argv[])
{
    LoRabegin(915000000);

    gpioUnexport(gpio_cs) ;
    gpioUnexport(gpio_lora) ;
    return 0;
}



