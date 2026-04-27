/*
 * fortissimo.h
 *
 * Created: 1/25/2018 5:02:48 AM
 *  Author: C15326
 */
/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#ifndef FORTISSIMO_H_
#define FORTISSIMO_H_

#include <stdint.h>

typedef struct
{
    unsigned char SGap;
    unsigned char WGap;
    unsigned char One;
    unsigned char Zero;
} FORTISSIMO_IMMO_TIMING_St;

/*External Variables*/
extern const uint8_t GDA[1];
extern const uint8_t GID[1];
extern const uint8_t GSLP[1];
extern const uint8_t GTX[1];
extern const uint8_t GRH[1];
extern const uint8_t GFDX[1];
extern const uint8_t SBI[2];
extern const uint8_t CBI[2];
extern const uint8_t LSLI[2];
extern const uint8_t LSRI[2];
extern const uint8_t AND[3];
extern const uint8_t ANDI[3];
extern const uint8_t OR[3];
extern const uint8_t ORI[3];
extern const uint8_t ADD[3];
extern const uint8_t ADDI[3];
extern const uint8_t SUB[3];
extern const uint8_t SUBI[3];
extern const uint8_t CP[3];
extern const uint8_t NOT[2];
extern const uint8_t WRB[3];
extern const uint8_t WRI[3];
extern const uint8_t SGP[1];
extern const uint8_t LD[2];
extern const uint8_t ST[2];
extern const uint8_t SKIF[1];
extern const uint8_t SKINF[1];
extern const uint8_t CALLA[2];
extern const uint8_t JMP[2];
extern const uint8_t CALL[1];
extern const uint8_t WAIT[2];
extern const uint8_t JMPL[1];
extern const uint8_t DBNZ[1];
extern const uint8_t CONFC[2];
extern const uint8_t CLRC[1];
extern const uint8_t WAITC[2];
extern const uint8_t RET[1];
extern const uint8_t JMPP[1];
extern const uint8_t TOUT[2];
extern const uint8_t PTX[1];
extern const uint8_t FDXCC[1];
extern const uint8_t SC[1];
extern const uint8_t SDR[3];
extern const uint8_t SAP[1];
extern const uint8_t SCC[2];
extern const uint8_t SD[3];
extern const uint8_t RX[1];
extern const uint8_t CLR[1];
extern const uint8_t SSC[1];
extern const uint8_t GSI[3];
extern const uint8_t RFS[1];
extern const uint8_t SWRI[3];
extern const uint8_t SWRB[4];
extern const uint8_t SORI[3];
extern const uint8_t SANDI[3];
extern const uint8_t SRD[4];
extern const uint8_t SLBL[1];
extern const uint8_t SRB[5];
extern const uint8_t HALT[1];
extern const uint8_t NOP[1];
extern const uint8_t RUN[1];
extern const uint8_t CRX[1];
extern const uint8_t CDH[1];
extern uint8_t active_antenna;

/*DEFINES*/

// CMD LENGTHS
#define GDA_LENGTH 1
#define GID_LENGTH 1
#define GSLP_LENGTH 1
#define GTX_LENGTH 1
#define GRH_LENGTH 1
#define GFDX_LENGTH 1
#define SBI_LENGTH 2
#define CBI_LENGTH 2
#define LSLI_LENGTH 2
#define LSRI_LENGTH 2
#define AND_LENGTH 3
#define ANDI_LENGTH 3
#define OR_LENGTH 3
#define ORI_LENGTH 3
#define ADD_LENGTH 3
#define ADDI_LENGTH 3
#define SUB_LENGTH 3
#define SUBI_LENGTH 3
#define CP_LENGTH 3
#define NOT_LENGTH 2
#define WRI_LENGTH 3
#define WRB_LENGTH 4
#define WRAR_LENGTH 4
#define SGP_LENGTH 1
#define LD_LENGTH 2
#define ST_LENGTH 2
#define PUSH_LENGTH 2
#define POP_LENGTH 2
#define SKIF_LENGTH 1
#define SKINF_LENGTH 1
#define CALLA_LENGTH 2
#define JMP_LENGTH 2
#define CALL_LENGTH 1
#define WAIT_LENGTH 2
#define JMPL_LENGTH 1
#define DBNZ_LENGTH 1
#define CONFC_LENGTH 2
#define CLRC_LENGTH 1
#define WAITC_LENGTH 2
#define RET_LENGTH 1
#define JMPP_LENGTH 1
#define TOUT_LENGTH 2
#define PTX_LENGTH 1
#define SJ_LENGTH 3
#define FDXCC_LENGTH 1
#define SC_LENGTH 1
#define SDR_LENGTH 3
#define SAP_LENGTH 1
#define SCC_LENGTH 2
#define SD_LENGTH 3
#define RX_LENGTH 1
#define CLR_LENGTH 1
#define SSC_LENGTH 1
#define GSI_LENGTH 3
#define RFS_LENGTH 1
#define SWRI_LENGTH 3
#define SWRB_LENGTH 4
#define SORI_LENGTH 3
#define SANDI_LENGTH 3
#define SRD_LENGTH 4
#define SLBL_LENGTH 1
#define SRB_LENGTH 5
#define HALT_LENGTH 1
#define RUN_LENGTH 1
#define CRX_LENGTH 1
#define CDH_LENGTH 1
#define NOP_LENGTH 1

