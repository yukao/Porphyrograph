/*
   Wusik Effect V4 (c) Wusik Dot Com - WilliamK
*/
#include "pg-all_include.h"

#define MVerb  MVerb<float>

// template <class T> const T& min(const T& a, const T& b) { return (a<b)?a:b; }

//======================================================================================================
WsEffect::WsEffect(void)	// 1 program, 1 parameter only
{
	vst_strncpy(programName, "Default", kVstMaxProgNameLen);	// default program name

	memset(fParameter,0,sizeof(fParameter));
	memset(cParameterName,0,sizeof(cParameterName));

	sampleRate = 44100.0f;

	// Init(); should be called outside (as for MVerb)
}
void WsEffect::Init()
{
	strncpy(cParameterName[kPreDelay],"PreDelay", kVstMaxParamStrLen);
	strncpy(cParameterName[kDecay],   "Decay",    kVstMaxParamStrLen);
	strncpy(cParameterName[kLowPass], "LowPass",  kVstMaxParamStrLen);
	strncpy(cParameterName[kHighPass],"HighPass", kVstMaxParamStrLen);
	strncpy(cParameterName[kStereoIn],"StereoIn", kVstMaxParamStrLen);
	strncpy(cParameterName[kDeviate], "Deviate",  kVstMaxParamStrLen);
	strncpy(cParameterName[kEchoVol], "Echo Vol", kVstMaxParamStrLen);
	strncpy(cParameterName[kEchoTime],"Echo Time",kVstMaxParamStrLen);
	strncpy(cParameterName[kFeedBk],  "Feedbk",   kVstMaxParamStrLen);
	strncpy(cParameterName[kFilter],  "Filter",   kVstMaxParamStrLen);
	strncpy(cParameterName[kFiltTyp], "FiltTyp",  kVstMaxParamStrLen);
	strncpy(cParameterName[kPanOut],  "PanOut",   kVstMaxParamStrLen);
	strncpy(cParameterName[kLevel],   "Level",    kVstMaxParamStrLen);
	strncpy(cParameterName[kMix],     "Mix",      kVstMaxParamStrLen);
 
	bLoading = true;

	memset(xEDText,0,sizeof(xEDText));

	//BlankSize = 4;
	//Blank = new float[4];

	PrevDelay = -1;
  counter = 0;
  liquidDelay = 0.5;
  DelayLevel = 1;
  BPM = 120.0f;
  Ammount = 44100*3; // 3 Seconds
  if (Ammount <= 0) Ammount = 100;
	Delay  = float(Ammount-1);

	Delay1 = new DelayL(Delay, Ammount);
  Delay2 = new DelayL(Delay, Ammount);
  Delay1->setModulated(false);
  Delay2->setModulated(false);
  Filter1 = new FilterOneP();
  Filter2 = new FilterOneP();
	bufoutputs = new float*[2];
	HighFilter[0] = new FilterOneP();
	HighFilter[1] = new FilterOneP();

	Reset();

	Wet = 1;
	Dry = 0;
	em_verb.setParameter(MVerb::DAMPINGFREQ, 0.0f); 
	em_verb.setParameter(MVerb::DENSITY, 1.0f);
	//em_verb.setParameter(MVerb::BANDWIDTHFREQ, 1.0f); 
	//em_verb.setParameter(MVerb::DECAY, 0.5f);
	//em_verb.setParameter(MVerb::PREDELAY, 0.0f);      
	//em_verb.setParameter(MVerb::SIZE, 0.7f); //also set in Reset() !!
  //em_verb.setParameter(MVerb::GAIN, 0.45f);
	em_verb.setParameter(MVerb::MIX, 1.0f);
	em_verb.setParameter(MVerb::EARLYMIX, 1.0f);

	em_verb2.setParameter(MVerb::DAMPINGFREQ, 0.0f);
	em_verb2.setParameter(MVerb::DENSITY, 1.0f);
	//em_verb.setParameter(MVerb::BANDWIDTHFREQ, 1.0f); 
	//em_verb.setParameter(MVerb::DECAY, 0.5f);
	//em_verb.setParameter(MVerb::PREDELAY, 0.0f);      
	//em_verb.setParameter(MVerb::SIZE, 0.7f); //also set in Reset() !!
  //em_verb2.setParameter(MVerb::GAIN, 0.45f);
	em_verb2.setParameter(MVerb::MIX, 1.0f);
	em_verb2.setParameter(MVerb::EARLYMIX, 1.0f);
 
	setParameter(kPreDelay, 0.0f);
	setParameter(kDecay, 0.5f);
	setParameter(kLowPass, 1.0f);
	setParameter(kHighPass,0.0f);
	setParameter(kStereoIn,0.0f);
	setParameter(kDeviate,0.0f);
	setParameter(kEchoVol,0.5f);
	setParameter(kEchoTime,0.2f);
	setParameter(kFeedBk,0.31f);
	setParameter(kFilter,1.0f);
	setParameter(kFiltTyp,0.0f);
	setParameter(kPanOut,0.5f);
 	setParameter(kLevel,0.5f);
	setParameter(kMix,0.5f);
  bLoading = false;
}

