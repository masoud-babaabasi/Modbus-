/* 
 * Edited : Masoud Babaabsi
 * Date : January 2023
 * File: $Id: mbcrc.c,v 1.7 2023/01/18
 */

/* ----------------------- Platform includes --------------------------------*/
#ifndef __MBCRC_H
#define __MBCRC_H
#include <stdint.h>

uint16_t usMBCRC16(uint8_t * pucFrame, uint16_t usLen, uint8_t ucCRCHi, uint8_t ucCRCLo);

#endif