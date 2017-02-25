#ifndef NRF24L01_H_
#define NRF24L01_H_

#include "stm32f1xx_hal.h"
#include "spi.h"

//Define some constans for nRF24L01PP
#define nRF_SPI_TIMEOUT							1000	// milliseconds

//Define the commands for operate the nRF24L01+
//For Commands table, take a look at nRF24L01+ Product Specification Page 51.
#define nRF_CMD_R_REGISTER					0x00  // Command for read register
#define nRF_CMD_W_REGISTER					0x20 	// Command for write register
#define nRF_CMD_R_RX_PAYLOAD				0x61  // Command for read RX payload
#define nRF_CMD_W_TX_PAYLOAD				0xA0  // Command for write TX payload
#define nRF_CMD_FLUSH_TX						0xE1 	// Command for flush TX FIFO
#define nRF_CMD_FLUSH_RX						0xE2  // Command for flush RX FIFO
#define nRF_CMD_REUSE_TX_PL					0xE3  // Command for reuse TX payload
#define nRF_CMD_R_RX_PL_WID					0x60	// Command for read RX payload width
#define nRF_CMD_W_ACK_PAYLOAD				0xA8	// Command for write payload to be transmitted with ACK packet
#define nRF_CMD_W_TX_PAYLOAD_NOACK	0xB0	// Command for disable AUTOACK on a specific packet
#define NOP													0xFF  // No operation, might be used to read the STATUS register

//Define the register address for nRF24L01PP
#define nRF_CONFIG									0x00  // Configurate the status of transceiver, mode of CRC and the replay of transceiver status
#define nRF_EN_AA										0x01  // Enable the atuo-ack in all channels
#define nRF_EN_RXADDR								0x02  // Enable Rx Address
#define nRF_SETUP_AW								0x03  // Configurate the address width
#define nRF_SETUP_RETR							0x04  // Setup the retransmit
#define nRF_RF_CH										0x05  // Configurate the RF frequency
#define nRF_RF_SETUP								0x06  // Setup the rate of data, and transmit power
#define nRF_STATUS									0x07  //
#define nRF_OBSERVE_TX							0x08  //
#define nRF_CD											0x09  // Carrier detect
#define nRF_RX_ADDR_P0							0x0A  // Receive address of channel 0
#define nRF_RX_ADDR_P1							0x0B  // Receive address of channel 1
#define nRF_RX_ADDR_P2							0x0C  // Receive address of channel 2
#define nRF_RX_ADDR_P3							0x0D  // Receive address of channel 3
#define nRF_RX_ADDR_P4							0x0E  // Receive address of channel 4
#define nRF_RX_ADDR_P5							0x0F  // Receive address of channel 5
#define nRF_TX_ADDR									0x10  //       Transmit address
#define nRF_RX_PW_P0								0x11  // Size of receive data in channel 0
#define nRF_RX_PW_P1								0x12  // Size of receive data in channel 1
#define nRF_RX_PW_P2								0x13  // Size of receive data in channel 2
#define nRF_RX_PW_P3								0x14  // Size of receive data in channel 3
#define nRF_RX_PW_P4								0x15  // Size of receive data in channel 4
#define nRF_RX_PW_P5								0x16  // Size of receive data in channel 5
#define nRF_FIFO_STATUS							0x17  // FIFO Status
#define nRF_DYNPD										0x1C  // Dynamic Payload State for pipes
#define nRF_FEATURE									0x1D  // FEATURE Register

typedef enum{
	nRF_DATA_PIPE_0 = 0,
	nRF_DATA_PIPE_1 = 1,
	nRF_DATA_PIPE_2 = 2,
	nRF_DATA_PIPE_3 = 3,
	nRF_DATA_PIPE_4 = 4,
	nRF_DATA_PIPE_5 = 5
} nRF_DATA_PIPE;