// CMD DATA RETURN LOCATIONS
#define GSI_FIRST_RXD_BYTE 1
#define SRD_FIRST_RXD_BYTE 3
#define SRB_FIRST_RXD_BYTE 4
#define NO_RETURN 0

// BITS
#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7

#define _0BITS 0
#define _1BITS 1
#define _2BITS 2
#define _3BITS 3
#define _4BITS 4
#define _5BITS 5
#define _6BITS 6
#define _7BITS 7

// PINS
#define PIN0 0
#define PIN1 1
#define PIN2 2
#define PIN3 3

// LOGIC LEVELS
#define LOGIC_LOW 0
#define LOGIC_HIGH 1

// LOGIC LEVELS
#define VALUE_LOW 0
#define VALUE_HIGH 1

// REGISTERS
#define REG0 0
#define REG1 1
#define REG2 2
#define REG3 3
#define REG4 4
#define REG5 5
#define REG6 6
#define REG7 7

// FLAGS
#define FLAG_CARRY 0
#define FLAG_ZERO 1
#define FLAG_OVERFLOW 2
#define FLAG_NEGATIVE 3

// CONDITIONS
#define ICO 0
#define ICA 1
#define ECO 2
#define ECA 3

// LABELS
#define LABEL0 0
#define LABEL1 1
#define LABEL2 2
#define LABEL3 3

// PRESCALER
#define PS0 0
#define PS1 1
#define PS2 2
#define PS3 3

// REGISTERS
#define SQECM0_LOC 0
#define SQECM1_LOC 1
#define SQICM0_LOC 2
#define SQICM1_LOC 3
#define SQICM2_LOC 4

// JAMMING BITS
#define _1_JAM_BITS 0
#define _2_JAM_BITS 1
#define _3_JAM_BITS 2
#define _4_JAM_BITS 3
#define _5_JAM_BITS 4
#define _6_JAM_BITS 5
#define _7_JAM_BITS 6
#define _8_JAM_BITS 7
#define _9_JAM_BITS 8
#define _10_JAM_BITS 9
#define _11_JAM_BITS 10
#define _12_JAM_BITS 11
#define _13_JAM_BITS 12
#define _14_JAM_BITS 13
#define _15_JAM_BITS 14
#define _16_JAM_BITS 15

// BOOLEAN
#define TRUE 1
#define FALSE 0

// BUFFERS
#define NO_BUFFERS 0
#define MAIN_BUFFER 1
#define SUB_BUFFER 2
#define BOTH_BUFFERS 3

// SPI SETTINGS
#define CPOL0_CPHA0 4
#define CPOL0_CPHA1 5
#define CPOL1_CPHA0 6
#define CPOL1_CPHA1 7

// BUFFERS
#define SQLBL0_LOC 0
#define SQLBL1_LOC 1
#define SQLBL2_LOC 2
#define SQLBL3_LOC 3

// DRIVER indexes
#define DRIVER_A00 0x0001
#define DRIVER_A01 0x0002
#define DRIVER_A02 0x0004
#define DRIVER_A10 0x0008
#define DRIVER_A11 0x0010
#define DRIVER_A20 0x0020
#define DRIVER_A21 0x0040
#define DRIVER_A22 0x0080
#define DRIVER_A30 0x0100
#define DRIVER_A31 0x0200

