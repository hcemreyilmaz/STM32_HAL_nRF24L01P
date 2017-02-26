#include "nRF24L01P.h"
HAL_StatusTypeDef HAL_nRF24L01P_Init(nRF24L01P *nRF)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	HAL_StatusTypeDef retValue = HAL_OK;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_PowerUP(nRF, nRF_ENABLE) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	do {
			if(HAL_nRF24L01P_ReadRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
			{
				return HAL_ERROR;
			}
	} while((regValue & 0x02) == 0x00); // Did device powered up? 
	
	/* ---- InitProcess ---- */
	retValue |= HAL_nRF24L01P_SetPRXWidth(nRF, nRF->PayloadWidth, nRF_DATA_PIPE_0);
	retValue |= HAL_nRF24L01P_SetPRXAddress(nRF, nRF->RX_Address, nRF_DATA_PIPE_0);
	retValue |= HAL_nRF24L01P_SetPTXAddress(nRF, nRF->TX_Address);
	retValue |= HAL_nRF24L01P_RXDataReadyIRQ(nRF, nRF_ENABLE);
	retValue |= HAL_nRF24L01P_TXDataSentIRQ(nRF, nRF_ENABLE);
	retValue |= HAL_nRF24L01P_MaxReTransmitIRQ(nRF, nRF_ENABLE);
	retValue |= HAL_nRF24L01P_CRC(nRF, nRF_ENABLE);
	retValue |= HAL_nRF24L01P_SetCRCWidth(nRF, nRF->CRC_Width);
	retValue |= HAL_nRF24L01P_SetAddressWidth(nRF, nRF->ADDR_Width);
	retValue |= HAL_nRF24L01P_SetRFChannel(nRF, nRF->RF_Channel);
	retValue |= HAL_nRF24L01P_SetDataRate(nRF, nRF->Data_Rate);
	retValue |= HAL_nRF24L01P_SetRetransmissionCount(nRF, nRF->RetransmitCount);
	retValue |= HAL_nRF24L01P_SetRetransmissionDelay(nRF, nRF->RetransmitDelay);
	
	retValue |= HAL_nRF24L01P_DynACK(nRF, nRF_ENABLE);
	retValue |= HAL_nRF24L01P_ACKPayload(nRF, nRF_ENABLE);
	retValue |= HAL_nRF24L01P_DynPayload(nRF, nRF_ENABLE);
	
	retValue |= HAL_nRF24L01P_RXPipe(nRF, nRF_DATA_PIPE_0, nRF_ENABLE);
	retValue |= HAL_nRF24L01P_DPLPipe(nRF, nRF_DATA_PIPE_0, nRF_ENABLE);
	
	retValue |= HAL_nRF24L01P_AutoACK(nRF, nRF_DATA_PIPE_0, nRF_ENABLE);
	retValue |= HAL_nRF24L01P_AutoACK(nRF, nRF_DATA_PIPE_1, nRF_DISABLE);
	retValue |= HAL_nRF24L01P_AutoACK(nRF, nRF_DATA_PIPE_2, nRF_DISABLE);
	retValue |= HAL_nRF24L01P_AutoACK(nRF, nRF_DATA_PIPE_3, nRF_DISABLE);
	retValue |= HAL_nRF24L01P_AutoACK(nRF, nRF_DATA_PIPE_4, nRF_DISABLE);
	retValue |= HAL_nRF24L01P_AutoACK(nRF, nRF_DATA_PIPE_5, nRF_DISABLE);
	
	retValue |= HAL_nRF24L01P_ClearInterrupts(nRF);
	
	retValue |= HAL_nRF24L01P_TXRX(nRF, nRF_STATE_RX);
	retValue |= HAL_nRF24L01P_FlushRX(nRF);
	
	if(retValue != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_IRQ_Handler(nRF24L01P *nRF)
{
	/* ---- Local Vars. ---- */
	uint8_t regStatus;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_STATUS, &regStatus) != HAL_OK)
	{
		return HAL_ERROR;
	}	
	/* ---- RX FIFO Int.---- */
	if((regStatus & (1 << 6)) != 0)
	{
		uint8_t regFIFO_Status;
		HAL_nRF24L01P_CE_Low(nRF);
		do {
		HAL_nRF24L01P_ReadRXPayload(nRF, nRF->RX_Buffer);
		regStatus |= (1 << 6);
		HAL_nRF24L01P_WriteRegister(nRF, nRF_STATUS, &regStatus);
		HAL_nRF24L01P_ReadRegister(nRF, nRF_FIFO_STATUS, &regFIFO_Status);
		} while((regFIFO_Status & 0x01) == 0x00);
		HAL_nRF24L01P_CE_High(nRF);
	}
	/* ---- TX Sent Int.---- */
	if((regStatus & (1 << 5)) != 0)
	{
		HAL_nRF24L01P_CE_Low(nRF);
		regStatus |= (1 << 5);
		HAL_nRF24L01P_TXRX(nRF, nRF_STATE_RX);
		HAL_nRF24L01P_WriteRegister(nRF, nRF_STATUS, &regStatus);
		HAL_nRF24L01P_CE_High(nRF);
		nRF->Busy = 0;
	}
	/* ---- MAXReTX Int.---- */
	if((regStatus & (1 << 4)) != 0)
	{
		regStatus |= (1 << 4);
		
		HAL_nRF24L01P_FlushTX(nRF);
		HAL_nRF24L01P_PowerUP(nRF, nRF_DISABLE);	// bi kapatip açalim da düzelsin...
		HAL_nRF24L01P_PowerUP(nRF, nRF_ENABLE);
		
		HAL_nRF24L01P_CE_Low(nRF);
		HAL_nRF24L01P_TXRX(nRF, nRF_STATE_RX);
		HAL_nRF24L01P_WriteRegister(nRF, nRF_STATUS, &regStatus);
		HAL_nRF24L01P_CE_High(nRF);
		nRF->Busy = 0;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_ReceivePacket(nRF24L01P *nRF, uint8_t *Data)
{
	nRF->Busy = 1;
	
	HAL_nRF24L01P_CE_Low(nRF);
	HAL_nRF24L01P_TXRX(nRF, nRF_STATE_RX);
	HAL_nRF24L01P_CE_High(nRF);	

	while(nRF->Busy);	// TODO: Add *timeout* functionality
	
	for(uint8_t i = 0; i < nRF->PayloadWidth; i++)
	{
		Data[i] = nRF->RX_Buffer[i];
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_TransmitPacket(nRF24L01P *nRF, uint8_t *Data)
{
	nRF->Busy = 1;

	HAL_nRF24L01P_CE_Low(nRF);
	HAL_nRF24L01P_TXRX(nRF, nRF_STATE_TX);
	HAL_nRF24L01P_WriteTXPayload(nRF, Data);
	HAL_nRF24L01P_CE_High(nRF);

	while(nRF->Busy);	// TODO: Add *timeout* functionality
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_TransmitPacketACK(nRF24L01P *nRF, uint8_t *Data, nRF_DATA_PIPE Pipe)
{
	nRF->Busy = 1;

	HAL_nRF24L01P_CE_Low(nRF);
	HAL_nRF24L01P_TXRX(nRF, nRF_STATE_TX);
	HAL_nRF24L01P_WriteTXPayloadACK(nRF, Data, Pipe);
	HAL_nRF24L01P_CE_High(nRF);

	while(nRF->Busy);	// TODO: Add *timeout* functionality
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetPRXWidth(nRF24L01P *nRF, nRF_PRX_WIDTH Width, nRF_DATA_PIPE Pipe)
{
	/* ---- Local Vars. ---- */
	uint8_t regAddr, regData;
	regData = Width & 0x3F;
	/* ---- Pre Process ---- */
	switch(Pipe)
	{
		case 0: //P0
			regAddr = nRF_RX_PW_P0;
		break;
		case 1: //P1
			regAddr = nRF_RX_PW_P1;
		break;
		case 2: //P2
			regAddr = nRF_RX_PW_P2;
		break;
		case 3: //P3
			regAddr = nRF_RX_PW_P3;
		break;
		case 4: //P4
			regAddr = nRF_RX_PW_P4;
		break;
		case 5: //P5
			regAddr = nRF_RX_PW_P5;
		break;
		default:
			return HAL_ERROR;		
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, regAddr, &regData)  != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetPTXAddress(nRF24L01P *nRF, uint8_t *pRegData)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero[5];
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_W_REGISTER + nRF_TX_ADDR, pRegData, Zero, 5) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetPRXAddress(nRF24L01P *nRF, uint8_t *pRegData, nRF_DATA_PIPE Pipe)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero[5];
	uint8_t Size;
	uint8_t regAddr;
	/* ---- Pre Process ---- */
	switch(Pipe)
	{
		case 0: //P0
			Size = 5;
			regAddr = nRF_RX_ADDR_P0;
		break;
		case 1: //P1
			Size = 5;
			regAddr = nRF_RX_ADDR_P1;
		break;
		case 2: //P2
			Size = 1;
			regAddr = nRF_RX_ADDR_P2;
		break;
		case 3: //P3
			Size = 1;
			regAddr = nRF_RX_ADDR_P3;
		break;
		case 4: //P4
			Size = 1;
			regAddr = nRF_RX_ADDR_P4;
		break;
		case 5: //P5
			Size = 1;
			regAddr = nRF_RX_ADDR_P5;
		break;
		default:
			return HAL_ERROR;		
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_W_REGISTER + regAddr, pRegData, Zero, Size) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_DPLPipe(nRF24L01P *nRF, nRF_DATA_PIPE Pipe, nRF_STATE DPL_State)
{
		/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_DYNPD, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(DPL_State)
	{
		regValue |= (1 << Pipe);
	} else {
		regValue &= ~(1 << Pipe);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_DYNPD, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_DynPayload(nRF24L01P *nRF, nRF_STATE DPL_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_FEATURE, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(DPL_State)
	{
		regValue |= (1 << 2);
	} else {
		regValue &= ~(1 << 2);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_FEATURE, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_ACKPayload(nRF24L01P *nRF, nRF_STATE ACK_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_FEATURE, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(ACK_State)
	{
		regValue |= (1 << 1);
	} else {
		regValue &= ~(1 << 1);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_FEATURE, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_DynACK(nRF24L01P *nRF, nRF_STATE ACK_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_FEATURE, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(ACK_State)
	{
		regValue |= (1 << 0);
	} else {
		regValue &= ~(1 << 0);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_FEATURE, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_MaxReTransmitIRQ(nRF24L01P *nRF, nRF_STATE IRQ_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(IRQ_State)
	{
		regValue &= ~(1 << 4);
	} else {
		regValue |= (1 << 4);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_TXDataSentIRQ(nRF24L01P *nRF, nRF_STATE IRQ_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(IRQ_State)
	{
		regValue &= ~(1 << 5);
	} else {
		regValue |= (1 << 5);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_RXDataReadyIRQ(nRF24L01P *nRF, nRF_STATE IRQ_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(IRQ_State)
	{
		regValue &= ~(1 << 6);
	} else {
		regValue |= (1 << 6);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_TXRX(nRF24L01P *nRF, nRF_TXRX_STATE TxRx_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(TxRx_State)
	{
		regValue |= (1);
		nRF->State = nRF_STATE_TX;
	} else {
		regValue &= ~(1);
		nRF->State = nRF_STATE_RX;
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_PowerUP(nRF24L01P *nRF, nRF_STATE Power)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(Power)
	{
		regValue |= (1 << 1);
	} else {
		regValue &= ~(1 << 1);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetCRCWidth(nRF24L01P *nRF, nRF_CRC_WIDTH CRC_Width)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(CRC_Width)
	{
		regValue |= (1 << 2);
	} else {
		regValue &= ~(1 << 2);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_CRC(nRF24L01P *nRF, nRF_STATE CRC_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	if(CRC_State)
	{
		regValue |= (1 << 3);
	} else {
		regValue &= ~(1 << 3);
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_CONFIG, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_AutoACK(nRF24L01P *nRF, nRF_DATA_PIPE Pipe, nRF_STATE ACK_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_EN_AA, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	Pipe &= (0x3F); //nRF_EN_AA reg's 6. and 7. bits are Reserved
	if(ACK_State)
	{
		regValue |= (1 << Pipe); //nRF_EN_AA reg is Configured
	} else {
		regValue &= ~(1 << Pipe); //nRF_EN_AA reg is Configured		
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_EN_AA, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_RXPipe(nRF24L01P *nRF, nRF_DATA_PIPE Pipe, nRF_STATE Pipe_State)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_EN_RXADDR, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	Pipe &= (0x3F); //nRF_EN_RXADDR reg's 7. bit is Reserved
	if(Pipe_State)
	{
		regValue |= (1 << Pipe);
	} else {
		regValue &= ~(1 << Pipe);
	} //nRF_EN_RXADDR reg is Configured
	
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_EN_RXADDR, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetAddressWidth(nRF24L01P *nRF, nRF_ADDR_WIDTH AddrWidth)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_SETUP_AW, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	AddrWidth &= (0x03);
	regValue &= (0xFC); //nRF_SETUP_AW reg's 0, 1 and 2. bits are Cleared
	regValue |= (AddrWidth); //nRF_SETUP_AW reg's 0, 1 and 2. bits are Configured
	
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_SETUP_AW, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetRetransmissionDelay(nRF24L01P *nRF, nRF_RETX_DELAY Delay)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_SETUP_RETR, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	Delay &= (0x0F);
	regValue &= (0x0F); //nRF_SETUP_RETR reg's 4, 5, 6 and 7. bits are Cleared
	regValue |= (Delay << 4); //nRF_SETUP_RETR reg's 4, 5, 6 and 7. bits are Configured
	
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_SETUP_RETR, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetRetransmissionCount(nRF24L01P *nRF, nRF_RETX_COUNT Count)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_SETUP_RETR, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	Count &= (0x0F);
	regValue &= (0xF0); //nRF_SETUP_RETR reg's 0, 1, 2 and 3. bits are Cleared
	regValue |= (Count); //nRF_SETUP_RETR reg's 0, 1, 2 and 3. bits are Configured
	
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_SETUP_RETR, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetRFChannel(nRF24L01P *nRF, uint8_t Channel)
{
	/* ---- Pre Process ---- */
	Channel &= 0x7F;	// nRF_RF_CH 7. bit is RESERVED
										// Freq. = (2400 + "Channel" Value) MHz 
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_RF_CH, &Channel) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_ClearInterrupts(nRF24L01P *nRF)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_STATUS, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	regValue |= (0x07 << 4); //nRF_STATUS reg's 4.,5. and 6. bits are RESET
	
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_STATUS, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetContinuousWave(nRF24L01P *nRF, nRF_STATE ContWave)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_RF_SETUP, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	if(ContWave)
	{
		regValue |= 0x80;	//nRF_RF_SETUP reg's 7. bit (CONT_WAVE) is SET
	} else {
		regValue &= 0x7F;	//nRF_RF_SETUP reg's 7. bit (CONT_WAVE) is RESET
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_RF_SETUP, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetTXPower(nRF24L01P *nRF, nRF_TX_PWR Power)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_RF_SETUP, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	regValue &= (0xF9);				//nRF_RF_SETUP reg's 1. and 2. bit are RESET
	regValue |= (Power << 1);	//nRF_RF_SETUP reg's 1. and 2. bit are Configured
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_RF_SETUP, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SetDataRate(nRF24L01P *nRF, nRF_DATA_RATE Rate)
{
	/* ---- Local Vars. ---- */
	uint8_t regValue;
	/* ---- Pre Process ---- */
	if(HAL_nRF24L01P_ReadRegister(nRF, nRF_RF_SETUP, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	switch(Rate)
	{
		case nRF_DATA_RATE_250KBPS:
			regValue |=  (0x01 << 5);	//nRF_RF_SETUP reg's 5. bit is SET
			regValue &= ~(0x01 << 3);	//nRF_RF_SETUP reg's 3. bit is RESET
		break;
		case nRF_DATA_RATE_1MBPS:
			regValue &= ~(0x01 << 5);	//nRF_RF_SETUP reg's 5. bit is RESET
			regValue &= ~(0x01 << 3);	//nRF_RF_SETUP reg's 3. bit is RESET
		break;
		case nRF_DATA_RATE_2MBPS:
			regValue &= ~(0x01 << 5);	//nRF_RF_SETUP reg's 5. bit is RESET
			regValue |=  (0x01 << 3);	//nRF_RF_SETUP reg's 3. bit is SET
		break;
		default:
			return HAL_ERROR;
	}
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_WriteRegister(nRF, nRF_RF_SETUP, &regValue) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_FlushTX(nRF24L01P *nRF)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero = 0x00;
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_FLUSH_TX, &Zero, &Zero, 0) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_FlushRX(nRF24L01P *nRF)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero = 0x00;
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_FLUSH_RX, &Zero, &Zero, 0) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_ReadRXPayload(nRF24L01P *nRF, uint8_t *pRegData)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero[nRF->PayloadWidth];
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_R_RX_PAYLOAD, Zero, pRegData, nRF->PayloadWidth) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_WriteTXPayload(nRF24L01P *nRF, uint8_t *pRegData)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero[nRF->PayloadWidth];
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_W_TX_PAYLOAD, pRegData, Zero, nRF->PayloadWidth) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_WriteTXPayloadACK(nRF24L01P *nRF, uint8_t *pRegData, nRF_DATA_PIPE Pipe)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero[nRF->PayloadWidth];
	Pipe &= 0x07;
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_W_ACK_PAYLOAD + Pipe, pRegData, Zero, nRF->PayloadWidth) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_WriteRegister(nRF24L01P *nRF, uint8_t regAddr, uint8_t *pRegData)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero = 0x00;
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_W_REGISTER + regAddr, pRegData, &Zero, 1) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_ReadRegister(nRF24L01P *nRF, uint8_t regAddr, uint8_t *pRegData)
{
	/* ---- Local Vars. ---- */
	uint8_t Zero = 0x00;
	/* ---- Fcn Process ---- */
	if(HAL_nRF24L01P_SendCommand(nRF, nRF_CMD_R_REGISTER + regAddr, &Zero, pRegData, 1) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef HAL_nRF24L01P_SendCommand(nRF24L01P *nRF, uint8_t Command, uint8_t *TxBuffer, uint8_t *RxBuffer, uint8_t Size)
{
	/* ---- Local Vars. ---- */
	uint8_t tempTxBuffer[Size + 1];
	uint8_t tempRxBuffer[Size + 1];
	/* ---- Pre Process ---- */
	tempTxBuffer[0] = Command;
	tempRxBuffer[0] = 0x00;
	for(uint8_t i = 0; i < Size; i++)
	{
		tempTxBuffer[i + 1] = TxBuffer[i]; // Fill tempTxBuffer with Command + All of TxBuffer
		tempRxBuffer[i + 1] = 0x00;
	}
	/* ---- Fcn Process ---- */
	HAL_nRF24L01P_nSS_Low(nRF); // notSlaveSelect pin is Low
	if(HAL_SPI_TransmitReceive(nRF->hspi, tempTxBuffer, tempRxBuffer, Size + 1, nRF_SPI_TIMEOUT) != HAL_OK)
	{
		return HAL_ERROR;
	}
	/* ---- PostProcess ---- */
	for(uint8_t i = 0; i < Size; i++)
	{
		RxBuffer[i] = tempRxBuffer[i + 1];
	}
	HAL_nRF24L01P_nSS_High(nRF); // notSlaveSelect pin is High
	return HAL_OK;
}

void HAL_nRF24L01P_nSS_High(nRF24L01P *nRF)
{
	HAL_GPIO_WritePin(nRF->nRF_nSS_GPIO_PORT, nRF->nRF_nSS_GPIO_PIN, GPIO_PIN_SET);
}
void HAL_nRF24L01P_nSS_Low(nRF24L01P *nRF)
{
	HAL_GPIO_WritePin(nRF->nRF_nSS_GPIO_PORT, nRF->nRF_nSS_GPIO_PIN, GPIO_PIN_RESET);
}
void HAL_nRF24L01P_CE_High(nRF24L01P *nRF)
{
	HAL_GPIO_WritePin(nRF->nRF_CE_GPIO_PORT, nRF->nRF_CE_GPIO_PIN, GPIO_PIN_SET);
}
void HAL_nRF24L01P_CE_Low(nRF24L01P *nRF)
{
	HAL_GPIO_WritePin(nRF->nRF_CE_GPIO_PORT, nRF->nRF_CE_GPIO_PIN, GPIO_PIN_RESET);
}
