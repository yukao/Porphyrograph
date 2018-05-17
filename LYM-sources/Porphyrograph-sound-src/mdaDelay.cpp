////////////////////////////////////////////////////////////////////
// mda VST plug-ins http://mda.smartelectronix.com/
//
// Copyright (c) 2008 Paul Kellett
//
// Permission is hereby granted, free of charge, to any person 
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, 
// publish, distribute, sublicense, and/or sell copies of the Software, 
// and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be 
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
// OTHER DEALINGS IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////

#include "pg-all_include.h"

mdaDelay::mdaDelay(void)		// programs, parameters
{
  //inits here! kLDelay = 0,kLRDelayRatio,kFeedBk,kTone,kMix,kLevel
  fParam0 = 0.50f; //left delay
  fParam1 = 0.27f; //right ratio
  fParam2 = 0.70f; //feedback
  fParam3 = 0.50f; //tone
  fParam4 = 0.33f; //wet mix
  fParam5 = 0.50f; //output

  size = 32766;  //set max delay time at max sample rate
  buffer = new float[size + 2]; //spare just in case!
  ipos = 0;
  fil0 = 0.0f;

  strcpy(programName, "Delay");
	
  suspend();		//flush buffer
  setParameter(0, 0.5); 
}
void mdaDelay::Init(void) {
  //inits here! kLDelay = 0,kLRDelayRatio,kFeedBk,kTone,kMix,kLevel
  fParam0 = 0.50f; //left delay
  fParam1 = 0.27f; //right ratio
  fParam2 = 0.70f; //feedback
  fParam3 = 0.50f; //tone
  fParam4 = 0.33f; //wet mix
  fParam5 = 0.50f; //output

  size = 32766;  //set max delay time at max sample rate
  buffer = new float[size + 2]; //spare just in case!
  ipos = 0;
  fil0 = 0.0f;

strcpy(programName, "Delay");
	
  suspend();		//flush buffer
  setParameter(0, 0.5); 
}

bool  mdaDelay::getProductString(char* text) { strcpy(text, "mda Delay"); return true; }
bool  mdaDelay::getVendorString(char* text)  { strcpy(text, "mda"); return true; }
bool  mdaDelay::getEffectName(char* name)    { strcpy(name, "Delay"); return true; }

void mdaDelay::setParameter(VstInt32 index, float value)
{
  float tmp;

  switch(index)
  {
    case kLDelay: fParam0 = value; break;
    case kLRDelayRatio: fParam1 = value; break;
    case kFeedBk: fParam2 = value; break;
    case kTone: fParam3 = value; break;
    case kMix: fParam4 = value; break;
    case kLevel: fParam5 = value; break;
 }
  //calcs here
  ldel = (VstInt32)(size * fParam0 * fParam0);
  if(ldel<4) ldel=4;
  
  switch(int(fParam1 * 17.9f)) //fixed left/right ratios
  {
    case  17: tmp = 0.5000f; break;
    case  16: tmp = 0.6667f; break;
    case  15: tmp = 0.7500f; break;
    case  14: tmp = 0.8333f; break;
    case  13: tmp = 1.0000f; break;
    case  12: tmp = 1.2000f; break;
    case  11: tmp = 1.3333f; break;
    case  10: tmp = 1.5000f; break;
    case   9: tmp = 2.0000f; break;
    default: tmp = 4.0f * fParam1; break; //variable ratio
  }
  rdel = (VstInt32)(size * fParam0 * fParam0 * tmp);
  if(rdel>size) rdel=size;
  if(rdel<4) rdel=4;

  fil = fParam3;

  if(fParam3>0.5f)  //simultaneously change crossover frequency & high/low mix
  {
    fil = 0.5f * fil - 0.25f; 
    lmix = -2.0f * fil;
    hmix = 1.0f;
  }
  else 
  { 
    hmix = 2.0f * fil; 
    lmix = 1.0f - hmix;
  }
  fil = (float)exp(-6.2831853f * pow(10.0f, 2.2f + 4.5f * fil) / PG_AUDIO_SAMPLE_RATE);

  fbk = 0.495f * fParam2;
  wet = 1.0f - fParam4;
  wet = fParam5 * (1.0f - wet * wet); //-3dB at 50% mix
  dry = fParam5 * 2.0f * (1.0f - fParam4 * fParam4);

  //if(fParam2>0.99) { fbk=0.5f; wet=0.0f; } //freeze
}

mdaDelay::~mdaDelay()
{
	if(buffer) delete [] buffer;
}

void mdaDelay::suspend()
{
	memset(buffer, 0, size * sizeof(float));
}

void mdaDelay::setProgramName(char *name)
{
	strcpy(programName, name);
}

void mdaDelay::getProgramName(char *name)
{
	strcpy(name, programName);
}

bool mdaDelay::getProgramNameIndexed (VstInt32 category, VstInt32 index, char* name)
{
	if (index == 0) 
	{
	    strcpy(name, programName);
	    return true;
	}
	return false;
}

float mdaDelay::getParameter(VstInt32 index)
{
	float v=0;

  switch(index)
  {
    case 0: v = fParam0; break;
    case 1: v = fParam1; break;
    case 2: v = fParam2; break;
    case 3: v = fParam3; break;
    case 4: v = fParam4; break;
    case 5: v = fParam5; break;
  }
  return v;
}

