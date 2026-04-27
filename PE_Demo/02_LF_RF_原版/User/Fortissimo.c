/* ===========================================  Includes  =========================================== */
#include "gpio.h"
#include "spi_master_drv.h"
#include "fortissimo.h"
// #include "main.h"
#include "LF.h"
#include "spi.h"
#include <string.h>
#include <string.h>

/*PROTOTYPES*/
void copyData(uint8_t *CMD_BUFFER, uint8_t *CMD, uint8_t length);

void lf_ata5293_GDA(void);
void lf_ata5293_GID(void);
void lf_ata5293_GSLP(void);
void lf_ata5293_GTX(void);
void lf_ata5293_GRH(void);
void lf_ata5293_GFDX(void);
void lf_ata5293_SBI(uint8_t bitToSet, uint8_t address);
void lf_ata5293_CBI(uint8_t bitToClear, uint8_t address);
void lf_ata5293_LSLI(uint8_t shiftBy, uint8_t address);
void lf_ata5293_LSRI(uint8_t shiftBy, uint8_t address);
void lf_ata5293_AND(uint8_t destReg, uint8_t valueReg);
void lf_ata5293_ANDI(uint8_t destReg, uint8_t value);
void lf_ata5293_OR(uint8_t destReg, uint8_t valueReg);
void lf_ata5293_ORI(uint8_t destReg, uint8_t value);
void lf_ata5293_ADD(uint8_t destReg, uint8_t valueReg);
void lf_ata5293_ADDI(uint8_t destReg, uint8_t value);
void lf_ata5293_SUB(uint8_t destReg, uint8_t valueReg);
void lf_ata5293_SUBI(uint8_t destReg, uint8_t value);
void lf_ata5293_CMP(uint8_t Rd, uint8_t address);
void lf_ata5293_NOT(uint8_t address);
void lf_ata5293_WRI(uint8_t address, uint8_t data);
void lf_ata5293_WRB(uint8_t address, uint8_t length, uint8_t *data);
void lf_ata5293_WRAR(uint8_t address, uint8_t length, uint8_t *data);
void lf_ata5293_SGP(uint8_t pin, uint8_t level);
void lf_ata5293_LD(uint8_t reg, uint8_t address);
void lf_ata5293_ST(uint8_t reg, uint8_t address);
void lf_ata5293_PUSH(uint8_t address);
void lf_ata5293_POP(uint8_t address);
void lf_ata5293_SKIF(uint8_t flags);
void lf_ata5293_SKINF(uint8_t flags);
void lf_ata5293_CALLA(uint16_t address);
void lf_ata5293_JMP(uint16_t address);
void lf_ata5293_CALL(uint8_t label);
void lf_ata5293_WAIT(uint8_t prescaler, uint8_t counter);
void lf_ata5293_JMPL(uint8_t label);
void lf_ata5293_DBNZ(uint8_t label);
void lf_ata5293_CONFC(uint8_t reg, uint8_t mask);
void lf_ata5293_CLRC(void);
void lf_ata5293_WAITC(uint8_t value, uint8_t flag);
void lf_ata5293_RET(void);
void lf_ata5293_JMPP(void);
void lf_ata5293_TOUT(uint8_t t);
void lf_ata5293_PTX(uint8_t pauseDuration);
void lf_ata5293_FDXCC(uint8_t enable);
void lf_ata5293_SC(uint8_t carrierDuration);
void lf_ata5293_SDR(uint16_t drivers, uint8_t duration);
void lf_ata5293_SAP(void);
void lf_ata5293_SCC(uint8_t coilCurrent);
void lf_ata5293_SD(uint16_t bitsToSend, uint8_t *data);
void lf_ata5293_IRO(uint8_t enable);
void lf_ata5293_CLR(uint8_t bufferSelection);
void lf_ata5293_SSC(uint8_t spiSetting);
void lf_ata5293_GSI(uint8_t *returnedData);
void lf_ata5293_RFS(void);
void lf_ata5293_SWRI(uint8_t address, uint8_t data);
void lf_ata5293_SWRB(uint8_t address, uint8_t, uint8_t *data);
void lf_ata5293_SORI(uint8_t address, uint8_t value);
void lf_ata5293_SANDI(uint8_t address, uint8_t value);
void lf_ata5293_SRD(uint8_t address, uint8_t *returnedData);
void lf_ata5293_SLBL(uint8_t labelSource, uint8_t labelRegister);
void lf_ata5293_SRB(uint8_t address, uint8_t length, uint8_t *returnedData);
void lf_ata5293_HALT(void);
void lf_ata5293_RUN(void);
void lf_ata5293_CRX(void);
void lf_ata5293_CDH(void);
void lf_ata5293_NOP(void);
uint8_t checkStatus(void);
void DriverOutput(uint16_t driver, uint8_t driverConfiguration, uint8_t waveFormType, uint8_t path);
void lf_SelectCoilCurrent(uint8_t coilcurrent, uint16_t driver, uint8_t path);
uint8_t get_current_setting(uint16_t current);

