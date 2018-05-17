/*
© 2004 Wusik.com - Williamk
*/
#if !defined(__DELAYL_H)
#define __DELAYL_H

//======================================================================================================
class DelayL
{
public:
  void clear(void);

  //! Overloaded constructor which specifies the current and maximum delay-line lengths.
  
  DelayL(float theDelay, long maxDelay);

  //! Class destructor.
  ~DelayL();

  //! Set the delay-line length.
  /*!
    The valid range for \e theDelay is from 0 to the maximum delay-line length.
  */
  void setDelay(float theDelay);

  //! Return the current delay-line length.
  float getDelay(void) const;

  void setModulated(bool aModulated = false);

  //! Return the value which will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  float nextOut(void);
  float nextOutA(void);

  //! Input one sample to the delay-line and return one output.
  //float tick(float sample);
  //float tickmodidleA(float sample);
  float tickraw(float sample);
  //float tickmodidle(float sample);
  float tickmod(float sample);
  //float tickrawidle(float sample);
  //float tickcubicidle(float sample);
  bool notIdle(void);
  bool Idle(void);
 
  void newDelay(long maxDelay);

  void setNotIdle (void);

protected:  
  float alpha;
  float omAlpha;
  float nextOutput;
  bool doNextOut;
  long inPoint;
  long outPoint;
  long length;
  float delay;
  float gain;
  float *outputs;
  float *inputs;
  bool Modulated;
  int gate;
  bool Loading;
  float coeff;
  float apInput;
};

#endif