// COIL CURRENTS
//  V1.1
/*
#define CURRENT_100mA				  0
#define CURRENT_106_25mA			  1
#define CURRENT_112_5mA			  2
#define CURRENT_118_75mA			  3
#define CURRENT_125mA				  4
#define CURRENT_131_25mA			  5
#define CURRENT_137_5mA			  6
#define CURRENT_143_75mA			  7
#define CURRENT_150mA				  8
#define CURRENT_156_25mA			  9
#define CURRENT_162_5mA			  10
#define CURRENT_168_75mA			  11
#define CURRENT_175mA				  12
#define CURRENT_181_25mA			  13
#define CURRENT_187_5mA			  14
#define CURRENT_193_75mA			  15
#define CURRENT_200mA				  16
#define CURRENT_206_25mA			  17
#define CURRENT_212_5mA			  18
#define CURRENT_218_75mA			  19
#define CURRENT_225mA				  20
#define CURRENT_231_25mA			  21
#define CURRENT_237_5mA			  22
#define CURRENT_243_75mA			  23
#define CURRENT_250mA_Range0    	  24
#define CURRENT_250mA_Range1		  32
#define CURRENT_262_5mA			  33
#define CURRENT_275mA				  34
#define CURRENT_287_5mA			  35
#define CURRENT_300mA				  36
#define CURRENT_312_5mA			  37
#define CURRENT_325mA				  38
#define CURRENT_337_5mA			  39
#define CURRENT_350mA				  40
#define CURRENT_362_5mA			  41
#define CURRENT_375mA				  42
#define CURRENT_387_5mA			  43
#define CURRENT_400mA				  44
#define CURRENT_412_5mA			  45
#define CURRENT_425mA				  46
#define CURRENT_437_5mA			  47
#define CURRENT_450mA				  48
#define CURRENT_462_5mA			  49
#define CURRENT_475mA				  50
#define CURRENT_487_5mA			  51
#define CURRENT_500mA				  52
#define CURRENT_512_5mA			  53
#define CURRENT_525mA				  54
#define CURRENT_537_5mA			  55
#define CURRENT_550mA				  56
#define CURRENT_562_5mA			  57
#define CURRENT_575mA				  58
#define CURRENT_587_5mA			  59
#define CURRENT_600mA_Range1		  60
#define CURRENT_600mA_Range2		  64
#define CURRENT_625mA				  65
#define CURRENT_650mA				  66
#define CURRENT_675mA				  67
#define CURRENT_700mA				  68
#define CURRENT_725mA				  69
#define CURRENT_750mA				  70
#define CURRENT_775mA				  71
#define CURRENT_800mA				  72
#define CURRENT_825mA				  73
#define CURRENT_850mA				  74
#define CURRENT_875mA				  75
#define CURRENT_900mA				  76
#define CURRENT_925mA				  77
#define CURRENT_950mA				  78
#define CURRENT_975mA				  79
#define CURRENT_1000mA				  80
#define CURRENT_1025mA				  81
#define CURRENT_1050mA				  82
#define CURRENT_1075mA				  83
#define CURRENT_1100mA				  84
#define CURRENT_1125mA				  85
#define CURRENT_1150mA				  86
#define CURRENT_1175mA				  87
#define CURRENT_1200mA				  88
#define CURRENT_1225mA				  89
#define CURRENT_1250mA				  90

//COIL CURRENTS DOUBLED
#define CURRENTx2_100mA				  0&0x80
#define CURRENTx2_106_25mA			  1&0x80
#define CURRENTx2_112_5mA			  2&0x80
#define CURRENTx2_118_75mA			  3&0x80
#define CURRENTx2_125mA				  4&0x80
#define CURRENTx2_131_25mA			  5&0x80
#define CURRENTx2_137_5mA			  6&0x80
#define CURRENTx2_143_75mA			  7&0x80
#define CURRENTx2_150mA				  8&0x80
#define CURRENTx2_156_25mA			  9&0x80
#define CURRENTx2_162_5mA			  10&0x80
#define CURRENTx2_168_75mA			  11&0x80
#define CURRENTx2_175mA				  12&0x80
#define CURRENTx2_181_25mA			  13&0x80
#define CURRENTx2_187_5mA			  14&0x80
#define CURRENTx2_193_75mA			  15&0x80
#define CURRENTx2_200mA				  16&0x80
#define CURRENTx2_206_25mA			  17&0x80
#define CURRENTx2_212_5mA			  18&0x80
#define CURRENTx2_218_75mA			  19&0x80
#define CURRENTx2_225mA				  20&0x80
#define CURRENTx2_231_25mA			  21&0x80
#define CURRENTx2_237_5mA			  22&0x80
#define CURRENTx2_243_75mA			  23&0x80
#define CURRENTx2_250mA_Range0    	  24&0x80
#define CURRENTx2_250mA_Range1		  32&0x80
#define CURRENTx2_262_5mA			  33&0x80
#define CURRENTx2_275mA				  34&0x80
#define CURRENTx2_287_5mA			  35&0x80
#define CURRENTx2_300mA				  36&0x80
#define CURRENTx2_312_5mA			  37&0x80
#define CURRENTx2_325mA				  38&0x80
#define CURRENTx2_337_5mA			  39&0x80
#define CURRENTx2_350mA				  40&0x80
#define CURRENTx2_362_5mA			  41&0x80
#define CURRENTx2_375mA				  42&0x80
#define CURRENTx2_387_5mA			  43&0x80
#define CURRENTx2_400mA				  44&0x80
#define CURRENTx2_412_5mA			  45&0x80
#define CURRENTx2_425mA				  46&0x80
#define CURRENTx2_437_5mA			  47&0x80
#define CURRENTx2_450mA				  48&0x80
#define CURRENTx2_462_5mA			  49&0x80
#define CURRENTx2_475mA				  50&0x80
#define CURRENTx2_487_5mA			  51&0x80
#define CURRENTx2_500mA				  52&0x80
#define CURRENTx2_512_5mA			  53&0x80
#define CURRENTx2_525mA				  54&0x80
#define CURRENTx2_537_5mA			  55&0x80
#define CURRENTx2_550mA				  56&0x80
#define CURRENTx2_562_5mA			  57&0x80
#define CURRENTx2_575mA				  58&0x80
#define CURRENTx2_587_5mA			  59&0x80
#define CURRENTx2_600mA_Range1		  60&0x80
#define CURRENTx2_600mA_Range2		  64&0x80
#define CURRENTx2_625mA				  65&0x80
#define CURRENTx2_650mA				  66&0x80
#define CURRENTx2_675mA				  67&0x80
#define CURRENTx2_700mA				  68&0x80
#define CURRENTx2_725mA				  69&0x80
#define CURRENTx2_750mA				  70&0x80
#define CURRENTx2_775mA				  71&0x80
#define CURRENTx2_800mA				  72&0x80
#define CURRENTx2_825mA				  73&0x80
#define CURRENTx2_850mA				  74&0x80
#define CURRENTx2_875mA				  75&0x80
#define CURRENTx2_900mA				  76&0x80
#define CURRENTx2_925mA				  77&0x80
#define CURRENTx2_950mA				  78&0x80
#define CURRENTx2_975mA				  79&0x80
#define CURRENTx2_1000mA			  80&0x80
#define CURRENTx2_1025mA			  81&0x80
#define CURRENTx2_1050mA			  82&0x80
#define CURRENTx2_1075mA			  83&0x80
#define CURRENTx2_1100mA			  84&0x80
#define CURRENTx2_1125mA			  85&0x80
#define CURRENTx2_1150mA			  86&0x80
#define CURRENTx2_1175mA			  87&0x80
#define CURRENTx2_1200mA			  88&0x80
#define CURRENTx2_1225mA			  89&0x80
#define CURRENTx2_1250mA			  90&0x80
*/
// V3
#define CURRENT_101_25mA 0
#define CURRENT_105mA 1
#define CURRENT_108_75mA 2
#define CURRENT_112_50mA 3
#define CURRENT_116_25mA 4
#define CURRENT_120mA 5
#define CURRENT_123_75mA 6
#define CURRENT_127_50mA 7
#define CURRENT_131_25mA 8
#define CURRENT_135mA 9
#define CURRENT_138_75mA 10
#define CURRENT_142_50mA 11
#define CURRENT_146_25mA 12
#define CURRENT_150mA 13
#define CURRENT_153_75mA 14
#define CURRENT_157_50mA 15
#define CURRENT_161_25mA 16
#define CURRENT_165mA 17
#define CURRENT_168_75mA 18
#define CURRENT_172_50mA 19
#define CURRENT_176_25mA 20
#define CURRENT_180mA_Range0 21
#define CURRENT_180mA_Range1 25
#define CURRENT_187_5mA 26
#define CURRENT_195mA 27
#define CURRENT_202_5mA 28
#define CURRENT_210mA 29
#define CURRENT_217_5mA 30
#define CURRENT_225mA 31
#define CURRENT_232_5mA 32
#define CURRENT_240mA 33
#define CURRENT_247_5mA 34
#define CURRENT_255mA 35
#define CURRENT_262_5mA 36
#define CURRENT_270mA 37
#define CURRENT_277_5mA 38
#define CURRENT_285mA 39
#define CURRENT_292_5mA 40
#define CURRENT_300mA 41
#define CURRENT_307_5mA 42
#define CURRENT_315mA 43
#define CURRENT_322_5mA 44
#define CURRENT_330mA 45
#define CURRENT_337_5mA 46
#define CURRENT_345mA 47
#define CURRENT_352_5mA 48
#define CURRENT_360mA 49
#define CURRENT_367_5mA 50
#define CURRENT_375mA_Range1 51
#define CURRENT_375mA_Range2 64
#define CURRENT_362_5mA 65
#define CURRENT_375mA 66
#define CURRENT_387_5mA 67
#define CURRENT_400mA 68
#define CURRENT_412_5mA 69
#define CURRENT_425_5mA 70
#define CURRENT_437_5mA 71
#define CURRENT_450mA 72
#define CURRENT_462_5mA 73
#define CURRENT_475mA 74
#define CURRENT_487_5mA 75
#define CURRENT_500mA 76
#define CURRENT_512_5mA 77
#define CURRENT_525mA 78
#define CURRENT_537_5mA 79
#define CURRENT_550mA 80
#define CURRENT_562_5mA 81
#define CURRENT_575mA 82
#define CURRENT_587_5mA 83
#define CURRENT_600mA_Range2 84
#define CURRENT_600mA_Range3 88
#define CURRENT_625mA 89
#define CURRENT_650mA 90
#define CURRENT_675mA 91
#define CURRENT_700mA 92
#define CURRENT_725mA 93
#define CURRENT_750mA 94
#define CURRENT_775mA 95
#define CURRENT_800mA 96
#define CURRENT_825mA 97
#define CURRENT_850mA 98
#define CURRENT_875mA 99
#define CURRENT_900mA 100
#define CURRENT_925mA 101
#define CURRENT_950mA 102
#define CURRENT_975mA 103
#define CURRENT_1000mA 104
#define CURRENT_1025mA 105
#define CURRENT_1050mA 106
#define CURRENT_1075mA 107
#define CURRENT_1100mA 108
#define CURRENT_1125mA 109
#define CURRENT_1150mA 110
#define CURRENT_1175mA 111
#define CURRENT_1200mA 112
#define CURRENT_1225mA 113
#define CURRENT_1250mA 114
#define CURRENTx2_101_25mA 0 & 0x80
#define CURRENTx2_105mA 1 & 0x80
#define CURRENTx2_108_75mA 2 & 0x80
#define CURRENTx2_112_50mA 3 & 0x80
#define CURRENTx2_116_25mA 4 & 0x80
#define CURRENTx2_120mA 5 & 0x80
#define CURRENTx2_123_75mA 6 & 0x80
#define CURRENTx2_127_50mA 7 & 0x80
#define CURRENTx2_131_25mA 8 & 0x80
#define CURRENTx2_135mA 9 & 0x80
#define CURRENTx2_138_75mA 10 & 0x80
#define CURRENTx2_142_50mA 11 & 0x80
#define CURRENTx2_146_25mA 12 & 0x80
#define CURRENTx2_150mA 13 & 0x80
#define CURRENTx2_153_75mA 14 & 0x80
#define CURRENTx2_157_50mA 15 & 0x80
#define CURRENTx2_161_25mA 16 & 0x80
#define CURRENTx2_165mA 17 & 0x80
#define CURRENTx2_168_75mA 18 & 0x80
#define CURRENTx2_172_50mA 19 & 0x80
#define CURRENTx2_176_25mA 20 & 0x80
#define CURRENTx2_180mA_Range0 21 & 0x80
#define CURRENTx2_180mA_Range1 25 & 0x80
#define CURRENTx2_187_5mA 26 & 0x80
#define CURRENTx2_195mA 27 & 0x80
#define CURRENTx2_202_5mA 28 & 0x80
#define CURRENTx2_210mA 29 & 0x80
#define CURRENTx2_217_5mA 30 & 0x80
#define CURRENTx2_225mA 31 & 0x80
#define CURRENTx2_232_5mA 32 & 0x80
#define CURRENTx2_240mA 33 & 0x80
#define CURRENTx2_247_5mA 34 & 0x80
#define CURRENTx2_255mA 35 & 0x80
#define CURRENTx2_262_5mA 36 & 0x80
#define CURRENTx2_270mA 37 & 0x80
#define CURRENTx2_277_5mA 38 & 0x80
#define CURRENTx2_285mA 39 & 0x80
#define CURRENTx2_292_5mA 40 & 0x80
#define CURRENTx2_300mA 41 & 0x80
#define CURRENTx2_307_5mA 42 & 0x80
#define CURRENTx2_315mA 43 & 0x80
#define CURRENTx2_322_5mA 44 & 0x80
#define CURRENTx2_330mA 45 & 0x80
#define CURRENTx2_337_5mA 46 & 0x80
#define CURRENTx2_345mA 47 & 0x80
#define CURRENTx2_352_5mA 48 & 0x80
#define CURRENTx2_360mA 49 & 0x80
#define CURRENTx2_367_5mA 50 & 0x80
#define CURRENTx2_375mA_Range1 51 & 0x80
#define CURRENTx2_375mA_Range2 64 & 0x80
#define CURRENTx2_362_5mA 65 & 0x80
#define CURRENTx2_375mA 66 & 0x80
#define CURRENTx2_387_5mA 67 & 0x80
#define CURRENTx2_400mA 68 & 0x80
#define CURRENTx2_412_5mA 69 & 0x80
#define CURRENTx2_425_5mA 70 & 0x80
#define CURRENTx2_437_5mA 71 & 0x80
#define CURRENTx2_450mA 72 & 0x80
#define CURRENTx2_462_5mA 73 & 0x80
#define CURRENTx2_475mA 74 & 0x80
#define CURRENTx2_487_5mA 75 & 0x80
#define CURRENTx2_500mA 76 & 0x80
#define CURRENTx2_512_5mA 77 & 0x80
#define CURRENTx2_525mA 78 & 0x80
#define CURRENTx2_537_5mA 79 & 0x80
#define CURRENTx2_550mA 80 & 0x80
#define CURRENTx2_562_5mA 81 & 0x80
#define CURRENTx2_575mA 82 & 0x80
#define CURRENTx2_587_5mA 83 & 0x80
#define CURRENTx2_600mA_Range2 84 & 0x80
#define CURRENTx2_600mA_Range3 88 & 0x80
#define CURRENTx2_625mA 89 & 0x80
#define CURRENTx2_650mA 90 & 0x80
#define CURRENTx2_675mA 91 & 0x80
#define CURRENTx2_700mA 92 & 0x80
#define CURRENTx2_725mA 93 & 0x80
#define CURRENTx2_750mA 94 & 0x80
#define CURRENTx2_775mA 95 & 0x80
#define CURRENTx2_800mA 96 & 0x80
#define CURRENTx2_825mA 97 & 0x80
#define CURRENTx2_850mA 98 & 0x80
#define CURRENTx2_875mA 99 & 0x80
#define CURRENTx2_900mA 100 & 0x80
#define CURRENTx2_925mA 101 & 0x80
#define CURRENTx2_950mA 102 & 0x80
#define CURRENTx2_975mA 103 & 0x80
#define CURRENTx2_1000mA 104 & 0x80
#define CURRENTx2_1025mA 105 & 0x80
#define CURRENTx2_1050mA 106 & 0x80
#define CURRENTx2_1075mA 107 & 0x80
#define CURRENTx2_1100mA 108 & 0x80
#define CURRENTx2_1125mA 109 & 0x80
#define CURRENTx2_1150mA 110 & 0x80
#define CURRENTx2_1175mA 111 & 0x80
#define CURRENTx2_1200mA 112 & 0x80
#define CURRENTx2_1225mA 113 & 0x80
#define CURRENTx2_1250mA 114 & 0x80