/*CONSTANTS*/
const uint8_t GDA[1] = {0x01};                         // returns 0
const uint8_t GID[1] = {0x02};                         // returns 0
const uint8_t GSLP[1] = {0x03};                        // returns 0
const uint8_t GTX[1] = {0x04};                         // returns 0
const uint8_t GRH[1] = {0x05};                         // returns 0
const uint8_t GFDX[1] = {0x06};                        // returns 0
const uint8_t SBI[2] = {0x08, 0x00};                   // returns 0
const uint8_t CBI[2] = {0x10, 0x00};                   // returns 0
const uint8_t LSLI[2] = {0x18, 0x00};                  // returns 0
const uint8_t LSRI[2] = {0x20, 0x00};                  // returns 0
const uint8_t AND[3] = {0x28, 0x00, 0x00};             // returns 0
const uint8_t ANDI[3] = {0x29, 0x00, 0x00};            // returns 0
const uint8_t OR[3] = {0x2A, 0x00, 0x00};              // returns 0
const uint8_t ORI[3] = {0x2B, 0x00, 0x00};             // returns 0
const uint8_t ADD[3] = {0x2C, 0x00, 0x00};             // returns 0
const uint8_t ADDI[3] = {0x2D, 0x00, 0x00};            // returns 0
const uint8_t SUB[3] = {0x2E, 0x00, 0x00};             // returns 0
const uint8_t SUBI[3] = {0x2F, 0x00, 0x00};            // returns 0
const uint8_t CP[3] = {0x30, 0x00, 0x00};              // returns 0
const uint8_t NOT[2] = {0x31, 0x00};                   // returns 0
const uint8_t WRI[3] = {0x32, 0x00, 0x00};             // returns 0
const uint8_t WRB[3] = {0x33, 0x00, 0x00};             // returns 0
const uint8_t WRAR[3] = {0x34, 0x00, 0x00};            // returns
const uint8_t SGP[1] = {0x38};                         // returns 0
const uint8_t LD[2] = {0x40, 0x00};                    // returns 0
const uint8_t ST[2] = {0x48, 0x00};                    // returns 0
const uint8_t PUSH[2] = {0x35, 0x00};                  // returns 0
const uint8_t POP[2] = {0x36, 0x00};                   // returns 0
const uint8_t SKIF[1] = {0x50};                        // returns 0
const uint8_t SKINF[1] = {0x58};                       // returns 0
const uint8_t CALLA[2] = {0x60, 0x00};                 // returns 0
const uint8_t JMP[2] = {0x68, 0x00};                   // returns 0
const uint8_t CALL[1] = {0x70};                        // returns 0
const uint8_t WAIT[2] = {0x74, 0x00};                  // returns 0
const uint8_t JMPL[1] = {0x78};                        // returns 0
const uint8_t DBNZ[1] = {0x7C};                        // returns 0
const uint8_t CONFC[2] = {0x80, 0x00};                 // returns 0
const uint8_t CLRC[1] = {0x87};                        // returns 0
const uint8_t WAITC[2] = {0x88, 0x00};                 // returns 0
const uint8_t RET[1] = {0x89};                         // returns 0
const uint8_t JMPP[1] = {0x8A};                        // returns 0
const uint8_t TOUT[2] = {0x8B, 0x00};                  // returns 0
const uint8_t PTX[1] = {0x90};                         // returns 0
const uint8_t SJ[3] = {0xA0, 0x00, 0x00};              // returns *obsolete
const uint8_t FDXCC[1] = {0xA0};                       // returns 0
const uint8_t SC[1] = {0xB0};                          // returns 0
const uint8_t SDR[3] = {0xC0, 0x00, 0x00};             // returns 0
const uint8_t SAP[1] = {0xC1};                         // returns 0
const uint8_t SCC[2] = {0xC2, 0x00};                   // returns 0
const uint8_t SD[3] = {0xC3, 0x00, 0x00};              // returns 0
const uint8_t RX[1] = {0xC4};                          // returns 0
const uint8_t CLR[1] = {0xE0};                         // returns 0
const uint8_t SSC[1] = {0xE0};                         // returns 0
const uint8_t GSI[3] = {0xE8, 0x00, 0x00};             // returns 2 bytes ****************************** 3-2 = 1
const uint8_t RFS[1] = {0xEA};                         // returns
const uint8_t SWRI[3] = {0xEB, 0x00, 0x00};            // returns 0
const uint8_t SWRB[4] = {0xEC, 0x00, 0x00, 0x00};      // returns 0
const uint8_t SORI[3] = {0xED, 0x00, 0x00};            // returns 0
const uint8_t SANDI[3] = {0xEE, 0x00, 0x00};           // returns 0
const uint8_t SRD[4] = {0xEF, 0x00, 0x00, 0x00};       // returns 1 byte ********************** 4-1 = 3
const uint8_t SLBL[1] = {0xF0};                        // returns 0
const uint8_t SRB[5] = {0xF8, 0x00, 0x00, 0x00, 0x00}; // returns *************************
const uint8_t HALT[1] = {0xF9};                        // returns 0
const uint8_t NOP[1] = {0xFD};                         // returns 0
const uint8_t RUN[1] = {0xFA};                         // returns 0
const uint8_t CRX[1] = {0xFB};                         // returns 0
const uint8_t CDH[1] = {0xFC};                         // returns 0
extern SPI_BuffersTypeDef SPI_BUFFERS;
/***********************************************************************************************************************
 * Function Name: lf_Mode_GoTo_IdleMode()
 * Description : Command device into Idle Mode
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/
void lf_ata5293_GDA()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)GDA, GDA_LENGTH);
    SPI_BUFFERS.Length = GDA_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_Mode_GoTo_IdleMode()
 * Description : Command device into Idle Mode
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/
void lf_ata5293_GID()
{
    memset(SPI_BUFFERS.Rx, 0, GID_LENGTH); // 把 RX 缓冲区前 GID_LENGTH 字节置 0，覆盖上次残留数据
    copyData(SPI_BUFFERS.Tx, (uint8_t *)GID, GID_LENGTH);
    SPI_BUFFERS.Length = GID_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE;
}
/***********************************************************************************************************************
 * Function Name: lf_Mode_GoTo_SleepMode()
 * Description : Command device into Sleep Mode
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_GSLP()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)GSLP, GSLP_LENGTH);
    SPI_BUFFERS.Length = GSLP_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE;
}

/***********************************************************************************************************************
 * Function Name: lf_Mode_GoTo_TxMode()
 * Description : Command device into Transmit Mode
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_GTX()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)GTX, GTX_LENGTH);
    SPI_BUFFERS.Length = GTX_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_Mode_GoTo_RxHDXMode()
 * Description : Command device into Receive Mode Half Duplex
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_GRH()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)GRH, GRH_LENGTH);
    SPI_BUFFERS.Length = GRH_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_Mode_GoTo_RxFDXMode()
 * Description : Command device into Receive Mode Full Duplex
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_GFDX()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)GFDX, GFDX_LENGTH);
    SPI_BUFFERS.Length = GFDX_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}
/***********************************************************************************************************************
   _             _           _       __           _ _   _                    _   _           _
  | |           (_)         | |     / /          (_) | | |                  | | (_)         | |
  | | ___   __ _ _  ___ __ _| |    / /  __ _ _ __ _| |_| |__  _ __ ___   ___| |_ _  ___ __ _| |
  | |/ _ \ / _` | |/ __/ _` | |   / /  / _` | '__| | __| '_ \| '_ ` _ \ / _ \ __| |/ __/ _` | |
  | | (_) | (_| | | (_| (_| | |  / /  | (_| | |  | | |_| | | | | | | | |  __/ |_| | (_| (_| | |
  |_|\___/ \__, |_|\___\__,_|_| /_/    \__,_|_|  |_|\__|_| |_|_| |_| |_|\___|\__|_|\___\__,_|_|
            __/ |
           |___/
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SetBitImmediately
 * Description : Set specified bit in register
 * Arguments : bitToSet(0-7): Bit to be Set to 1, address(0x00-0xFF): Address of register
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SBI(uint8_t bitToSet, uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SBI, SBI_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | bitToSet;
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = SBI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_ClearBitImmediately
 * Description : Clear specified bit in register
 * Arguments : bitToClear(0-7): Bit to be cleared to 1, address(0x00-0xFF): Address of register
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CBI(uint8_t bitToClear, uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CBI, CBI_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | bitToClear;
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = CBI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_LeftShiftBits
 * Description : Left shifts bits in a specified register
 * Arguments : shiftBy(0-7): How many bits to left shift by, address(0x00-0xFF): Address of register
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_LSLI(uint8_t shiftBy, uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)LSLI, LSLI_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | shiftBy;
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = LSLI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_RightShiftBits
 * Description : Right shifts bits in a specified register
 * Arguments : shiftBy(0-7): How many bits to right shift by, address(0x00-0xFF): Address of register
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_LSRI(uint8_t shiftBy, uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)LSRI, LSRI_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | shiftBy;
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = LSRI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_AND
 * Description : Logical AND two registers together and store the value in one of them
 * Arguments : destReg: Register where the value of it is ANDed with another register and saved back to it
 *             valueReg: Register whose value is ANDed with desReg and saved to destReg
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_AND(uint8_t destReg, uint8_t valueReg)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)AND, AND_LENGTH);
    SPI_BUFFERS.Tx[1] = destReg;
    SPI_BUFFERS.Tx[2] = valueReg;
    SPI_BUFFERS.Length = AND_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_ANDI
 * Description : Logical AND a register with a value and store the value in register in one of them
 * Arguments : destReg: Register where the value of it is ANDed with a specified value and saved back to it
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_ANDI(uint8_t destReg, uint8_t value)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)ANDI, ANDI_LENGTH);
    SPI_BUFFERS.Tx[1] = destReg;
    SPI_BUFFERS.Tx[2] = value;
    SPI_BUFFERS.Length = ANDI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_OR
 * Description : Logical OR two registers together and store the value in one of them
 * Arguments : destReg: Register where the value of it is ORDed with another register and saved back to it
 *             valueReg: Register whose value is ORed with desReg and saved to destReg
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_OR(uint8_t destReg, uint8_t valueReg)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)OR, OR_LENGTH);
    SPI_BUFFERS.Tx[1] = destReg;
    SPI_BUFFERS.Tx[2] = valueReg;
    SPI_BUFFERS.Length = OR_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_ORI
 * Description : Logical OR a register with a value and store the value in register in one of them
 * Arguments : destReg: Register where the value of it is ORed with a specified value and saved back to it
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_ORI(uint8_t destReg, uint8_t value)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)ORI, ORI_LENGTH);
    SPI_BUFFERS.Tx[1] = destReg;
    SPI_BUFFERS.Tx[2] = value;
    SPI_BUFFERS.Length = ORI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_ADD
 * Description : Logical ADD two registers together and store the value in one of them
 * Arguments : destReg: Register where the value of it is ADDed with another register and saved back to it
 *             valueReg: Register whose value is ADDed with desReg and saved to destReg
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_ADD(uint8_t destReg, uint8_t valueReg)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)ADD, ADD_LENGTH);
    SPI_BUFFERS.Tx[1] = destReg;
    SPI_BUFFERS.Tx[2] = valueReg;
    SPI_BUFFERS.Length = ADD_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_ADDI
 * Description : Logical ADD a register with a value and store the value in register in one of them
 * Arguments : destReg: Register where the value of it is ADDed with a specified value and saved back to it
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_ADDI(uint8_t destReg, uint8_t value)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)ADDI, ADDI_LENGTH);
    SPI_BUFFERS.Tx[1] = destReg;
    SPI_BUFFERS.Tx[2] = value;
    SPI_BUFFERS.Length = ADDI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SUB
 * Description : Logical SUBtract two registers together and store the value in one of them
 * Arguments : destReg: Register where the value of it is SUBtracted with another register and saved back to it
 *             valueReg: Register whose value is SUBtracted with desReg and saved to destReg
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SUB(uint8_t destReg, uint8_t valueReg)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SUB, SUB_LENGTH);
    SPI_BUFFERS.Tx[1] = destReg;
    SPI_BUFFERS.Tx[2] = valueReg;
    SPI_BUFFERS.Length = SUB_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SUBI
 * Description : Logical SUBtract a register with a value and store the value in register in one of them
 * Arguments : destReg: Register where the value of it is SUBtracted with a specified value and saved back to it
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SUBI(uint8_t destReg, uint8_t value)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SUBI, SUBI_LENGTH);
    SPI_BUFFERS.Tx[1] = destReg;
    SPI_BUFFERS.Tx[2] = value;
    SPI_BUFFERS.Length = SUBI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_CMP
 * Description : Logical Compare of two registers
 * Arguments : Rd: First register to compare, Address: Second register to compare to the first
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CMP(uint8_t Rd, uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CP, CP_LENGTH);
    SPI_BUFFERS.Tx[1] = Rd;
    SPI_BUFFERS.Tx[2] = address;
    SPI_BUFFERS.Length = CP_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_NOT
 * Description : Inverse register
 * Arguments : Rd: First register to compare, Address: Second register to compare to the first
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_NOT(uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)NOT, NOT_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = NOT_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
   _                 _       __      _
  | |               | |     / /     | |
  | | ___   __ _  __| |    / /   ___| |_ ___  _ __ ___
  | |/ _ \ / _` |/ _` |   / /   / __| __/ _ \| '__/ _ \
  | | (_) | (_| | (_| |  / /    \__ \ || (_) | | |  __/
  |_|\___/ \__,_|\__,_| /_/     |___/\__\___/|_|  \___|
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lf_ata5293_WRI()
 * Description : Write Immediate
 * Arguments : address: Address to write, value: value to write to address
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_WRI(uint8_t address, uint8_t data)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)WRI, WRI_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Tx[2] = data;
    SPI_BUFFERS.Length = WRI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_WRB()
 * Description : Write multiple bytes to the buffer starting at an address
 * Arguments : address: Base address to start writing, *data: Data to be written, dataSize: Number of bytes in data
 * Return Value : none
 ***********************************************************************************************************************/
