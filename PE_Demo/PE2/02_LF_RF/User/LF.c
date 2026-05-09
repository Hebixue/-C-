#include "LF.h"
#include "Fortissimo.h"
#include "stdio.h"

#define INITIALVDS0 500 // [0..1023]
#define INITIALVDS1 250 // [0..1023]
uint8_t VDSI0H = (INITIALVDS0 >> 2);
uint8_t VDSI1H = (INITIALVDS1 >> 2);
uint8_t VDSIL = ((INITIALVDS0 & 0x03) | (INITIALVDS1 & 0x03) << 2);
uint8_t AntennaCurrent = CURRENT_500mA;
uint32_t rollingCnt = 1;
uint16_t immoFails;
uint16_t immoRuns;
uint8_t active_antenna = 3;
SPI_BuffersTypeDef SPI_BUFFERS;

void SendLFWakeUp(void);
void Immobilizer(void);
uint8_t GetAscii(uint8_t data);

void SendLFWakeUp(void)
{
  uint8_t temp;
  uint8_t Peps_Telegram[22];
  uint8_t *tmpPtr = &Peps_Telegram[0];

  // build PEPS telegram
  *tmpPtr++ = 0xAA;
  *tmpPtr++ = 0xAA;
  *tmpPtr++ = 0xAA;
  *tmpPtr++ = 0xAA;
  *tmpPtr++ = 0xAA;
  *tmpPtr++ = 0xA9;

  lf_ata5293_SWRI(PWVIC0, 34); // （时延到输出稳定后再调节电流） VI Configuration on path 0. Sets the delay from the beginning of a wave output till the VI-Regulator starts working.
  // lf_ata5293_SWRI(PWBC, 0x72);       // （使能path0的电流监测与path0、path1的升压电路,默认开关频率为250khz） 0x72 Selects current regulation on path 0, Enables boost voltage regulator for path 0+1.
  // 开关频率设置在125khz，满足硬件电路
  lf_ata5293_SWRI(PWBC, 0x70);
  lf_ata5293_SWRI(PWBST, 0x00);
  lf_ata5293_SWRI(PWACFG, 0x50);
  // lf_ata5293_SWRI(PWVDSIL, VDSIL);   // set the initial target voltage for the booster regulator
  // lf_ata5293_SWRI(PWVDSI0H, VDSI0H); // set the initial target voltage for the booster regulator (path0)（10.2v）
  lf_ata5293_SWRI(PWVDSIL, 0x11); // set the initial target voltage for the booster regulator
  lf_ata5293_SWRI(PWVDSI0H, 0xB6);
  // lf_ata5293_SWRI(PWVDSI1H, VDSI1H); // set the initial target voltage for the for path 1 (Jammer)（20.5v）
  DriverOutput(DRIVER_A30, POSITIVE_POLARITY_WAVEFORM, SINE_WAVE, PATH0);   // Ant1 SE
  DriverOutput(DRIVER_A02, POSITIVE_POLARITY_WAVEFORM, SINE_WAVE, PATH0);   // Ant2 SE
  DriverOutput(DRIVER_A31, POSITIVE_POLARITY_WAVEFORM, SINE_WAVE, PATH0);   // Ant3 SE
  DriverOutput(DRIVER_A10, POSITIVE_POLARITY_WAVEFORM, SINE_WAVE, PATH0);   // Ant4 SE

  // Configure waveform and data
  lf_ata5293_SWRI(TXMOD, (Cordic | f_125));    // 生成频率可调的125kHz载波，曼切斯特编码下降沿为0，上升沿为1
  lf_ata5293_SWRI(TXENC, (TX_NRZ | TX_M4kBd)); // setting for Manchester 4 kBd - need to be changed for Fortissimo

  // Enter TX Mode
  // 调试前面完成前先关闭TX模式
  lf_ata5293_GTX();
  switch (active_antenna)
  {
  case 1: // Ant1 Single Ended A30
    tmpPtr = Manchester2spi(0x14, tmpPtr);
    tmpPtr = Manchester2spi(0x13, tmpPtr);
    tmpPtr = Manchester2spi(0x12, tmpPtr);
    tmpPtr = Manchester2spi(0x11, tmpPtr);
    tmpPtr = Manchester2spi(0xB6, tmpPtr);
    tmpPtr = Manchester2spi(0xFE, tmpPtr);
    tmpPtr = Manchester2spi(0xCA, tmpPtr);
    tmpPtr = Manchester2spi(0xFE, tmpPtr);

    lf_SelectCoilCurrent(AntennaCurrent, DRIVER_A30, PATH0);
    lf_ata5293_WRI(DRTMH, 0x00);
    lf_ata5293_WRI(DRTML, DRIVER_A30);
    lf_ata5293_SDR((DRIVER_A30), TRUE);
    break;
  case 2: // Ant2 SE A02
    tmpPtr = Manchester2spi(0x14, tmpPtr);
    tmpPtr = Manchester2spi(0x13, tmpPtr);
    tmpPtr = Manchester2spi(0x12, tmpPtr);
    tmpPtr = Manchester2spi(0x11, tmpPtr);
    tmpPtr = Manchester2spi(0xB7, tmpPtr);
    tmpPtr = Manchester2spi(0xFE, tmpPtr);
    tmpPtr = Manchester2spi(0xCA, tmpPtr);
    tmpPtr = Manchester2spi(0xFE, tmpPtr);

    lf_SelectCoilCurrent(AntennaCurrent, DRIVER_A02, PATH0);
    lf_ata5293_WRI(DRTMH, 0x00);
    lf_ata5293_WRI(DRTML, DRIVER_A02);
    lf_ata5293_SDR((DRIVER_A02), TRUE);
    break;
  case 3: // Ant3 SE A31
    tmpPtr = Manchester2spi(0x14, tmpPtr);
    tmpPtr = Manchester2spi(0x13, tmpPtr);
    tmpPtr = Manchester2spi(0x12, tmpPtr);
    tmpPtr = Manchester2spi(0x11, tmpPtr);
    tmpPtr = Manchester2spi(0xB8, tmpPtr);
    tmpPtr = Manchester2spi(0xFE, tmpPtr);
    tmpPtr = Manchester2spi(0xCA, tmpPtr);
    tmpPtr = Manchester2spi(0xFE, tmpPtr);

    lf_SelectCoilCurrent(AntennaCurrent, DRIVER_A31, PATH0);
    lf_ata5293_WRI(DRTMH, (DRIVER_A31 >> 8));
    lf_ata5293_WRI(DRTML, 0x00);
    lf_ata5293_SDR(DRIVER_A31, TRUE);
    break;
  case 4: // Ant4 SE A10
    tmpPtr = Manchester2spi(0x14, tmpPtr);
    tmpPtr = Manchester2spi(0x13, tmpPtr);
    tmpPtr = Manchester2spi(0x12, tmpPtr);
    tmpPtr = Manchester2spi(0x11, tmpPtr);
    tmpPtr = Manchester2spi(0xB9, tmpPtr);
    tmpPtr = Manchester2spi(0xFE, tmpPtr);
    tmpPtr = Manchester2spi(0xCA, tmpPtr);
    tmpPtr = Manchester2spi(0xFE, tmpPtr);

    lf_SelectCoilCurrent(AntennaCurrent, DRIVER_A10, PATH0);
    // lf_ata5293_WRI(PWCMS0, 0x53);
    lf_ata5293_WRI(DRTMH, 0x00);
    lf_ata5293_WRI(DRTML, DRIVER_A10);
    lf_ata5293_SDR((DRIVER_A10), TRUE);
    break;
  default:
    break;
  }

		// Start Telegram
		lf_ata5293_SD(22 * 8, Peps_Telegram); // Send data
		lf_ata5293_PTX(0x0C);
		lf_ata5293_SC(0x0F);
		lf_ata5293_PTX(1);
		// 进入诊断模式
		// lf_ata5293_GDA();
		uint8_t i;
		for (i = 160; i > 0; i--)
		{
			i--;
		}
		lf_ata5293_GID(); // Go to Idle mode
		checkStatus();
		// Read VDSIR to update VDSI for next run
		lf_ata5293_SRD(PWVDSIR0H, &VDSI0H);
		lf_ata5293_SRD(PWVDSIR1H, &VDSI1H);
		lf_ata5293_SRD(PWVDSIRL, &VDSIL);
		// Read latest VDS value
		lf_ata5293_SRD(PWVDSH, &temp);
		// Read driver temperature
		lf_ata5293_SRD(PWBAM, &temp);
		lf_ata5293_SRD(PWAPH0, &temp);
		lf_ata5293_SRD(PWASF0, &temp);
}