//---------------------------------------------------------------------------------------
void WsEffect::suspend()
{
	Reset();
}

//---------------------------------------------------------------------------------------
WsEffect::~WsEffect()
{
}

//---------------------------------------------------------------------------------------
void WsEffect::close()
{
  delete Delay1;
  delete Delay2;
  delete Filter1;
  delete Filter2;
  //delete[] Blank;
	delete[] bufoutputs;
	delete HighFilter[0];
	delete HighFilter[1];
}

//---------------------------------------------------------------------------------------
void WsEffect::Update()
{
	float value = PreDelay;
	em_verb.setParameter(MVerb::PREDELAY,value); 

	value = min(1.0f,PreDelay+(Deviation*0.1f));
	em_verb2.setParameter(MVerb::PREDELAY,value); 

	value = PreDecay;
	em_verb.setParameter(MVerb::SIZE,(value+0.37f)*0.729f);
	em_verb.setParameter(MVerb::DECAY,value); 

	value = min(1.0f,PreDecay+(Deviation*0.2f));
	em_verb2.setParameter(MVerb::SIZE,(value+0.37f)*0.729f);
	em_verb2.setParameter(MVerb::DECAY,value); 
}

//---------------------------------------------------------------------------------------
void WsEffect::setParameter(VstInt32 index, float value)
{
	if (index < kNumParams) fParameter[index] = value;
	float xvalue = 0;
	float value1 = 0;
	float value2 = 0;
	switch (index) {
		case kPreDelay:
			PreDelay = value;
			Update();
			break;
		case kDecay:
			PreDecay = value;
			Update();
			break;
		case kLowPass:
			em_verb.setParameter(MVerb::BANDWIDTHFREQ,(value+0.06f)*0.94f); 
			em_verb2.setParameter(MVerb::BANDWIDTHFREQ,(value+0.06f)*0.94f); 
			break;
		case kHighPass:
			bHigh = (value > 0.00001f);
			if (bHigh) {
				HighFilter[0]->setFreq(value);
				HighFilter[1]->setFreq(value);
			}
			break;
		case kStereoIn:
			Stereo = (value > 0.001f);
			break;
		case kDeviate:
			Deviation = value*value;
			Update();
			break;
		case kEchoVol:
			EchoVolume = value*value;
			break;
		case kEchoTime:
			xvalue = value;
			value1 = 0;
			value2 = 0;
			if (xvalue > 0.5f) {
				value1 = 1.0f;
				value2 = (xvalue - 0.5f) * 2.0f;
				value2 *= value2;
				value2 = float (int( (value2 * (128.0f*2.0f)) + 16.0f ));
			}
			else {
				switch (int((xvalue * 2.0f) * 20.0f)) {
					case  0: value1 = 7; value2 = 8;	break;
					case  1: value1 = 3; value2 = 8;	break;
					case  2: value1 = 1; value2 = 3;	break;
					case  3: value1 = 3; value2 =16;	break;
					case  4: value1 = 5; value2 = 8;	break;
					case  5: value1 = 4; value2 = 4;	break;
					case  6: value1 = 3; value2 = 4;	break;
					case  7: value1 = 2; value2 = 4;	break;
					case  8: value1 = 1; value2 = 4;	break;
					case  9: value1 = 1; value2 = 5;	break;
					case 10: value1 = 1; value2 = 6;	break;
					case 11: value1 = 1; value2 = 7;	break;
					case 12: value1 = 1; value2 = 8;	break;
					case 13: value1 = 1; value2 = 9;	break;
					case 14: value1 = 1; value2 =10;	break;
					case 15: value1 = 1; value2 =11;	break;
					case 16: value1 = 1; value2 =12;	break;
					case 17: value1 = 1; value2 =13;	break;
					case 18: value1 = 1; value2 =14;	break;
					case 19: value1 = 1; value2 =15;	break;
					case 20: value1 = 1; value2 =16;	break;
				}
			}
			sprintf(xEDText,"%d/%d",int(value1),int(value2));
			setDelay( (float(value1/value2) * (240.0f/float(BPM))) / 3.0f, false );
			break;
		case kFeedBk:
			FeedBack = value * 0.96f;
			break;
		case kFilter:
			Filter1->setFreq(value);
			Filter2->setFreq(value);
			break;
		case kFiltTyp:
		    if( value) {
			  FilterHigh = true;
			}
			else {
			  FilterHigh = false;
			}
			if (FilterHigh) {
				Filter1->setType(1);
				Filter2->setType(1);
			}
			else {
				Filter1->setType(0);
				Filter2->setType(0);
			}
			break;
		case kPanOut:
			Pan[0] = min(1.0f,(1.0f-value)*2.0f);
			Pan[1] = min(1.0f,value*2.0f);
			break;
		case kLevel:
			em_verb.setParameter(MVerb::GAIN, value);
			em_verb2.setParameter(MVerb::GAIN, value);
			break;
		case kMix:
			if (value < 0.5f)       { Wet = value*2.0f;   Dry = 1.0f; }
			else if (value == 0.5f) { Wet = 1.0f;         Dry = 1.0f; }
			else                    { Wet = 1.0f;         Dry = (1.0f-value)*2.0f; }
			break;
	} //switch(index)
}

