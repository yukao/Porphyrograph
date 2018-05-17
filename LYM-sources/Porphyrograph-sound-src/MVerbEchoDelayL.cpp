/*
© 2004 Wusik.com - Williamk
*/
#include "pg-all_include.h"
//#include "emmintrin.h"

//======================================================================================================
DelayL :: DelayL(float theDelay, long maxDelay)
{
  Loading = true;
  doNextOut = false;
  inPoint = 0;
  Modulated = false;
  outPoint = 0;
  length = maxDelay+1;
  inputs = new float[length+10];
  outputs = new float[1];
  clear();
  inPoint = 0;
  setDelay(theDelay);
  doNextOut = true;
  gate = 10000;
  apInput = 0;
  Loading = false;
}

//---------------------------------------------------------------------------------------
void DelayL :: newDelay(long maxDelay)
{
  Loading = true;
  length = maxDelay+1;
  delete [] inputs;
  inputs = new float[length+10];
  clear();
  setDelay(1);
  Loading = false;
}

//---------------------------------------------------------------------------------------
DelayL :: ~DelayL()
{
	doNextOut = false;
	delete [] inputs;
	delete [] outputs;
}

//---------------------------------------------------------------------------------------
void DelayL :: clear(void)
{
  for (long i=0; i<length+10; i++) inputs[i] = 0.0f;
  outputs[0] = 0.0f;
}


//---------------------------------------------------------------------------------------
void DelayL :: setDelay(float theDelay)
{
  float outPointer;

  if (theDelay > length-1) {
    // Force delay to maxLength
    outPointer = (float)(inPoint + 1.0);
    delay = (float)(length - 1);
  }
 #if ASM == 0
  else if (theDelay < 0 ) {
    outPointer = (float)inPoint + 0.4999999999f;
    delay = 0.5;
  }
  else {
    outPointer = inPoint - theDelay + 1.0f;  // read chases write
    delay = theDelay;
  }

  while (outPointer < 0)
    outPointer += length; // modulo maximum length

  outPoint = (long) outPointer;  // integer part
  alpha = 1.0f + outPointer - outPoint; // fractional part
  omAlpha = (float) 1.0 - alpha;

  if (alpha < 0.5) {
    // The optimal range for alpha is about 0.5 - 1.5 in order to
    // achieve the flattest phase delay response.
    outPoint += 1;
    if (outPoint >= length) outPoint -= length;
    alpha += (float) 1.0;
  }

  coeff = ((float) 1.0 - alpha) / ((float) 1.0 + alpha); // coefficient for all pass
 #else
  else if (theDelay < 0 ) {
    outPointer = (float)inPoint;
    delay = 0.5;
  }
  else {
    outPointer = inPoint - theDelay;  // read chases write
    delay = theDelay;
  }

  while (outPointer < 0)
    outPointer += length; // modulo maximum length

  //outPoint = (long) outPointer;  // integer part

  long xoutPoint;
  _asm {
	movss		xmm1,	[outPointer]
	cvttss2si	eax,	xmm1
	mov			[xoutPoint],	eax
  }
  outPoint = xoutPoint; // integer part

  alpha = outPointer - outPoint; // fractional part
  omAlpha = (float) 1.0 - alpha;

  coeff = ((float) 1.0 - (alpha+1.0f)) / ((float) 1.0 + (alpha+1.0f)); // coefficient for all pass
 #endif
}

//---------------------------------------------------------------------------------------
void DelayL :: setModulated(bool aModulated)
{
	Modulated = aModulated;
}

//---------------------------------------------------------------------------------------
float DelayL :: getDelay(void) const
{
  return delay;
}

//---------------------------------------------------------------------------------------
float DelayL :: nextOut(void)
{
  if ( doNextOut ) {
    // First 1/2 of interpolation
    nextOutput = inputs[outPoint] * omAlpha;
    // Second 1/2 of interpolation
    if (outPoint+1 < length)
      nextOutput += inputs[outPoint+1] * alpha;
    else
      nextOutput += inputs[0] * alpha;
    doNextOut = false;
  }

  return nextOutput;
}

//---------------------------------------------------------------------------------------
float DelayL :: tickraw(float sample)
{
  if (Loading) return 0;

  inputs[inPoint++] = sample;

  if (inPoint >= length) {
    inPoint -= length;
		if (inPoint < 0) inPoint = 0;
  }

  // Increment input pointer modulo length.
  outputs[0] = inputs[outPoint++];

  if (outPoint >= length) {
	  outPoint -= length;
	  if (outPoint < 0) outPoint = 0;
  }
  return outputs[0];
}

//---------------------------------------------------------------------------------------
float DelayL :: tickmod(float sample)
{
  if (Loading) return 0;

  inputs[inPoint++] = sample;
 
  if (inPoint >= length)
  {
    inPoint -= length;
		if (inPoint < 0) inPoint = 0;
  }

  // Increment input pointer modulo length.
  if ( doNextOut ) {
    // First 1/2 of interpolation
    outputs[0] = inputs[outPoint] * omAlpha;
    // Second 1/2 of interpolation
    if (outPoint+1 < length)
      outputs[0] += inputs[outPoint+1] * alpha;
    else
      outputs[0] += inputs[0] * alpha;
    doNextOut = false;
  }
  doNextOut = true;

  if (++outPoint >= length)  
  {
	  outPoint -= length;
	  if (outPoint < 0) outPoint = 0;
  }
  return outputs[0];
}

//---------------------------------------------------------------------------------------
bool DelayL :: notIdle()
{
	if (gate > delay) return false; else return true;
}

bool DelayL :: Idle()
{
	if (gate > delay) return true; else return false;
}

//---------------------------------------------------------------------------------------
float DelayL :: nextOutA(void)
{
  if ( doNextOut ) {
    // Do allpass interpolation delay.
    nextOutput = -coeff * outputs[0];
    nextOutput += apInput + (coeff * inputs[outPoint]);
    doNextOut = false;
  }
  return nextOutput;
}

//---------------------------------------------------------------------------------------
void DelayL :: setNotIdle (void)
{
	gate = 0;
}