void lf_ata5293_WRB(uint8_t address, uint8_t length, uint8_t *data)
{
    uint8_t *tmpPtr = &SPI_BUFFERS.Tx[3];
    copyData(SPI_BUFFERS.Tx, (uint8_t *)WRB, WRB_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Tx[2] = length;
    for (uint8_t i = 0; i < length; i++)
    {
        *tmpPtr++ = data[i];
    }
    SPI_BUFFERS.Length = WRB_LENGTH + length - 1;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_WRAR()
 * Description : Write multiple bytes to the buffer starting at an address
 * Arguments : address: Base address to start writing, *data: Data to be written, dataSize: Number of bytes in data
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_WRAR(uint8_t address, uint8_t length, uint8_t *data)
{
    uint8_t *tmpPtr = &SPI_BUFFERS.Tx[3];
    copyData(SPI_BUFFERS.Tx, (uint8_t *)WRAR, WRAR_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Tx[2] = length;
    for (uint8_t i = 0; i < length; i++)
    {
        *tmpPtr++ = data[i];
    }
    SPI_BUFFERS.Length = WRAR_LENGTH + length - 1;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SGP()
 * Description : Set GPIO pin to a high or low
 * Arguments : pin: Pin to modify (0-3), level: Set state to high or low (0-1)
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SGP(uint8_t pin, uint8_t level)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SGP, SGP_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | (pin << 1) | level;
    SPI_BUFFERS.Length = SGP_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_LD()
 * Description : Load data from given Address to register R0..7
 * Arguments : reg: Register to load address value into, address: Address to fetch data from
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_LD(uint8_t reg, uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)LD, LD_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | reg;
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = LD_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_ST()
 * Description : Store data in register (R0-R7) to address
 * Arguments : reg: Register to fetch data from, address: Address to store data
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_ST(uint8_t reg, uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)ST, ST_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | reg;
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = ST_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_PUSH()
 * Description : Store data from address into data stack
 * Arguments : Address to fetch data from
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_PUSH(uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)PUSH, PUSH_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = PUSH_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}
/***********************************************************************************************************************
 * Function Name: lf_ata5293_POP()
 * Description : Load data from data stack into address
 * Arguments : Address to store data to
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_POP(uint8_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)POP, POP_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = POP_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
                  _             _ _              __ _
                 | |           | | |            / _| |
   ___ ___  _ __ | |_ _ __ ___ | | | ___ _ __  | |_| | _____      __
  / __/ _ \| '_ \| __| '__/ _ \| | |/ _ \ '__| |  _| |/ _ \ \ /\ / /
 | (_| (_) | | | | |_| | | (_) | | |  __/ |    | | | | (_) \ V  V /
  \___\___/|_| |_|\__|_|  \___/|_|_|\___|_|    |_| |_|\___/ \_/\_/
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SKIF()
 * Description : Skip next instruction if flag F in status register SQSTR is set.
 * Arguments :  flags: Bit location of flags in SQSTR to check
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SKIF(uint8_t flags)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SKIF, SKIF_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | flags;
    SPI_BUFFERS.Length = SKIF_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SKINF()
 * Description : Description : Skip next instruction if flag F in status register SQSTR is not set.
 * Arguments :  flags: Bit location of flags in SQSTR to check
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SKINF(uint8_t flags)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SKINF, SKINF_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | flags;
    SPI_BUFFERS.Length = SKINF_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_CALLA()
 * Description : Call Address. Jumps to the PC address A. The return address (to the instruction after the CALLA) will
 *               be stored onto the Stack.
 * Arguments : address: Address to jump to
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CALLA(uint16_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CALLA, CALLA_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | ((address >> 8) & (0x00FF));
    SPI_BUFFERS.Tx[1] = address & 0x00FF;
    SPI_BUFFERS.Length = CALLA_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_JMP()
 * Description : Description : Jumps to the PC Address A.
 * Arguments : address: Address to jump to
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_JMP(uint16_t address)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)JMP, JMP_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | ((address >> 8) & (0x00FF));
    SPI_BUFFERS.Tx[1] = address & 0x00FF;
    SPI_BUFFERS.Length = JMP_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_CALL()
 * Description : Call sub-routine locate @ label L address and store return address on stack. The return address will be
 *               stored onto the stack
 * Arguments : label: Label to call
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CALL(uint8_t label)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CALL, CALL_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | label;
    SPI_BUFFERS.Length = CALL_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_WAIT()
 * Description : Wait for a timed defined  by the prescaler P and the counter value N. The wait time depends on the LF
 *               clock and therefore multiples of 8祍 can be programmed.
 * Arguments : prescaler: Prescaler value, N: Counter value
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_WAIT(uint8_t prescaler, uint8_t counter)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)WAIT, WAIT_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | prescaler;
    SPI_BUFFERS.Tx[1] = counter;
    SPI_BUFFERS.Length = WAIT_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_JMPL()
 * Description : Jumps to Label L
 * Arguments : label: Label to jump to
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_JMPL(uint8_t label)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)JMPL, JMPL_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | label;
    SPI_BUFFERS.Length = JMPL_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_DBNZ()
 * Description : Decrement Sequencer loop register SQLP and branch if not zero. Branch target is label  L [0..3]
 *               Skip to next address if decrement result is zero.
 * Arguments : label: Label to branch to
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_DBNZ(uint8_t label)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)DBNZ, DBNZ_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | label;
    SPI_BUFFERS.Length = DBNZ_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_CONFC()
 * Description : Configure external or internal condition mask register R (SQECM0, SQECM1, SQICM0, SQICM1) with given
 *               mask M.
 * Arguments : reg: Register(SQECM0,SQECM1,SQICM0,SQICM1,SQICM1), mask: Mask value
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CONFC(uint8_t reg, uint8_t mask)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CONFC, CONFC_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | reg;
    SPI_BUFFERS.Tx[1] = mask;
    SPI_BUFFERS.Length = CONFC_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_CLRC()
 * Description : Clears all external or internal condition mask registers (SQECM0, SQECM1, SQICM0, SQICM1 and SQICM2).
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CLRC()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CLRC, CLRC_LENGTH);
    SPI_BUFFERS.Length = CLRC_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_WAITC()
 * Description : Wait for condition flag F and value V in SQSTR register
 * Arguments : value: ?, flag: Conditions (ICO, ICA, ECO, ECA)
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_WAITC(uint8_t value, uint8_t flag)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)WAITC, WAITC_LENGTH);
    SPI_BUFFERS.Tx[1] = SPI_BUFFERS.Tx[1] | (value << 2) | flag;
    SPI_BUFFERS.Length = WAITC_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_RET()
 * Description : Return from subroutine
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_RET()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)RET, RET_LENGTH);
    SPI_BUFFERS.Length = RET_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: ata5293_JMPP()
 * Description : Jumps to sequencer address pointer address (SQAPA, SQAPB)
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_JMPP()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)JMPP, JMPP_LENGTH);
    SPI_BUFFERS.Length = JMPP_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_TOUT()
 * Description : Set and start timeout timer. Setting T to greate zero resets and starts/restarts the timeout timer.
 *               Setting T to zero disables the timeout timer.
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_TOUT(uint8_t t)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)TOUT, TOUT_LENGTH);
    SPI_BUFFERS.Tx[1] = t;
    SPI_BUFFERS.Length = TOUT_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
  _                   _       _           _
 | |                 | |     | |         | |
 | |___  __  _ __ ___| | __ _| |_ ___  __| |
 | __\ \/ / | '__/ _ \ |/ _` | __/ _ \/ _` |
 | |_ >  <  | | |  __/ | (_| | ||  __/ (_| |
  \__/_/\_\ |_|  \___|_|\__,_|\__\___|\__,_|
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lf_ata5293_PTX()
 * Description: Pause the TX LF transmission for a duration defined by T.
 * Arguments : pauseDuration: Pause LF transmission by 2 * pauseDuration * TxSymbolDuration (Range: 0x00-0x0F)
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_PTX(uint8_t pauseDuration)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)PTX, PTX_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | pauseDuration;
    SPI_BUFFERS.Length = PTX_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_FDXCC()
 * Description: Trigger FDX Carrier Compensation in single ended or differential mode.
 * Arguments : enable: enable/disable compensation
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_FDXCC(uint8_t enable)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)FDXCC, FDXCC_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | enable;
    SPI_BUFFERS.Length = FDXCC_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SC()
 * Description: Send carrier.
 * Arguments : carrierDuration: Length of carrier defined by 2 * carrierDuration * TxSymbolDuration
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SC(uint8_t carrierDuration)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SC, SC_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | (carrierDuration & 0x0F);
    SPI_BUFFERS.Length = SC_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SDR()
 * Description: Select and enable driver.
 * Arguments : drivers: Which antenna drivers to select, enable: Enable or disable all selected drivers
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SDR(uint16_t drivers, uint8_t enable)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SDR, SDR_LENGTH);
    SPI_BUFFERS.Tx[1] = drivers;                                 // 截断16位drivers驱动低8位放到Tx[1]中
    SPI_BUFFERS.Tx[2] = ((drivers >> 8) & 0x03) | (enable << 7); // 将16位drivers驱动高8位的低两位放到Tx[2]中
    SPI_BUFFERS.Length = SDR_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SAP()
 * Description: Store antenna parameters.
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SAP()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SAP, SAP_LENGTH);
    SPI_BUFFERS.Length = SAP_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SCC()
 * Description: Set coil current.
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SCC(uint8_t coilCurrent)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SCC, SCC_LENGTH);
    SPI_BUFFERS.Tx[1] = coilCurrent;
    SPI_BUFFERS.Length = SCC_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SD()
 * Description: Send Data.
 * Arguments : bitsToSend: Number of data bits to send, data: Data to send
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SD(uint16_t bitsToSend, uint8_t *data)
{

    uint8_t dataSize = ((bitsToSend) / 8) + (((bitsToSend) % 8 > 0) ? 1 : 0);
    uint8_t *tmpPtr = &SPI_BUFFERS.Tx[2];
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SD, SD_LENGTH);
    SPI_BUFFERS.Tx[1] = bitsToSend - 1;
    for (uint8_t i = 0; i < dataSize; i++)
    {
        *tmpPtr++ = data[i];
    }
    SPI_BUFFERS.Length = SD_LENGTH + dataSize - 1;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
                       _       _           _
                      | |     | |         | |
  _ ____  __  _ __ ___| | __ _| |_ ___  __| |
 | '__\ \/ / | '__/ _ \ |/ _` | __/ _ \/ _` |
 | |   >  <  | | |  __/ | (_| | ||  __/ (_| |
 |_|  /_/\_\ |_|  \___|_|\__,_|\__\___|\__,_|
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lf_ata5293_IRO()
 * Description: Enables or disables RX output
 * Arguments : enable: Boolean variable set to 1 to enable and 0 to disable rx output.
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_IRO(uint8_t enable)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)RX, RX_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | enable;
    SPI_BUFFERS.Length = RX_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
                           _           _   _              _____ _____ _____
                          | |         | | | |            / ____|  __ \_   _|
   _____  _____  ___ _   _| |_ ___  __| | | |__  _   _  | (___ | |__) || |
  / _ \ \/ / _ \/ __| | | | __/ _ \/ _` | | '_ \| | | |  \___ \|  ___/ | |
 |  __/>  <  __/ (__| |_| | ||  __/ (_| | | |_) | |_| |  ____) | |    _| |_
  \___/_/\_\___|\___|\__,_|\__\___|\__,_| |_.__/ \__, | |_____/|_|   |_____|
                                                  __/ |
                                                 |___/
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lf_ata5293_CLR()
 * Description: Clear the main buffer and/or sub-buffers of the sequence control. Selection is done by setting N[1:0] to
 *              the corresponding value.
 * Arguments : bufferSelection: Which buffer(s) are to be cleared
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CLR(uint8_t bufferSelection)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CLR, CLR_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | bufferSelection;
    SPI_BUFFERS.Length = CLR_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SSC()
 * Description: Sets the SPI configuration: polarity CPOL and phase CPHA
 * Arguments : enable: Boolean variable set to 1 to enable and 0 to disable rx output.
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SSC(uint8_t spiSetting)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SSC, SSC_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | spiSetting;
    SPI_BUFFERS.Length = SSC_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_GSI()
 * Description: Get status information returns the functional status stored in the STR and FAR registers
 *              (Status / Fault Register). IRQ sources and global fault bits. More details are available in the fault
 *              registers of the corresponding blocks.
 * Arguments : unknown: TBD
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_GSI(uint8_t *returnedData)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)GSI, GSI_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0];
    SPI_BUFFERS.Length = GSI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, &SPI_BUFFERS.Tx[0], SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    for (uint16_t i = 0; i < (SPI_BUFFERS.Length - GSI_FIRST_RXD_BYTE); i++)
    {
        returnedData[i] = SPI_BUFFERS.Rx[i + GSI_FIRST_RXD_BYTE];
    }
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_RFS()
 * Description: Resets the fault registers and re-enables operability of the IC in the event of a prior fault shutdown
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_RFS()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)RFS, RFS_LENGTH);
    SPI_BUFFERS.Length = RFS_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, &SPI_BUFFERS.Tx[0], SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SWRI()
 * Description: Write data to register address.
 * Arguments : enable: Boolean variable set to 1 to enable and 0 to disable rx output.
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SWRI(uint8_t address, uint8_t data)
{
    memset(SPI_BUFFERS.Rx, 0, GID_LENGTH);
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SWRI, SWRI_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Tx[2] = data;
    SPI_BUFFERS.Length = SWRI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, &SPI_BUFFERS.Tx[0], SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SWRB()
 * Description: Write N[7:0] +1 bytes to buffer. The data bytes N[7:0] +1  are usually a sequence of commands.
 *              揗ain Buffer?wraps around during writing. 揝ub- Buffer?1-3 fill up and stop at the end.
 *              A buffer overflow generates an error.
 * Arguments : address: Address to start writing at, length: How many bytes to write, data: Data to write.
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SWRB(uint8_t address, uint8_t length, uint8_t *data)
{
    uint8_t *tmpPtr = &SPI_BUFFERS.Tx[3];
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SWRB, SWRB_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Tx[2] = length;
    for (uint8_t i = 0; i < length; i++)
    {
        *tmpPtr++ = data[i];
    }
    SPI_BUFFERS.Length = SWRB_LENGTH + length - 1;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SORI()
 * Description: Bitwise OR of immediate value <imval> with the destination address <address(d)>. Register SQSTR is not
 *              affected. Operation: Rd <- Rd | K
 * Arguments : address: Address of data to OR data with value and store back at address, value: Value to OR address with.
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SORI(uint8_t address, uint8_t value)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SORI, SORI_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Tx[2] = value;
    SPI_BUFFERS.Length = SORI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SANDI()
 * Description: Bitwise AND of immediate value <imval> with the destination address <address(d)>. Register SQSTR is not
 *              affected. Operation: Rd <- Rd & K
 * Arguments : address: Address of data to AND data with value and store back at address, value: Value to AND address with.
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SANDI(uint8_t address, uint8_t value)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SANDI, SANDI_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Tx[2] = value;
    SPI_BUFFERS.Length = SANDI_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SRD()
 * Description: Read access to a register. Data is transferred during 4th byte (<dummy 1>).
 * Arguments : address: Address to read.
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SRD(uint8_t address, uint8_t *returnedData)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SRD, SRD_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Length = SRD_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, &SPI_BUFFERS.Tx[0], SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    for (uint16_t i = 0; i < (SPI_BUFFERS.Length - SRD_FIRST_RXD_BYTE); i++)
    {
        returnedData[i] = SPI_BUFFERS.Rx[i + SRD_FIRST_RXD_BYTE];
    }
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SLBL()
 * Description: Writes 3 LSB Bits of command to SQLBCT register (Sequencer Label Control Register)
 * Arguments : labelSource: buffer selection, labelRegister: Label register to update
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SLBL(uint8_t labelSource, uint8_t labelRegister)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SLBL, SLBL_LENGTH);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | (labelSource << 2);
    SPI_BUFFERS.Tx[0] = SPI_BUFFERS.Tx[0] | labelRegister;
    SPI_BUFFERS.Length = SLBL_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, &SPI_BUFFERS.Tx[0], SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_SRB()
 * Description: Reads a data block <data> of length <N>+1 from address <addr>. addr = 0 ..255, N = 0 .. 255,
 *              data = 1 ?N+1 Bytes
 * Arguments : address: Address to start reading from, length: How many bytes to read
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_SRB(uint8_t address, uint8_t length, uint8_t *returnedData)
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)SRB, SRB_LENGTH);
    SPI_BUFFERS.Tx[1] = address;
    SPI_BUFFERS.Tx[2] = length;
    SPI_BUFFERS.Length = SRB_LENGTH + length;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, &SPI_BUFFERS.Tx[0], SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    for (uint16_t i = 0; i < (SPI_BUFFERS.Length - SRB_FIRST_RXD_BYTE); i++)
    {
        returnedData[i] = SPI_BUFFERS.Rx[i + SRB_FIRST_RXD_BYTE];
    }
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_HALT()
 * Description: Halts execution immediately (set HEXE bit in SQCTRL register).
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_HALT()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)HALT, HALT_LENGTH);
    SPI_BUFFERS.Length = HALT_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_RUN()
 * Description: Starts execution immediately (clear HEXE bit in SQCTRL register).
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_RUN()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)RUN, RUN_LENGTH);
    SPI_BUFFERS.Length = RUN_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_CRX()
 * Description: Clears the RX buffer and resets the corresponding status flag.
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CRX()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CRX, CRX_LENGTH);
    SPI_BUFFERS.Length = CRX_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_CDH()
 * Description: Set DHCLR bit in DHFCTL register (Door Handle Fifo Control Register)
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_CDH()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)CDH, CDH_LENGTH);
    SPI_BUFFERS.Length = CDH_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
 * Function Name: lf_ata5293_NOP()
 * Description: No operation. Last SPI command is shifted through MISO
 * Arguments : none
 * Return Value : none
 ***********************************************************************************************************************/

void lf_ata5293_NOP()
{
    copyData(SPI_BUFFERS.Tx, (uint8_t *)NOP, NOP_LENGTH);
    SPI_BUFFERS.Length = NOP_LENGTH;
    // gpio_set_pin_level(FTS_NCS, FALSE);
    SPI_DRV_MasterTransferBlocking(SPI_INSTANCE, SPI_BUFFERS.Tx, SPI_BUFFERS.Rx, SPI_BUFFERS.Length, SPI_TIMEOUT);
    while (SPI_DRV_MasterGetTransferStatus(SPI_INSTANCE, NULL) == STATUS_BUSY)
        ;
    // gpio_set_pin_level(FTS_NCS, TRUE);
}

/***********************************************************************************************************************
             _
            (_)
   _ __ ___  _ ___  ___
  | '_ ` _ \| / __|/ __|
  | | | | | | \__ \ (__ _
  |_| |_| |_|_|___/\___(_)
***********************************************************************************************************************/
void copyData(uint8_t *CMD_BUFFER, uint8_t *CMD, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        CMD_BUFFER[i] = CMD[i];
    }
    // strcpy(CMD_BUFFER, CMD);
}

uint8_t checkStatus(void)
{
    uint8_t Status_Fault[2];
    lf_ata5293_GSI(&Status_Fault[0]);
    return Status_Fault[1];
}

void DriverOutput(uint16_t driver, uint8_t driverConfiguration, uint8_t waveFormType, uint8_t path)
{
    switch (driver)
    {
    case DRIVER_A00:
        lf_ata5293_ANDI(DRC0, 0xF0);
        lf_ata5293_ORI(DRC0, (driverConfiguration | path | waveFormType));
        break;
    case DRIVER_A01:
        lf_ata5293_ANDI(DRC0, 0x0F);
        lf_ata5293_ORI(DRC0, (driverConfiguration | path | waveFormType) << 4);
        break;
    case DRIVER_A02:
        lf_ata5293_ANDI(DRC1, 0xF0);
        lf_ata5293_ORI(DRC1, (driverConfiguration | path | waveFormType));
        break;
    case DRIVER_A10:
        lf_ata5293_ANDI(DRC1, 0x0F);
        lf_ata5293_ORI(DRC1, (driverConfiguration | path | waveFormType) << 4);
        break;
    case DRIVER_A11:
        lf_ata5293_ANDI(DRC2, 0xF0);
        lf_ata5293_ORI(DRC2, (driverConfiguration | path | waveFormType));
        break;
    case DRIVER_A20:
        lf_ata5293_ANDI(DRC2, 0x0F);
        lf_ata5293_ORI(DRC2, (driverConfiguration | path | waveFormType) << 4);
        break;
    case DRIVER_A21:
        lf_ata5293_ANDI(DRC3, 0xF0);
        lf_ata5293_ORI(DRC3, (driverConfiguration | path | waveFormType));
        break;
    case DRIVER_A22:
        lf_ata5293_ANDI(DRC3, 0x0F);
        lf_ata5293_ORI(DRC3, (driverConfiguration | path | waveFormType) << 4);
        break;
    case DRIVER_A30:
        lf_ata5293_ANDI(DRC4, 0xF0);
        lf_ata5293_ORI(DRC4, (driverConfiguration | path | waveFormType));
        break;
    case DRIVER_A31:
        lf_ata5293_ANDI(DRC4, 0x0F);                                            // 初始化A31与A30,清除之前的指令。 A31 为 “Tx Path0 + 正极性正弦波模式”，配置 A30 为 “Tx Path1 + 静态接 VDS 模式”
        lf_ata5293_ORI(DRC4, (driverConfiguration | path | waveFormType) << 4); //  单独配置 A31为“Tx Path0 + 正极性正弦波模式”
        break;
    }
}

void lf_SelectCoilCurrent(uint8_t coilcurrent, uint16_t driver, uint8_t path)
{
    uint8_t i = 0;
    uint16_t c;

    do
    {
        c = driver & (0x0001 << i);
        i++;
    } while (c == 0);
    i--;
    if (path == PATH1)
    {
        lf_ata5293_WRI(PWCMS1, (0xF0 | i));
        lf_ata5293_WRI(PWPPCC1, coilcurrent);
    }
    else
    {
        lf_ata5293_WRI(PWCMS0, (0xF0 | i));
        lf_ata5293_SCC(coilcurrent);
    }
}

uint8_t get_current_setting(uint16_t current)
{
    uint8_t temp;
    if (current <= 183)
    {
        temp = ((current - 101) / 3.75) + 0.4;
    }
    else if (current <= 381)
    {
        temp = ((current - 180) / 7.5) + 25.4;
    }
    else if (current <= 613)
    {
        temp = ((current - 375) / 12.5) + 66.4;
    }
    else
    {
        temp = ((current - 600) / 25) + 88.4;
    }
    return temp;
}
// unsigned char *Manchester2spi(unsigned char manchesterByte, unsigned char *bufferPtr)
//{
//     unsigned char lookUp[16] = {//上升沿代表0，下降沿代表1
//     0x55, // 索引0（输入0000）→ 反转后1111 → 扩展01010101 → 0x55
//     0x56, // 索引1（输入0001）→ 反转后1110 → 扩展01010110 → 0x56
//     0x59, // 索引2（输入0010）→ 反转后1101 → 扩展01011001 → 0x59
//     0x5A, // 索引3（输入0011）→ 反转后1100 → 扩展01011010 → 0x5A
//     0x65, // 索引4（输入0100）→ 反转后1011 → 扩展01100101 → 0x65
//     0x66, // 索引5（输入0101）→ 反转后1010 → 扩展01100110 → 0x66
//     0x69, // 索引6（输入0110）→ 反转后1001 → 扩展01101001 → 0x69
//     0x6A, // 索引7（输入0111）→ 反转后1000 → 扩展01101010 → 0x6A
//     0x95, // 索引8（输入1000）→ 反转后0111 → 扩展10010101 → 0x95
//     0x96, // 索引9（输入1001）→ 反转后0110 → 扩展10010110 → 0x96
//     0x99, // 索引10（输入1010）→ 反转后0101 → 扩展10011001 → 0x99
//     0x9A, // 索引11（输入1011）→ 反转后0100 → 扩展10011010 → 0x9A
//     0xA5, // 索引12（输入1100）→ 反转后0011 → 扩展10100101 → 0xA5
//     0xA6, // 索引13（输入1101）→ 反转后0010 → 扩展10100110 → 0xA6
//     0xA9, // 索引14（输入1110）→ 反转后0001 → 扩展10101001 → 0xA9
//     0xAA  // 索引15（输入1111）→ 反转后0000 → 扩展10101010 → 0xAA
// };
//     //unsigned char lookUp[16] = {0xAA, 0xA9, 0xA6, 0xA5, 0x9A, 0x99, 0x96, 0x95, 0x6A, 0x69, 0x66, 0x65, 0x5A, 0x59, 0x56, 0x55};//上升沿代表1，下降沿代表0
//     *bufferPtr++ = lookUp[(manchesterByte >> 4)];
//     *bufferPtr++ = lookUp[(manchesterByte & 0x0F)];
//     return bufferPtr;
// }
unsigned char *Manchester2spi(unsigned char manchesterByte, unsigned char *bufferPtr)
{
    unsigned char lookUp[16] = {0xAA, 0xA9, 0xA6, 0xA5, 0x9A, 0x99, 0x96, 0x95, 0x6A, 0x69, 0x66, 0x65, 0x5A, 0x59, 0x56, 0x55};
    *bufferPtr++ = lookUp[(manchesterByte >> 4)];
    *bufferPtr++ = lookUp[(manchesterByte & 0x0F)];
    return bufferPtr;
}