//---------------------------------------------------------------------------------------
void WsEffect::Reset()
{
  //if (bLoading) return;
	em_verb.setSampleRate(sampleRate);    setParameter(kDecay,fParameter[kDecay]); 
	em_verb2.setSampleRate(sampleRate);   setParameter(kDecay,fParameter[kDecay]);
	HighFilter[0]->updateSampleRate(sampleRate);    HighFilter[0]->init();
	HighFilter[1]->updateSampleRate(sampleRate);    HighFilter[1]->init();

	Delay1->clear();
  Delay2->clear();
  bFeedBack[0] = bFeedBack[1] = 0.0f;

  Filter1->init();
  Filter2->init();
}

//---------------------------------------------------------------------------------------
void WsEffect::setSampleRate(float sampleRate)
{
	if (sampleRate == this->sampleRate) return;
  this->sampleRate = sampleRate;
  bLoading = true;

	if (Delay1) {
	  Ammount = int(sampleRate*3.0f); // 3 Seconds
	  if (Ammount <= 0) Ammount = 100;
	  Delay = float(Ammount-1);
	  Delay1->newDelay(Ammount);
	  Delay2->newDelay(Ammount);
  }

  Reset();

	bLoading = false;
	for (int x=0; x<kNumParams; x++) setParameter(x,fParameter[x]);
}

//---------------------------------------------------------------------------------------
void WsEffect::setBPM(float BPM)
{
	if (BPM == this->BPM) return;
	this->BPM = BPM;
	setParameter(kEchoTime,fParameter[kEchoTime]);
}

