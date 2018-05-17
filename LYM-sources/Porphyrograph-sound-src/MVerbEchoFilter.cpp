/*
© 2004 Wusik.com - Williamk

	Type -  0 LowPass, 1 BandPass, 2 HighPass

	Below this is a Simple One Pole Filter.
*/
#include "pg-all_include.h"

//======================================================================================================
Filter :: Filter()
{
  init();
} 

//---------------------------------------------------------------------------------------
void Filter :: init()
{
  currentSampleRate = 44100.0f;
  for (int i=0; i<7; i++) dBuffer[i] = 0;
  Samplerate = (float)(6.28319 / 44100.0f) * (6500);
  Frequency = Samplerate;
  Rezonance = 1; 
  Pole = 4; // 24db 4Pole
  Type = 0; // LowPass
  Glide = 0;
  Gain = 1;
  Pre = 0;
  Smooth = true;
}

//---------------------------------------------------------------------------------------
void Filter :: updateSampleRate(float aSampleRate)
{
	currentSampleRate = aSampleRate;
}

//---------------------------------------------------------------------------------------
void Filter :: setType(int aType)
{
	Type = aType;
}

//---------------------------------------------------------------------------------------
void Filter :: setPole(int aPole)
{
	Pole = aPole;
}

//---------------------------------------------------------------------------------------
void Filter :: setSmooth (bool aSmooth)
{
	Smooth = aSmooth;
}

//---------------------------------------------------------------------------------------
void Filter :: setFreq(float aFrequency)
{
	Samplerate = (float)(6.28319 / currentSampleRate) * (6500);
	double y = ((double)aFrequency/1);
	double x = (y*y)+0.03;
	Frequency = (float)(x * Samplerate);
	if (Frequency > 1) Frequency = 1;
	if (Frequency < 0.02f) Frequency = 0.02f;
}

//---------------------------------------------------------------------------------------
void Filter :: setPureFreq(float aFrequency)
{
	Frequency = (float)(aFrequency * (6.28319 / currentSampleRate));
}

//---------------------------------------------------------------------------------------
void Filter :: setRezo(float aRezonance )
{
	if (aRezonance < 0.3f) aRezonance = 0.3f;
	if (aRezonance > .93f) aRezonance = 0.93f;
	Gain = (float)(((aRezonance * -1) * 0.9) + 0.94);
	Rezonance = (1 - aRezonance) + (1 - aRezonance);
}

//---------------------------------------------------------------------------------------
float Filter :: tick(float Input)
{
	// Pole 1
	HighOut = (Input - (dBuffer[1] + (dBuffer[0] * Rezonance )));
	BandOut = (HighOut * Frequency) + dBuffer[0];
	LowOut = (BandOut * Frequency) + dBuffer[1];
	denormal(BandOut);
	denormal(LowOut);
	dBuffer[0] = BandOut;
	dBuffer[1] = LowOut;
	switch (Type)
	{
		case 0: Out = LowOut; break;
		case 1: Out = LowOut; break;
		case 2: Out = HighOut; break;
	}

	if (Pole == 2) return Out;

	// Pole 2
	HighOut2 = (Out - (dBuffer[3] + (dBuffer[2] * Rezonance )));
	BandOut2 = (HighOut2 * Frequency) + dBuffer[2];
	LowOut2 = (BandOut2 * Frequency) + dBuffer[3];
	denormal(BandOut2);
	denormal(LowOut2);
	dBuffer[2] = BandOut2;
	dBuffer[3] = LowOut2;

	switch (Type)
	{
		case 0: Out2 = LowOut2; break;
		case 1: Out2 = HighOut2; break;
		case 2: Out2 = HighOut2; break;
	}

	if (Smooth) {
		Pre = ((Out2 - dBuffer[5]) * Glider2(Frequency)) + dBuffer[5];
		denormal(Pre);
		dBuffer[5] = Pre;
	} else Pre = Out2;

	return Pre*Glider(Gain);
}

//---------------------------------------------------------------------------------------
float Filter :: ticklow(float Input)
{
	// Pole 1
	BandOut = (((Input - (dBuffer[1] + (dBuffer[0] * Rezonance ))))
			  * Frequency) + dBuffer[0];
	LowOut = (BandOut * Frequency) + dBuffer[1];
	denormal(BandOut);
	denormal(LowOut);
	dBuffer[0] = BandOut;
	dBuffer[1] = LowOut;
	
	if (Pole == 2) return LowOut;

	// Pole 2
	BandOut2 = (((LowOut - (dBuffer[3] + (dBuffer[2] * Rezonance ))))
			   * Frequency) + dBuffer[2];
	LowOut2 = (BandOut2 * Frequency) + dBuffer[3];
	denormal(BandOut2);
	denormal(LowOut2);
	dBuffer[2] = BandOut2;
	dBuffer[3] = LowOut2;

	if (Smooth) {
		Pre = ((LowOut2 - dBuffer[5]) * Glider2(Frequency)) + dBuffer[5];
		denormal(Pre);
		dBuffer[5] = Pre;
	} else Pre = LowOut2;

	return Pre*Glider(Gain);
}

//---------------------------------------------------------------------------------------
float Filter :: tickBB(float Input, float BB)
{
	// 2 Pole WASP BandPass
	HighOut = (Input - (dBuffer[1] + (dBuffer[0] * Rezonance )));
	BandOut = (HighOut * Frequency) + dBuffer[0];
	LowOut = (BandOut * Frequency) + dBuffer[1];
	denormal(BandOut);
	denormal(LowOut);
	dBuffer[0] = BandOut;
	dBuffer[1] = LowOut;

	return ((BandOut * Rezonance) * BB) + Input;
}

