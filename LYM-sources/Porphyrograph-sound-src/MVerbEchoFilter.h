/*
© 2004 Wusik.com - Williamk
*/
#if !defined(__FILTER_H)
#define __FILTER_H          

#define denormal(sm) if(((*(unsigned int*)&sm)&0x7f800000)==0) sm=0.0f

//======================================================================================================
// 2 Pole Multi Filter //
class Filter
{
public: 
  Filter();

  float tick(float Input);
  float tickBB(float Input, float BB); 
  float ticklow(float Input);

  void setFreq(float aFrequency = 0);
  void setPureFreq(float aFrequency = 0);

  void updateSampleRate(float aSampleRate);

  void setRezo(float aRezonance = 0);

  void setPole(int aPole = 4);
  
  void setType(int aType = 0);

  float Glider(float Desired = 0);
  float Glider2(float Desired = 0);
  void setSmooth (bool aSmooth = true);

  void init();

protected:
  // Initialize class variables.
  float dBuffer[7];
  float Frequency;
  float Rezonance;
  float Samplerate;
  float Glide;
  float Gain;
  float Pre;
  int Pole;
  int Type;
  float HighOut, LowOut, BandOut, Out;
  float HighOut2, LowOut2, BandOut2, Out2; 
  bool Smooth;
  float currentSampleRate;
};

//======================================================================================================
// One Pole Simpler Filter //
class FilterOneP
{
public:
  FilterOneP();

  float tick(float Input);
  float ticklow(float Input);
  float tickhigh(float Input);

  void setFreq( float aFrequency = 0);
  void setPureFreq( float aFrequency = 0);

  void setType( int aType = 0);
  void init( void );

  void updateSampleRate(float aSampleRate);

protected:
  // Initialize class variables.
  float dBuffer;
  float Frequency;
  float Samplerate;
  int Type;
  float Out, Pre, F2;
  float currentSampleRate;
};

//======================================================================================================
// All Pass Filter //
class FilterAllP
{
public:
  FilterAllP();
  ~FilterAllP();

  float tick(float Input);
  void sampleRateCheck(void);

  void setDelay( float aT = 0);
  void setGain ( float aG = 0);
  void updateSampleRate(float aSampleRate);

protected:
  long  SampleRate;
  long  Ammount;
  float Out;
  float dBuffer;
  DelayL *Delay;
  float G;
  float currentSampleRate;
};

#endif // defined(__Filter_H)