//---------------------------------------------------------------------------------------
void WsEffect::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
  float* in1  =  inputs[0];
  float* in2  =  inputs[1];
  float* out1 = outputs[0];
  float* out2 = outputs[1];

	for (int i=0; i<sampleFrames; i++) { ibuf[0][i] = in1[i];  ibuf[1][i] = in2[i]; }

	//if (sampleFrames > BlankSize) {
	//	BlankSize = sampleFrames;
	//	delete[] Blank;
	//	Blank = new float[BlankSize];
	//}

	xSampFrames = sampleFrames;

	if (EchoVolume > 0.001f) {
		for (int i=0; i<sampleFrames; i++) {
			Signal[0] = in1[i];
			Signal[1] = in2[i];

			bFeedBack[0] = Delay1->tickraw(Filter1->tick(Signal[0]+(bFeedBack[0]*FeedBack*((FilterHigh) ? 0.8f : 1.0f))));
			bFeedBack[1] = Delay2->tickraw(Filter2->tick(Signal[1]+(bFeedBack[0]*FeedBack*((FilterHigh) ? 0.8f : 1.0f))));

			denormal(bFeedBack[0]);
			denormal(bFeedBack[1]);

			out1[i] = in1[i] + bFeedBack[0] * EchoVolume;
			out2[i] = in2[i] + bFeedBack[1] * EchoVolume;
		}
		sampleFrames = xSampFrames;
	}
	else {
		for (int i=0; i<sampleFrames; i++) { out1[i] = in1[i];   out2[i] = in2[i]; }
	}

	//if (Stereo) {
	//	bufoutputs[0] = outputs[0];
	//	bufoutputs[1] = Blank;
	//	em_verb.processReplacing(outputs, bufoutputs, sampleFrames);

	//	bufoutputs[0] = Blank;
	//	bufoutputs[1] = outputs[1];
	//	em_verb2.processReplacing(outputs, bufoutputs, sampleFrames);
	//}
	//else {
		bufoutputs[0] = outputs[0];
		bufoutputs[1] = outputs[1];
		em_verb.processReplacing(outputs, bufoutputs, sampleFrames);
	//}

  out1 = outputs[0];
  out2 = outputs[1];

	for (int i=0; i<sampleFrames; i++) {
		if (bHigh) {
			Signal[0] = HighFilter[0]->tickhigh(out1[i]);
			Signal[1] = HighFilter[1]->tickhigh(out2[i]);
		}
		else {
			Signal[0] = out1[i];
			Signal[1] = out2[i];
		}
		out1[i] = ibuf[0][i] * Dry  +  Signal[0] * Pan[0] * Wet;
		out2[i] = ibuf[1][i] * Dry  +  Signal[1] * Pan[1] * Wet;
   }
}

//---------------------------------------------------------------------------------------
void WsEffect::setDelay(float aDelay, bool doforce)
{
	if (!doforce && PrevDelay == aDelay) return;
	PrevDelay = aDelay;

	if (aDelay < 0.000001f) aDelay = 0.000001f;
	if (aDelay > 1.0f) aDelay = 1.0f;
	liquidDelay = aDelay;
	Delay = aDelay*Ammount;

	Delay1->setDelay(Delay);
	Delay2->setDelay(Delay);
}

//---------------------------------------------------------------------------------------
void WsEffect::getParameterDisplay(VstInt32 index, char* text)
{
	switch (index) {
		case kEchoTime:
			strncpy(text, xEDText, kVstMaxParamStrLen);
			return;
		case kFiltTyp:
			if (FilterHigh) strncpy(text,"High",kVstMaxParamStrLen); 
			           else strncpy(text,"Low",kVstMaxParamStrLen);	
			return;
	}
	if (index < kNumParams) 
	  strncpy( text , std::to_string((long double)fParameter[index]).c_str() , kVstMaxParamStrLen); 
   else vst_strncpy(text, "    ", kVstMaxParamStrLen);
}

