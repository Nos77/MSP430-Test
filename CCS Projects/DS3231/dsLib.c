// dsLib.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	Inspired by https://github.com/mizraith/RTClib/blob/master/RTC_DS3231.cpp
//
// Requirements:
//	dsLib.h
//
// Description:
// 	Interface with DS3231 to get time
//
// Notes:
//
// Todo:
//	Make more durable, timeouts, testing, etc.
//****************************************************************************************************



// Includes ------------------------------------------------------------------------------------------
#include "dsLib.h"



// Functions -----------------------------------------------------------------------------------------
uint8_t bcd2bin (uint8_t val){
	return val - 6 * (val >> 4);
}

uint8_t bin2bcd (uint8_t val){
	return val + 6 * (val / 10);
}

void DS3231GetCurrentTime(void){
    int i;	// For loop counter

	// Configure USCI_B0 for I2C mode - Sending
	UCB0CTLW0 |= UCSWRST;                     // Software reset enabled
	UCB0CTLW0 |= UCMODE_3 | UCMST | UCSYNC | UCTR | UCSSEL__SMCLK;   // I2C mode, Master mode, sync, Sending, SMCLK
	UCB0BRW = 0x0004;                         // baudrate = SMCLK / 4
	UCB0CTLW1 |= UCASTP_2;					  // Auto stop
	UCB0TBCNT = DS3231_TIME_LENGTH;			  // Auto stop count
	UCB0I2CSA = DS3231_I2C_ADDRESS;           // Slave address
	UCB0CTL1 &= ~UCSWRST;					  // Clear reset
	UCB0IE &= ~UCRXIE;						  // Clear rx interrupt
	UCB0IE &= ~UCTXIE;						  // Clear tx interrupt

	UCB0CTLW0 |= UCTXSTT;			// Send start
	while(!(UCB0IFG & UCTXIFG0));	// Wait for tx interrupt
	UCB0TXBUF = DS3231_REG_SECONDS;	// Send register address
	while(!(UCB0IFG & UCTXIFG0));
	UCB0CTLW0 |= UCTXSTP;					// Send stop
	UCB0CTLW0 &= ~UCTR;					// Change to receive
	while(UCB0CTLW0 & UCTXSTP);			// Ensure stop condition got sent

	UCB0CTLW0 |= UCTXSTT;			// Send start
	while(UCB0CTLW0 & UCTXSTT);	// Wait for tx interrupt
	for(i = 0; i < DS3231_TIME_LENGTH; i++){
		while(!(UCB0IFG & UCRXIFG0));		// Wait for RX interrupt flag
		g_getTimeArr[i] = UCB0RXBUF;		// Read data
	}
	while(UCB0CTLW0 & UCTXSTP);	// Wait for stop

	for(i = 0; i < DS3231_TIME_LENGTH; i++){
		g_getTimeArr[i] = bcd2bin(g_getTimeArr[i]);
	}
}
