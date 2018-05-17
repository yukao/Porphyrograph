/*
   Wusik Effect V4 (c) Wusik Dot Com - WilliamK
*/
#ifndef __wseffect__
#define __wseffect__


#define denormal(sm) if(((*(unsigned int*)&sm)&0x7f800000)==0) sm=0.0f

#define _VSTID 'W#R4'
const char EffectName[kVstMaxEffectNameLen] = "MVerb Echo";

enum { // NOTE maximum 13 parameters allowed for WusikStation !!
	kPreDelay=0,
	kDecay,
	kLowPass,
    kHighPass,
	kStereoIn,
	kDeviate,
	kEchoVol,
	kEchoTime,
    kFeedBk,
	kFilter,
	kFiltTyp,
	kPanOut,
	kLevel,
	kMix,
  kNumParams
};
//======================================================================================================
class WsEffect 
{
public:
	WsEffect(void);
	~WsEffect();
	void Init(void);
	void setSampleRate(float sampleRate);
	void setBPM(float BPM);
	void suspend();
    void Update(void);
	void Reset(void);
	void setDelay(float aDelay, bool doforce);

	// Processing
	void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);

	// Parameters
	void setParameter (VstInt32 index, float value);
	void getParameterDisplay (VstInt32 index, char* text);

	void close ();
	bool Stereo;
    bool FilterHigh;

protected:
	static char *getVstPath(void);
	char programName[kVstMaxProgNameLen + 1];
	float fParameter[kNumParams];
	char cParameterName[kNumParams][kVstMaxParamStrLen];
	float sampleRate;

	AEffect* ptrPlug;
	AEffect* ptrPlug2;
	MVerb<float> em_verb;
	MVerb<float> em_verb2;

	bool bHigh;
	FilterOneP* HighFilter[2];

	float PrevCopy[2];



  float Delay;
  float liquidDelay;
  float DelayLevel;
  float FeedBack;
  float PrevDelay;
  int Ammount;
  float bInput[2];
  float bFeedBack[2];
  DelayL *Delay1;
  DelayL *Delay2;
  FilterOneP *Filter1;
  FilterOneP *Filter2;
  int counter;
  bool bLoading;

  float EchoVolume;

  float BPM;

  float Pan[2];
  float Deviation;

  float PreDecay;
  float PreDelay;

  char xEDText[99];

  float Signal[2];
  int xSampFrames;

	//float** ptrInputBuffers;
	float** bufoutputs; //float** ptrOutputBuffers;

	float ibuf[2][2048];

	float Wet;
	float Dry;

	//float *Blank;
	//int BlankSize;
};

#endif