typedef enum{
	nRF_RETX_DELAY_250uS	= 0,
	nRF_RETX_DELAY_500uS	= 1,
	nRF_RETX_DELAY_750uS	= 2,
	nRF_RETX_DELAY_1000uS	= 3,
	nRF_RETX_DELAY_1250uS	= 4,
	nRF_RETX_DELAY_1500uS	= 5,
	nRF_RETX_DELAY_1750uS	= 6,
	nRF_RETX_DELAY_2000uS	= 7,
	nRF_RETX_DELAY_2250uS	= 8,
	nRF_RETX_DELAY_2500uS	= 9,
	nRF_RETX_DELAY_2750uS	= 10,
	nRF_RETX_DELAY_3000uS	= 11,
	nRF_RETX_DELAY_3250uS	= 12,
	nRF_RETX_DELAY_3500uS	= 13,
	nRF_RETX_DELAY_3750uS	= 14,
	nRF_RETX_DELAY_4000uS	= 15
} nRF_RETX_DELAY;

typedef enum{
	nRF_RETX_DISABLED			= 0,
	nRF_RETX_COUNT_1			= 1,
	nRF_RETX_COUNT_2			= 2,
	nRF_RETX_COUNT_3			= 3,
	nRF_RETX_COUNT_4			= 4,
	nRF_RETX_COUNT_5			= 5,
	nRF_RETX_COUNT_6			= 6,
	nRF_RETX_COUNT_7			= 7,
	nRF_RETX_COUNT_8			= 8,
	nRF_RETX_COUNT_9			= 9,
	nRF_RETX_COUNT_10			= 10,
	nRF_RETX_COUNT_11			= 11,
	nRF_RETX_COUNT_12			= 12,
	nRF_RETX_COUNT_13			= 13,
	nRF_RETX_COUNT_14			= 14,
	nRF_RETX_COUNT_15			= 15
} nRF_RETX_COUNT;

typedef enum{
	nRF_DATA_RATE_250KBPS	=	1,
	nRF_DATA_RATE_1MBPS		=	0,
	nRF_DATA_RATE_2MBPS		=	2
} nRF_DATA_RATE;

typedef enum{
	nRF_TX_PWR_M18dBm	=	0,
	nRF_TX_PWR_M12dBm	=	1,
	nRF_TX_PWR_M6dBm	=	2,
	nRF_TX_PWR_0dBm		=	3
} nRF_TX_PWR;

typedef enum{
	nRF_ADDR_WIDTH_3	=	1,
	nRF_ADDR_WIDTH_4	=	2,
	nRF_ADDR_WIDTH_5	=	3
} nRF_ADDR_WIDTH;

typedef enum{
	nRF_CRC_WIDTH_BYTE			=	0,
	nRF_CRC_WIDTH_HALFWORD	=	1
} nRF_CRC_WIDTH;

typedef enum{
	nRF_STATE_TX	=	0,
	nRF_STATE_RX	=	1
} nRF_TXRX_STATE;

typedef enum{
	nRF_DISABLE	=	0,
	nRF_ENABLE	=	1
} nRF_STATE;

typedef enum{
	nRF_RXPW_1BYTE		= 1,
	nRF_RXPW_2BYTES		= 2,
	nRF_RXPW_3BYTES		= 3,
	nRF_RXPW_4BYTES		= 4,
	nRF_RXPW_5BYTES		= 5,
	nRF_RXPW_6BYTES		= 6,
	nRF_RXPW_7BYTES		= 7,
	nRF_RXPW_8BYTES		= 8,
	nRF_RXPW_9BYTES		= 9,
	nRF_RXPW_10BYTES	= 10,
	nRF_RXPW_11BYTES	= 11,
	nRF_RXPW_12BYTES	= 12,
	nRF_RXPW_13BYTES	= 13,
	nRF_RXPW_14BYTES	= 14,
	nRF_RXPW_15BYTES	= 15,
	nRF_RXPW_16BYTES	= 16,
	nRF_RXPW_17BYTES	= 17,
	nRF_RXPW_18BYTES	= 18,
	nRF_RXPW_19BYTES	= 19,
	nRF_RXPW_20BYTES	= 20,
	nRF_RXPW_21BYTES	= 21,
	nRF_RXPW_22BYTES	= 22,
	nRF_RXPW_23BYTES	= 23,
	nRF_RXPW_24BYTES	= 24,
	nRF_RXPW_25BYTES	= 25,
	nRF_RXPW_26BYTES	= 26,
	nRF_RXPW_27BYTES	= 27,
	nRF_RXPW_28BYTES	= 28,
	nRF_RXPW_29BYTES	= 29,
	nRF_RXPW_30BYTES	= 30,
	nRF_RXPW_31BYTES	= 31,
	nRF_RXPW_32BYTES	= 32
} nRF_PRX_WIDTH;

