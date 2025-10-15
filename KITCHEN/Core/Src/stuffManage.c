/*
 * stuffAlretUart.c
 *
 *  Created on: Nov 25, 2024
 *      Author: root
 */
#include "stuffManage.h"

extern UART_HandleTypeDef huart2;

const char* stuffToString(stuff index)
{
	switch (index)
	{
	case Seaweed : return "Seaweed";
	case Potato : return "Potato";
	case SalmonRoe : return "SalmonRoe";
	case Deodeok : return "Deodeok";
	case Radish : return "Radish";
	case RedCabbage : return "RedCabbage";
	case Almond : return "Almond";
	case Cavior : return "Cavior";
	case Tofu : return "Tofu";
	case Egg : return "Egg";
	case Yuzu : return "Yuzu";
	case Snowcrab : return "Snowcrab";
	case Beansprouts : return "Beansprouts";
	case Ablalone : return "Ablalone";
	case KoreanBeef : return "KoreanBeef";
	case Apple : return "Apple";
	case Onion : return "Onion";
	case Acorn : return "Acorn";
	case Truffle : return "Truffle";
	case Noddle : return "Noddle";
	case Sesame : return "Sesame";
	case Burdock : return "Burdock";
	case Tilefish : return "Tilefish";
	case Mustard : return "Mustard";
	case Brassica : return "Brassica";
	case Tuna : return "Tuna";
	case Hibiscus : return "Hibiscus";
	case Peanut : return "Peanut";
	case HoneyCookie : return "HoneyCookie";
	case Kombucha : return "Kombucha";
	default : return "Invalid Stuff";
	}
}

void transmitString(const char* message)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 1000);
}


void printRemainingStuff(stuff index)
{
	if (index <= Kombucha)
	{
		char message[50];
		sprintf(message, "remaining %s : %d\r\n", stuffToString(index), remainingStuff[index]);
		transmitString(message);
	}
	else
	{
		transmitString("Invalid stuff\r\n");
	}
}