// Drive Options
#define POSITIVE_POLARITY_WAVEFORM 0
#define NEGATIVE_POLARITY_WAVEFORM 1
#define SWITCHED_TO_GROUND 8
#define SWITCHED_TO_VDS 9

#define SINE_WAVE 0
#define SQUARE_WAVE 4

#define PATH0 0
#define PATH1 2

// REGISTERS
//  ATA5293 Register name definitions
//  -                0
#define R0 0x00
#define R1 0x01
#define R2 0x02
#define R3 0x03
#define R4 0x04
#define R5 0x05
#define R6 0x06
#define R7 0x07
#define SQSTR 0x08
#define SQPCA 0x09
#define SQPCB 0x0A
#define SQCTRL 0x0B
#define SQSTATE 0x0C
#define SQBFS 0x0D
#define SQFAULT 0x0E
#define SQLP 0x0F
#define SQTO 0x10
#define SQECS0 0x11
#define SQECS1 0x12
#define SQECM0 0x13
#define SQECM1 0x14
#define SQICS0 0x15
#define SQICS1 0x16
#define SQICS2 0x17
#define SQICM0 0x18
#define SQICM1 0x19
#define SQICM2 0x1A
#define SQWTC 0x1B
#define SQWTP 0x1C
#define SQBPA 0x1D
#define SQBPB 0x1E
#define SQAPA 0x1F
#define SQAPB 0x20
#define SQIMD 0x21
#define SQIMA 0x22
#define SQIMB 0x23
#define SQSP 0x24
#define SQCAS 0x25
#define SQCBS 0x26
#define SQDSP 0x27
#define SQDSD 0x28
#define SQLBCT 0x29
#define SQLBL0 0x2A
#define SQLBL1 0x2B
#define SQLBL2 0x2C
#define SQLBL3 0x2D
#define SQMD 0x2E
#define SQMWP 0x2F
#define SQMFL 0x30
#define SQMFC 0x31
#define SQSD 0x32
#define SQSWP 0x33
#define STR 0x34
#define STRM 0x35
#define FAR 0x36
#define FARM 0x37
#define STCR 0x38
#define AFADR 0x39
#define PWSUPE 0x3A
#define PWSUPS 0x3B
#define PWSUPC 0x3C
#define PWBST 0x3D
#define PWBC 0x3E
#define PWFAULT 0x3F
#define PWADCH 0x40
#define PWADCM 0x41
#define PWADCC 0x42
#define PWDACH 0x43
#define PWDACL 0x44
#define PWVSH 0x45
#define PWVDSH 0x46
#define PWVI0H 0x47
#define PWVI1H 0x48
#define PWVML 0x49
#define PWACA 0x4A
#define PWACAC 0x4B
#define PWVDST0H 0x4C
#define PWVDST1H 0x4D
#define PWVPP0H 0x4E
#define PWVPP1H 0x4F
#define PWTVL 0x50
#define PWVDSIR0H 0x51
#define PWVDSIR1H 0x52
#define PWVDSIRL 0x53
#define PWAPH0 0x54
#define PWASF0 0x55
#define PWAPH1 0x56
#define PWASF1 0x57
#define PWACDT0 0x58
#define PWACDT1 0x59
#define PWVSLT 0x5A
#define PWZCC0 0x5B
#define PWZCC1 0x5C
#define PWVIC0 0x5D
#define PWVIC1 0x5E
#define PWVDSI0H 0x5F
#define PWVDSI1H 0x60
#define PWVDSIL 0x61
#define PWCMS0 0x62
#define PWCMS1 0x63
#define PWPPCC0 0x64
#define PWPPCC1 0x65
#define PWBAMT 0x66
#define PWBAM 0x67
#define DRCFG 0x68
#define DRC0 0x69
#define DRC1 0x6A
#define DRC2 0x6B
#define DRC3 0x6C
#define DRC4 0x6D
#define DRSELL 0x6E
#define DRSELH 0x6F
#define TXMOD 0x70
#define TXENC 0x71
#define TXBSG 0x72
#define TXBWG 0x73
#define TXBN0 0x74
#define TXBN1 0x75
#define TXAMP0 0x76
#define TXAMP1 0x77
#define TXPHD1 0x78
#define TXSQC0 0x79
#define TXSQC1 0x7A
#define TXCWA 0x7B
#define TXCWD 0x7C
#define TXSR0 0x7D
#define TXSCS0 0x7E
#define TXSR1 0x7F
#define TXSCS1 0x80
#define TXRDC 0x81
#define TXRDCU 0x82
#define TXRDSC 0x83
#define JMDAT 0x84
#define JMDC 0x85
#define JMSQ 0x86
#define JMAN1 0x87
#define JMAN2 0x88
#define JMAN3 0x89
#define JMANH 0x8A
#define DRSTAT 0x8B
#define DROTF 0x8C
#define DRLOCF 0x8D
#define DRHOCF 0x8E
#define DRB3F 0x8F
#define DRSOL 0x90
#define DRSOH 0x91
#define DRSIL 0x92
#define DRSIH 0x93
#define DRDCML 0x94
#define DRDCMH 0x95
#define DRDMOL 0x96
#define DRDMOH 0x97
#define DRTML 0x98
#define DRTMH 0x99
#define IMEN 0x9A
#define IMFE 0x9B