//---------------------------------------------------------------------------------------
float Filter :: Glider(float Desired)
{   // Filter Gain / Rez
	if (Desired != dBuffer[4]) {
		if (dBuffer[4] > 0.67) {dBuffer[4] = 0.67f;}
		if (dBuffer[4] < 0.103f) {dBuffer[4] = 0.103f;}
		if (Desired > dBuffer[4]) {dBuffer[4] += 0.001f;}
		if (Desired < dBuffer[4]) {dBuffer[4] -= 0.001f;}
	}
	return dBuffer[4];
}

//---------------------------------------------------------------------------------------
float Filter :: Glider2(float Desired)
{  // Filter Freq
	if (Desired != dBuffer[6]) {
		if (dBuffer[6] > 1) {dBuffer[6] = 1;}
		if (dBuffer[6] < 0.02f) {dBuffer[6] = 0.02f;}
		if (Desired > dBuffer[6]) {dBuffer[6] += 0.01f;}
		if (Desired < dBuffer[6]) {dBuffer[6] -= 0.01f;}
	}
	return dBuffer[6];
}

//======================================================================================================
// Simpler One Pole Filter (no rezonance) ... Type 0 = Low - 1 = High;
FilterOneP :: FilterOneP()
{
  Type = 0; // LowPass
  Frequency = 1;
  init();
  currentSampleRate = 44100.0f;
}

//---------------------------------------------------------------------------------------
void FilterOneP :: updateSampleRate(float aSampleRate)
{
	currentSampleRate = aSampleRate;
}

//---------------------------------------------------------------------------------------
void FilterOneP :: init( void )
{
  dBuffer = 0;
  Samplerate = (float)(6.28319 / currentSampleRate) * 6500;
}

//---------------------------------------------------------------------------------------
void FilterOneP :: setType( int aType)
{
	Type = aType;

    dBuffer = 0;
    Samplerate = (float)(6.28319 / currentSampleRate) * 6500;
}

//---------------------------------------------------------------------------------------
void FilterOneP :: setFreq( float aFrequency)
{
	Samplerate = (float)(6.28319 / currentSampleRate) * 6500;
	double y = ((double)aFrequency/1);
	double x = (y*y*y)+0.03;
	Frequency = (float)(x * Samplerate);
	if (Frequency > 1) Frequency = 1;
	if (Frequency < 0.02f) Frequency = 0.02f;
}

//---------------------------------------------------------------------------------------
void FilterOneP :: setPureFreq( float aFrequency) 
{
	Frequency = (float)(aFrequency * (6.28319 / currentSampleRate));
}

//---------------------------------------------------------------------------------------
float FilterOneP :: tick(float Input)
{
	Pre = ((Input - dBuffer) * Frequency) + dBuffer;
	if (Type == 1) // High Pass
		Out = (Input - dBuffer);
	else
		Out = Pre;
	denormal(Pre);
	dBuffer = Pre;
	return Out;
}

//---------------------------------------------------------------------------------------
float FilterOneP :: ticklow(float Input)
{
	dBuffer = ((Input - dBuffer) * Frequency) + dBuffer;
	//denormal(Pre);
	//dBuffer = Pre;
	return dBuffer; //Pre;
}

//---------------------------------------------------------------------------------------
float FilterOneP :: tickhigh(float Input)
{
	Pre = ((Input - dBuffer) * Frequency) + dBuffer;
	Out = (Input - dBuffer);
	denormal(Pre);
	dBuffer = Pre;
	return Out;
}

//======================================================================================================
// All Pass Filter
FilterAllP :: FilterAllP()
{
  dBuffer = 0;
  G = 0;
  currentSampleRate = 44100.0f;
  SampleRate = (long)currentSampleRate;
  Ammount = (long)(SampleRate*0.1f);
  Delay = new DelayL(0,Ammount);
  Delay->setModulated(true);
}

//---------------------------------------------------------------------------------------
void FilterAllP :: updateSampleRate(float aSampleRate)
{
	currentSampleRate = aSampleRate;
}

//---------------------------------------------------------------------------------------
FilterAllP :: ~FilterAllP()
{
	delete Delay;
}

//---------------------------------------------------------------------------------------
void FilterAllP::sampleRateCheck(void)
{
	if (currentSampleRate != SampleRate) {
		SampleRate = (long)currentSampleRate;
		Ammount = (long)(SampleRate*0.1f);
		Delay->newDelay(Ammount);
	}
}

//---------------------------------------------------------------------------------------
void FilterAllP :: setDelay( float aT)
{
	sampleRateCheck();
	if (aT > 0.1f) aT = 0.1f;
	if (aT < 0) aT = 0;

	Delay->setDelay(aT*(float)Ammount);
}

//---------------------------------------------------------------------------------------
void FilterAllP :: setGain( float aG)
{
	sampleRateCheck();
	if (aG > 1.0f) aG = 1.0f;
	if (aG < 0) aG = 0;
	G = aG;
}

//---------------------------------------------------------------------------------------
float FilterAllP :: tick(float Input)
{
	Out = dBuffer + (Input * ((G*-1.0f)+1.0f));
	dBuffer = Delay->tickmod((Out * G) + Input);
	denormal(dBuffer);
	return Out;
}