typedef struct{
	SPI_HandleTypeDef		*hspi;
	nRF_CRC_WIDTH				CRC_Width;
	nRF_ADDR_WIDTH			ADDR_Width;
	nRF_DATA_RATE 			Data_Rate;
	nRF_TX_PWR					TX_Power;
	nRF_TXRX_STATE			State;
	
	uint8_t							RF_Channel;
	nRF_PRX_WIDTH				PayloadWidth;
	nRF_RETX_COUNT			RetransmitCount;
	nRF_RETX_DELAY			RetransmitDelay;

	uint8_t*						RX_Address;
	uint8_t*						TX_Address;

	uint8_t							Busy;

	uint8_t*						RX_Buffer;
	uint8_t*						TX_Buffer;
	
	GPIO_TypeDef*				nRF_nSS_GPIO_PORT;
	uint16_t						nRF_nSS_GPIO_PIN;

	GPIO_TypeDef*				nRF_CE_GPIO_PORT;
	uint16_t						nRF_CE_GPIO_PIN;
} nRF24L01P;

/* ---- Function    Prototypes ---- */
HAL_StatusTypeDef HAL_nRF24L01P_ReadRegister(nRF24L01P *nRF, uint8_t regAddr, uint8_t *pRegData);
HAL_StatusTypeDef HAL_nRF24L01P_WriteRegister(nRF24L01P *nRF, uint8_t regAddr, uint8_t *pRegData);
HAL_StatusTypeDef HAL_nRF24L01P_ReadRXPayload(nRF24L01P *nRF, uint8_t *pRegData);
HAL_StatusTypeDef HAL_nRF24L01P_WriteTXPayload(nRF24L01P *nRF, uint8_t *pRegData);
HAL_StatusTypeDef HAL_nRF24L01P_WriteTXPayloadACK(nRF24L01P *nRF, uint8_t *pRegData, nRF_DATA_PIPE Pipe);
HAL_StatusTypeDef HAL_nRF24L01P_FlushRX(nRF24L01P *nRF);
HAL_StatusTypeDef HAL_nRF24L01P_FlushTX(nRF24L01P *nRF);
HAL_StatusTypeDef HAL_nRF24L01P_SetDataRate(nRF24L01P *nRF, nRF_DATA_RATE Rate);
HAL_StatusTypeDef HAL_nRF24L01P_SetTXPower(nRF24L01P *nRF, nRF_TX_PWR Power);
HAL_StatusTypeDef HAL_nRF24L01P_SetContinuousWave(nRF24L01P *nRF, nRF_STATE ContWave);
HAL_StatusTypeDef HAL_nRF24L01P_ClearInterrupts(nRF24L01P *nRF);
HAL_StatusTypeDef HAL_nRF24L01P_SetRFChannel(nRF24L01P *nRF, uint8_t Channel);
HAL_StatusTypeDef HAL_nRF24L01P_SetRetransmissionCount(nRF24L01P *nRF, nRF_RETX_COUNT Count);
HAL_StatusTypeDef HAL_nRF24L01P_SetRetransmissionDelay(nRF24L01P *nRF, nRF_RETX_DELAY Delay);
HAL_StatusTypeDef HAL_nRF24L01P_SetAddressWidth(nRF24L01P *nRF, nRF_ADDR_WIDTH AddrWidth);
HAL_StatusTypeDef HAL_nRF24L01P_RXPipe(nRF24L01P *nRF, nRF_DATA_PIPE Pipe, nRF_STATE Pipe_State);
HAL_StatusTypeDef HAL_nRF24L01P_AutoACK(nRF24L01P *nRF, nRF_DATA_PIPE Pipe, nRF_STATE ACK_State);
HAL_StatusTypeDef HAL_nRF24L01P_CRC(nRF24L01P *nRF, nRF_STATE CRC_State);
HAL_StatusTypeDef HAL_nRF24L01P_SetCRCWidth(nRF24L01P *nRF, nRF_CRC_WIDTH CRC_Width);
HAL_StatusTypeDef HAL_nRF24L01P_PowerUP(nRF24L01P *nRF, nRF_STATE Power);
HAL_StatusTypeDef HAL_nRF24L01P_TXRX(nRF24L01P *nRF, nRF_TXRX_STATE TxRx_State);
HAL_StatusTypeDef HAL_nRF24L01P_RXDataReadyIRQ(nRF24L01P *nRF, nRF_STATE IRQ_State);
HAL_StatusTypeDef HAL_nRF24L01P_TXDataSentIRQ(nRF24L01P *nRF, nRF_STATE IRQ_State);
HAL_StatusTypeDef HAL_nRF24L01P_MaxReTransmitIRQ(nRF24L01P *nRF, nRF_STATE IRQ_State);
HAL_StatusTypeDef HAL_nRF24L01P_SetPRXAddress(nRF24L01P *nRF, uint8_t *pRegData, nRF_DATA_PIPE Pipe);
HAL_StatusTypeDef HAL_nRF24L01P_SetPTXAddress(nRF24L01P *nRF, uint8_t *pRegData);
HAL_StatusTypeDef HAL_nRF24L01P_SetPRXWidth(nRF24L01P *nRF, nRF_PRX_WIDTH Width, nRF_DATA_PIPE Pipe);
HAL_StatusTypeDef HAL_nRF24L01P_DynACK(nRF24L01P *nRF, nRF_STATE ACK_State);
HAL_StatusTypeDef HAL_nRF24L01P_ACKPayload(nRF24L01P *nRF, nRF_STATE ACK_State);
HAL_StatusTypeDef HAL_nRF24L01P_DynPayload(nRF24L01P *nRF, nRF_STATE DPL_State);
HAL_StatusTypeDef HAL_nRF24L01P_DPLPipe(nRF24L01P *nRF, nRF_DATA_PIPE Pipe, nRF_STATE DPL_State);

HAL_StatusTypeDef HAL_nRF24L01P_TransmitPacket(nRF24L01P *nRF, uint8_t *Data);
HAL_StatusTypeDef HAL_nRF24L01P_TransmitPacketACK(nRF24L01P *nRF, uint8_t *Data, nRF_DATA_PIPE Pipe);
HAL_StatusTypeDef HAL_nRF24L01P_ReceivePacket(nRF24L01P *nRF, uint8_t *Data);
HAL_StatusTypeDef HAL_nRF24L01P_IRQ_Handler(nRF24L01P *nRF);
HAL_StatusTypeDef HAL_nRF24L01P_Init(nRF24L01P *nRF);

HAL_StatusTypeDef HAL_nRF24L01P_SendCommand(nRF24L01P *nRF, uint8_t Command, uint8_t *TxBuffer, uint8_t *RxBuffer, uint8_t Size);
/* ---- LL Function Prototypes ---- */
void HAL_nRF24L01P_nSS_High(nRF24L01P *nRF);
void HAL_nRF24L01P_nSS_Low(nRF24L01P *nRF);
void HAL_nRF24L01P_CE_High(nRF24L01P *nRF);
void HAL_nRF24L01P_CE_Low(nRF24L01P *nRF);
#endif