#define IMRFIL 0x9E
#define IMRXC 0x9F
#define FDCOMP 0xA0
#define FDDCR 0xA1
#define FDRMA 0xA2
#define PAGCC 0xA3
#define PDEMC 0xA4
#define PDLPHC 0xA5
#define PDLAMC 0xA6
#define PDFEMC 0xA7
#define PDFEAC 0xA8
#define PDFETC 0xA9
#define PDFSET 0xAA
#define RBCTL 0xAB
#define RBCFG 0xAC
#define RXFFL 0xAD
#define RBFS 0xAE
#define RBFD 0xAF
#define RBSRD 0xB0
#define RBSRS 0xB1
#define DHEN 0xB2
#define DHFTSEN 0xB3
#define DHCT 0xB4
#define DHOC 0xB5
#define DHPS 0xB6
#define DHEDG 0xB7
#define DHDBC 0xB8
#define DHGRPS 0xB9
#define DHFCTL 0xBA
#define DHFCFG 0xBB
#define DHFFL 0xBC
#define DHFD 0xBD
#define DHFAULT 0xBE
#define DHES 0xBF
#define DHEM 0xC0
#define DHDCFG 0xC1
#define DHDVO 0xC2
#define DHIVR 0xC3
#define DHHYE 0xC4
#define DHDPD 0xC5
#define GDS0 0xC6
#define GDS1 0xC7
#define IRQDS 0xC8
#define CODS 0xC9
#define PORT_REG 0xCA
#define DDR 0xCB
#define CLKEN 0xCC
#define CLKCSR 0xCD

