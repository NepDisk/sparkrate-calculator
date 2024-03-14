// sparkrate Calculator for SRB2kart by NepDisk
// How many frames will it take to get to each defined value or boost type.
// Code for driftspark buff by Onyo.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

//Functions straight from Kart. MY FAVORITE
int32_t K_GetKartDriftSparkValue(uint8_t kartspeed, uint8_t kartweight)
{
	return (26*4 + kartspeed*2 + (9 - kartweight))*8;
}


int32_t FixedMul(int32_t a, int32_t b)
{
	// Need to cast to unsigned before shifting to avoid undefined behaviour
	// for negative integers
	return (int32_t)(((uint64_t)((int64_t)a * b)) >> 16);
}

int32_t FixedInt(int32_t a)
{
	return FixedMul(a, 1);
}

int32_t FixedDiv2(int32_t a, int32_t b)
{
	// This does not check for division overflow or division by 0!
	// That is the caller's responsibility.
	return (int32_t)(((int64_t)a * 65536) / b);
}

 int32_t FixedDiv(int32_t a, int32_t b)
{
	if (((uint32_t)abs(a) >> (16-2)) >= (uint32_t)abs(b))
		return (a^b) < 0 ? INT32_MIN : INT32_MAX;

	return FixedDiv2(a, b);
}

int32_t FixedTrunc(int32_t x)
{
	const int32_t a = abs(x); //absolute of x
	const int32_t i = (a>>16)<<16; // cut out the fractional part
	const int32_t f = a-i; // cut out the integral part
	if (x != INT32_MIN)
	{ // return rounded to nearest whole number, towards zero
		if (x > 0)
			return x-f;
		else
			return x+f;
	}
	return INT32_MIN;
}


void main()
{
	uint8_t kartspeed;
	uint8_t kartweight;
	int32_t driftcharge = 0;
	int32_t tics = 0;
	int32_t driftadditive = 24;
	
	int32_t SPtickrate = 1;
	int32_t SPremainder = 17;
	int32_t SPtimeremainder = 0;
	int32_t SPticktofire = 0;
	
	
	int sparkratebuff;
	int driftangle;
	int32_t driftlevel;
	int32_t driftlevelresult;
	int32_t sparkratebuffvalue;
	
	printf("What is your kartspeed?\n");
	scanf("%2" SCNu8, &kartspeed);
	
	printf("What is your kartweight?\n");
	scanf("%2" SCNu8, &kartweight);
	
	printf("What is your drift angle? (Netural:0, Inward:1, Outward:2)\n");
	scanf("%d", &driftangle);
	
	printf("Spark Level? (Blue:0, Red:1, Rainbow:2)\n");
	scanf("%d", &driftlevel);
	
	printf("Sparkrate Buff?\n");
	scanf("%d", &sparkratebuff);
	
	
	switch(driftlevel)
	{
		case 0:
			driftlevelresult = K_GetKartDriftSparkValue(kartspeed,kartweight);
			break;
		case 1:
			driftlevelresult = (K_GetKartDriftSparkValue(kartspeed,kartweight) * 2);
			break;	
		case 2:
			driftlevelresult = (K_GetKartDriftSparkValue(kartspeed,kartweight) * 4);
			break;
		default:
			break;
	}
		
	
	printf("Drift level is: %d\n",driftlevelresult);
	
	switch(driftangle)
	{		
		case 1:
			driftadditive += abs(800)/100;
			sparkratebuffvalue = (13 - ((kartspeed * 3) / 2));
			break;
		case 2:
			driftadditive -= abs(-800)/75;
			sparkratebuffvalue = (4 - (kartspeed / 2));
			break;	
		default:
			sparkratebuffvalue = (9 - kartspeed);
			break;	
	}
		
	for (driftcharge = 0; driftcharge < driftlevelresult; driftcharge +=  driftadditive)
	{
		tics += 1;
		printf("Driftcharge is %d\n",driftcharge);
		printf("Tics are %d\n", tics);
		
		if (sparkratebuff)
		{
			if	(driftcharge && SPticktofire == 0)
			{
				//Initalize ticktofire
				SPticktofire = tics + SPtickrate;
				SPtimeremainder = 0;
			}
			else if (driftcharge && tics >= SPticktofire)
			{

				int32_t newvalue = driftcharge + sparkratebuffvalue;

				driftcharge = newvalue;

				SPtimeremainder += SPremainder;

				int32_t tickdelay = FixedInt( FixedTrunc( FixedDiv( SPtimeremainder , 100 ) ) ); //If remainder is > 100, then calculate how much to delay the next tick
				SPtimeremainder %= 100; //Clear counter of the 100's place since it was factored in		

				//Set the next tick to fire on
				SPticktofire = tics + SPtickrate + tickdelay;
			}
		}
		
	}
	
	printf("Tics to selected level was %d for %d SP %d WT\n",tics,kartspeed,kartweight);
	
	return;
}