void mdaDelay::getParameterName(VstInt32 index, char *label)
{
	switch(index)
  {
    case 0: strcpy(label, "L Delay "); break;
    case 1: strcpy(label, "R Delay "); break;
    case 2: strcpy(label, "Feedback"); break;
    case 3: strcpy(label, "Fb Tone "); break;
    case 4: strcpy(label, "FX Mix  "); break;
    case 5: strcpy(label, "Output  "); break;
  }
}

#include <stdio.h>
void int2strng(VstInt32 value, char *string) { sprintf(string, "%d", value); }

void mdaDelay::getParameterDisplay(VstInt32 index, char *text)
{
	switch(index)
  {
    case 0: int2strng((VstInt32)(ldel * 1000.0f / PG_AUDIO_SAMPLE_RATE), text); break;
    case 1: int2strng((VstInt32)(100 * rdel / ldel), text); break;
    case 2: int2strng((VstInt32)(99 * fParam2), text); break;
    case 3: int2strng((VstInt32)(200 * fParam3 - 100), text); break;
    case 4: int2strng((VstInt32)(100 * fParam4), text); break;
    case 5: int2strng((VstInt32)(20 * log10(2.0 * fParam5)), text); break;
  }
}

void mdaDelay::getParameterLabel(VstInt32 index, char *label)
{
	switch(index)
  {
    case 0:  strcpy(label, "ms"); break;
    case 3:  strcpy(label, "Lo <> Hi"); break;
    case 5:  strcpy(label, "dB"); break;
    default: strcpy(label, "%"); break;
  }
}

//--------------------------------------------------------------------------------
// process

void mdaDelay::process(float **inputs, float **outputs, VstInt32 sampleFrames)
{
  float *in1 = inputs[0];
  float *in2 = inputs[1];
  float *out1 = outputs[0];
  float *out2 = outputs[1];
  float currentInLeft, currentInRight, currentOutLeft, currentOutRight, 
	    outRightDelay, outLeftDelay;
  float lowMix=lmix, highMix=hmix, f=fil, f0=fil0, highDelaySignal;
  VstInt32 index=ipos, leftDelayInd, rightDelayInd;

  leftDelayInd = (index + ldel) % (size + 1);
  rightDelayInd = (index + rdel) % (size + 1);

  --in1;	
  --in2;	
  --out1;
  --out2;
  while(--sampleFrames >= 0) {
    currentInLeft = *++in1;
    currentInRight = *++in2;

    currentOutLeft = out1[1];
    currentOutRight = out2[1]; 

    outLeftDelay = *(buffer + leftDelayInd);
    outRightDelay = *(buffer + rightDelayInd);
    
    // calculates echoed signal and writes it
    highDelaySignal = wet * (currentInLeft + currentInRight) 
                    + fbk * (outLeftDelay + outRightDelay); 
    f0 = f * (f0 - highDelaySignal) + highDelaySignal;   
    *(buffer + index) = lowMix * f0 + highMix * highDelaySignal; 

    // current index and L/R indices management (circular buffer)
    index--; 
    if(index<0) 
      index=size;
    leftDelayInd--; 
    if(leftDelayInd<0) 
      leftDelayInd=size;
    rightDelayInd--;
    if(rightDelayInd<0) 
      rightDelayInd=size;

    *++out1 = currentOutLeft 
              + dry * currentInLeft + outLeftDelay; 
    *++out2 = currentOutRight 
              + dry * currentInRight + outRightDelay; 
  }
  // saves current index for next processing
  ipos = index;
  if(fabs(f0)<1.0e-10) 
    fil0=0.0f; 
  else 
    fil0=f0; 
}

void mdaDelay::processReplacing(float **inputs, float **outputs, 
				VstInt32 sampleFrames)
{
  float *in1 = inputs[0];
  float *in2 = inputs[1];
  float *out1 = outputs[0];
  float *out2 = outputs[1];
  float currentInLeft, currentInRight, outLeftDelay, outRightDelay;
  float lowMix=lmix, highMix=hmix, f=fil, f0=fil0, highDelaySignal;
  VstInt32 index=ipos, leftDelayInd, rightDelayInd;

  leftDelayInd = (index + ldel) % (size + 1);
  rightDelayInd = (index + rdel) % (size + 1);

  --in1;	
  --in2;	
  --out1;
  --out2;
  while(--sampleFrames >= 0) {
    currentInLeft = *++in1;
    currentInRight = *++in2;

    outLeftDelay = *(buffer + leftDelayInd); //delay outputs
    outRightDelay = *(buffer + rightDelayInd);
    
    // calculates echoed signal and writes it
    // high delay signal
    highDelaySignal = wet * (currentInLeft + currentInRight) 
                    + fbk * (outLeftDelay + outRightDelay); 
                    //mix input & feedback

    // low delay signal
    f0 = f * (f0 - highDelaySignal) + highDelaySignal;    //low-pass filter
    *(buffer + index) = lowMix * f0 
                      + highMix * highDelaySignal; //delay input

    // current index and L/R indices management (circular buffer)
    index--; 
    if(index<0) 
      index=size;
    leftDelayInd--; 
    if(leftDelayInd<0) 
      leftDelayInd=size;
    rightDelayInd--;
    if(rightDelayInd<0) 
      rightDelayInd=size;

    *++out1 = dry * currentInLeft + outLeftDelay; //mix wet & dry
    *++out2 = dry * currentInRight + outRightDelay; 
  }
  // saves current index for next processing
  ipos = index;
  if(fabs(f0)<1.0e-10) 
    fil0=0.0f; 
  else 
    fil0=f0; //trap denormals
}