#define FUCFG 0xD8
#define PLYFA 0xD9
#define PLYFD 0xDA
#define GPR0 0xDB
#define GPR1 0xDC
#define GPR2 0xDD
#define GPR3 0xDE
#define GPR4 0xDF
#define GPR5 0xE0
#define GPR6 0xE1
#define GPR7 0xE2
#define GPR8 0xE3
#define GPR9 0xE4
#define GPR10 0xE5
#define GPR11 0xE6
#define GPR12 0xE7
#define GPR13 0xE8
#define GPR14 0xE9
#define GPR15 0xEA

#define PDESC 0xEB
#define IMRSBB 0xEC
#define T8PRES 0xED
#define T8CNT 0xEE
#define DRACFG 0xF3
#define DRRAMP 0xF4
#define PWDPHC0 0xF5
#define PWDPHC1 0xF6
#define PWACFG  0xF7

// Bit definition
#define ENFRC 0
#define ENICLK 1
#define ENLCO 2
#define ENXTO 3
#define ENPLP 4
#define ENPLL 5
#define DISFRC 6
#define DISMRC 7

// ROM function address table
#define ATA_romFuncTX2RHDX_ASM_adr 0x011a
#define ATA_romFuncRHDX2TX_ASM_adr 0x011c
#define ATA_romFuncMeasADC_ASM_adr 0x011e
#define ATA_romFuncShortToGnd_ASM_adr 0x0120
#define ATA_romFuncShortToVBatt_ASM_adr 0x0122
#define ATA_romFuncCalcPercentage_ASM_adr 0x0124
#define ATA_romFuncCompensationLoop_ASM_adr 0x0126
#define ATA_romFuncShortToPinToPin_ASM_adr 0x0128
#define ATA_romFuncTX2RHDXprepare_ASM_adr 0x012A
#define ATA_romFuncDischgAnaAdcBus_ASM_adr 0x012C
#define ATA_romFuncRegSettingV11comp_ASM_adr 0x0132
#define ATA_romFuncRegSettingV30_ASM_adr 0x0135

// GPIO
#define GPIO0 0
#define GPIO1 1
#define GPIO2 2
#define GPIO3 3

// TXMOD parameter
#define Cordic (0 << 5)
#define ROM_Gauss (1 << 5)
#define ROM_Sinus (2 << 5)
#define CustomWave (3 << 5)

#define f_140_63 0
#define f_138_46 1
#define f_126_76 7
#define f_125 8
#define f_123_29 9
#define f_113_92 15

// TXENC parameter
#define TX_NRZ (0)
#define TX_Manchester (1 << 5)
#define TX_BPLM (2 << 5)

#define TX_M4kBd 15
#define TX_M2kBd 31

// Door Handle defines
// ************************
// Door Handle Data Current Threshold
#define DH_DCT_5_22mA 0x00
#define DH_DCT_5_65mA 0x01
#define DH_DCT_6_09mA 0x02
#define DH_DCT_6_62mA 0x03
#define DH_DCT_6_96mA 0x04
#define DH_DCT_7_39mA 0x05
#define DH_DCT_7_83mA 0x06
#define DH_DCT_8_26mA 0x07
#define DH_DCT_8_70mA 0x08
#define DH_DCT_9_13mA 0x09
#define DH_DCT_9_57mA 0x0A
#define DH_DCT_10_00mA 0x0B
#define DH_DCT_10_43mA 0x0C
#define DH_DCT_10_87mA 0x0D
#define DH_DCT_11_30mA 0x0E
#define DH_DCT_11_74mA 0x0F

// Door Handle Enable Delay
#define DH_END_0_25ms 0x00
#define DH_END_2ms (0x01 << 4)
#define DH_END_4ms (0x02 << 4)
#define DH_END_8ms (0x03 << 4)

// Door Handle Over Current Limit
#define DHOCL0_30mA 0x00
#define DHOCL0_40mA 0x01
#define DHOCL0_50mA 0x02
#define DHOCL0_60mA 0x03
#define DHOCL1_30mA (0x00 << 2)
#define DHOCL1_40mA (0x01 << 2)
#define DHOCL1_50mA (0x02 << 2)
#define DHOCL1_60mA (0x03 << 2)

/*PROTOTYPES*/
extern void copyData(uint8_t *, uint8_t *, uint8_t);
extern void lf_ata5293_GDA(void);
extern void lf_ata5293_GID(void);
extern void lf_ata5293_GSLP(void);
extern void lf_ata5293_GTX(void);
extern void lf_ata5293_GRH(void);
extern void lf_ata5293_GFDX(void);
extern void lf_ata5293_SBI(uint8_t bitToSet, uint8_t address);
extern void lf_ata5293_CBI(uint8_t bitToClear, uint8_t address);
extern void lf_ata5293_LSLI(uint8_t shiftBy, uint8_t address);
extern void lf_ata5293_LSRI(uint8_t shiftBy, uint8_t address);
extern void lf_ata5293_AND(uint8_t destReg, uint8_t valueReg);
extern void lf_ata5293_ANDI(uint8_t destReg, uint8_t value);
extern void lf_ata5293_OR(uint8_t destReg, uint8_t valueReg);
extern void lf_ata5293_ORI(uint8_t destReg, uint8_t value);
extern void lf_ata5293_ADD(uint8_t destReg, uint8_t valueReg);
extern void lf_ata5293_ADDI(uint8_t destReg, uint8_t value);
extern void lf_ata5293_SUB(uint8_t destReg, uint8_t valueReg);
extern void lf_ata5293_SUBI(uint8_t destReg, uint8_t value);
extern void lf_ata5293_CMP(uint8_t Rd, uint8_t address);
extern void lf_ata5293_NOT(uint8_t address);
extern void lf_ata5293_WRI(uint8_t address, uint8_t data);
extern void lf_ata5293_WRB(uint8_t address, uint8_t length, uint8_t *data);
extern void lf_ata5293_WRAR(uint8_t address, uint8_t length, uint8_t *data);
extern void lf_ata5293_SGP(uint8_t pin, uint8_t level);
extern void lf_ata5293_LD(uint8_t reg, uint8_t address);
extern void lf_ata5293_ST(uint8_t reg, uint8_t address);
extern void lf_ata5293_PUSH(uint8_t address);
extern void lf_ata5293_POP(uint8_t address);
extern void lf_ata5293_SKIF(uint8_t flags);
extern void lf_ata5293_SKINF(uint8_t flags);
extern void lf_ata5293_CALLA(uint16_t address);
extern void lf_ata5293_JMP(uint16_t address);
extern void lf_ata5293_CALL(uint8_t label);
extern void lf_ata5293_WAIT(uint8_t prescaler, uint8_t counter);
extern void lf_ata5293_JMPL(uint8_t label);
extern void lf_ata5293_DBNZ(uint8_t label);
extern void lf_ata5293_CONFC(uint8_t reg, uint8_t mask);
extern void lf_ata5293_CLRC(void);
extern void lf_ata5293_WAITC(uint8_t value, uint8_t flag);
extern void lf_ata5293_RET(void);
extern void lf_ata5293_JMPP(void);
extern void lf_ata5293_TOUT(uint8_t t);
extern void lf_ata5293_PTX(uint8_t pauseDuration);
extern void lf_ata5293_FDXCC(uint8_t enable);
extern void lf_ata5293_SC(uint8_t carrierDuration);
extern void lf_ata5293_SDR(uint16_t drivers, uint8_t duration);
extern void lf_ata5293_SAP(void);
extern void lf_ata5293_SCC(uint8_t coilCurrent);
extern void lf_ata5293_SD(uint16_t bitsToSend, uint8_t *data);
extern void lf_ata5293_IRO(uint8_t enable);
extern void lf_ata5293_CLR(uint8_t bufferSelection);
extern void lf_ata5293_SSC(uint8_t spiSetting);
extern void lf_ata5293_GSI(uint8_t *returnedData);
extern void lf_ata5293_RFS(void);
extern void lf_ata5293_SWRI(uint8_t address, uint8_t data);
extern void lf_ata5293_SWRB(uint8_t address, uint8_t, uint8_t *data);
extern void lf_ata5293_SORI(uint8_t address, uint8_t value);
extern void lf_ata5293_SANDI(uint8_t address, uint8_t value);
extern void lf_ata5293_SRD(uint8_t address, uint8_t *returnedData);
extern void lf_ata5293_SLBL(uint8_t labelSource, uint8_t labelRegister);
extern void lf_ata5293_SRB(uint8_t address, uint8_t length, uint8_t *returnedData);
extern void lf_ata5293_HALT(void);
extern void lf_ata5293_RUN(void);
extern void lf_ata5293_CRX(void);
extern void lf_ata5293_CDH(void);
extern void lf_ata5293_NOP(void);
extern uint8_t checkStatus(void);
extern void DriverOutput(uint16_t driver, uint8_t driverConfiguration, uint8_t waveFormType, uint8_t path);
extern void lf_SelectCoilCurrent(uint8_t coilcurrent, uint16_t driver, uint8_t path);
extern uint8_t get_current_setting(uint16_t current);
unsigned char *Manchester2spi(unsigned char manchesterByte, unsigned char *bufferPtr);
#endif /* FORTISSIMO_H_ */

/* =============================================  EOF  ============================================== */
