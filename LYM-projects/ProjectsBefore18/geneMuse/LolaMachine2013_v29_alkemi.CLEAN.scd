

(
Server.default.options.inDevice_("ASIO : Fast Track Pro ASIO")
Server.default.options.outDevice_("ASIO : Fast Track Pro ASIO")
s.boot;
)

(

// OSC messages receivers
d=();

// emission of messages made with a dictionary
d.pd = NetAddr.new("127.0.0.1", 1979); // create the NetAddr
d.pd.sendMsg("/flash"); // send message

// emission of messages made with a dictionary
d.virchor = NetAddr.new("192.168.0.2", 1978); // create the NetAddr
d.virchor.sendMsg("/flash"); // send message


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////variables to be evaluated first/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

~volumeMatch = 1.0;
~synthMatch1;
~synthMatch2;
~synthMatch3;
~synthMatch4;
~buffertorden;
~buffertordenOn = 0;
~synthbuffertorden;
~match1;
~match2;
~match3;
~match4;
~delayroom;
~weav;
~matchesloop;
~dinosteps;
~volumedino;
~timeOutMatches;
~timeOutDino;
~buffercrush;
~buffercrushOn;
~loopNumber = 0;

// Sequencer
~baandlydOn = 0.0;
~klikkelilikOn = 0.0;
~softstepfilterOn = 0.0;
~baglaens_delayOn = 0.0;
~kikP2de = 0.0;
~dumpstortromme = 0.0;
~kikP2deOn = 0.0;
~snareP3de = 0.0;
~sjovtromme = 0.0;
~snareP3deOn = 0.0;
~newdrumPde = 0.0;
~vandfosstromme = 0.0;
~newdrumPdeOn = 0.0;
~kikP3de = 0.0;
~voldsomstortromme = 0.0;
~kikP3deOn = 0.0;
~snarePde = 0.0;
~snarePdeOn = 0.0;
~armbolt1 = 0.0;
~snareP4de = 0.0;
~snareP4deOn = 0.0;
~armbolt2 = 0.0;
~hhP = 0.0;
~hhPOn = 0.0;
~hhP1 = 0.0;
~hhP1On = 0.0;
~snareP = 0.0;
~snarePOn = 0.0;
~snareP4 = 0.0;
~snareP4On = 0.0;
~snareP2fp = 0.0;
~snareP2fpOn = 0.0;
~cstykke = 0.0;
~cstykkeOn = 0.0;
~cstyk = 0.0;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//SynthDefs////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Delay + Room

SynthDef(\delay_reverb, { arg volume = 0.0 , maxdtime = 20, 
		ddecay = 20, delay = 2, mix = 0.02 , room = 0.02;
	var source;
	source = SoundIn.ar([0,1]) * volume;

	// delay
	source = CombN.ar( source , maxdtime , delay, ddecay )  + source ;
	
	// reverb
	source = FreeVerb.ar( source, mix, room) + source * 0.4 ; 

	Out.ar( [0,1], source );
}).send(s);	

/////////////////////////////////////////////////////////////////////////////////////
// Weav


SynthDef(\weav, { arg volume = 0.3 ;
	var source;
	source = Mix.ar(
	Pan2.ar(
		SinOsc.ar(
			[60, 62, 63, 65, 67, 68, 71, 72].midicps,
			mul: LFNoise2.kr(PinkNoise.kr(rrand(0.3, 0.4).dup(8), 0.5, 0.5), volume)
		),
		1.0.rand2.dup(8)
	)
)*0.3;
	Out.ar( [0,1], source );
}).send(s);


/////////////////////////////////////////////////////////////////////////////////
// DINO
SynthDef(\mybell,
{
	arg freq_bell = 440, thisDur = 1/10, volumedino = 0.5;
	var delay, trig;
	
	var bell = Mix.fill(12,
	{
		arg counter;
		var aenv, fenv, bell_duration;
		bell_duration = thisDur * rrand(1.0,2.0);
		aenv = EnvGen.kr(Env.perc(0, 1), 
			1, timeScale: bell_duration, doneAction: 2)/(counter+1);
		fenv = EnvGen.kr(Env.adsr(0, 0, 0.8, TRand.kr(0.2, 0.4, 1)),
			1, timeScale: bell_duration, doneAction: 2);
		Pan2.ar(
			SinOsc.ar(
				// the frequency has an enveloppe that makes it sound higher pitched at the end of its resonance
				freq_bell*(counter+1) * fenv * GrayNoise.kr(1/bell_duration,0.05,0.95) , 
				mul: aenv),
				//each freq has its own envelope for frequency and amplitude
			1.0.rand2
		);

        })*volumedino;

	Out.ar(0,  bell);
}).send(s);



~taskPlaying = 0;
~counter = 0;
// z = Synth(\bell);
~dinosteps = Task({
	var next_bell_freq = 40 , theTempo = 3,  frequencyFlat =400;
	var next = 0, melody, cadence;
	melody = [-5, 7, -3, 4, -2, -1];
	cadence = theTempo * [1, 1, 1, 2, 2, 2];
	block {|break|
	    {
		var duration = theTempo * rrand(1,2), //cadence.wrapAt(next), 
			index = rrand(1,4), 
			change = rrand(-2,2);

		Synth(\mybell, [\thisDur, duration , 
				\freq_bell, next_bell_freq.midicps.round(0.01) ,
				\volumedino , ~volumedino]);
		// d.pd.sendMsg("/bell "+next); // send message
		d.virchor.sendMsg("/flashdino "); // send message
		"flashdino".postln;
		next = next + 1;
		next_bell_freq = next_bell_freq + melody.wrapAt(next);
		duration.wait;
		~counter = ~counter + 1;
		~counter.postln;
		if( ~counter % 3 == 0 ) {
			theTempo = theTempo / 1.00;
  		};
		(Clock.seconds-~timeOutDino).postln;
		if( (Clock.seconds-~timeOutDino) > 120 ) {
   				break.value(120);
		}
    		// melody = melody.reverse;
		// melody.put(index,(melody.at(index))+change);
		// melody.put(0,(melody.at(0))-change);
	    }.loop;
	};
});


//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       OSC RESONDERS
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

// Delay
OSCresponder.new(nil, "/PDdelay", 
	{ 
		arg time, resp, msg; 
		var delayValue;
		// time.postln;
		// resp.postln;
		msg.postln; 
		// ((msg[1]+127.0)/255.0).postln;
		// msg[1].postln; 

		if( msg[2] > 0.5 ) {
			// delayValue = msg[1];
			delayValue = 0.3;
		}
		{
			delayValue = 0;
		};		


		//if( msg[1] > 0.1 ) {
		//	delayValue = msg[1];
		//}
		//{
		//	delayValue = 0;
		//};

		// x.set(\delay, (msg[1]+127.0)/511.0 );
		// v  = msg[4];
		~delayroom.set(\delay, delayValue );
	} 
).add;

/////////////////////////////////////////////////////////////////////////////////
// Room 
OSCresponder.new(nil, "/PDRoom", 
	{ 
		arg time, resp, msg; 
		var roomvalue;
		// time.postln;
		// resp.postln;
		msg.postln; 
		// ((msg[1]+127.0)/255.0).postln;
		msg[1].postln;
		
		if( msg[2] > 0.5 ) {
			roomvalue = msg[4];
		}
		{
			roomvalue = 0;
		};
		
		// x.set(\delay, (msg[1]+127.0)/511.0 );
		~delayroom.set(\room, roomvalue,  \mix, msg[4] );
	} 
).add;


/////////////////////////////////////////////////////////////////////////////////
//dinosteps vol
OSCresponder.new(nil, "/PDdino_vol", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		msg.postln; 		
		~volumedino =msg[1];
	} 
).add;

/////////////////////////////////////////////////////////////////////////////////
//backtrack vol
OSCresponder.new(nil, "/PDbt_vol", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		msg.postln;
		~synthbuffertorden.set(\volume, msg[1]*1);
	} 
).add;

/////////////////////////////////////////////////////////////////////////////////////
// Bt1 on
OSCresponder.new(nil, "/PDbt1_on", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		msg.postln; 

		~buffertordenOn = 1;
		~synthbuffertorden = Synth(\bufferBTDCont);
		~synthbuffertorden.set(\volume, 0);
		d.virchor.sendMsg('/launch'); // send message	
		w = Pchain(~baseP, ~drumlooper ).play(quant: 4);
	} 
).add;


/////////////////////////////////////////////////////////////////////////////////
// UDP test received from PD and sent to VirChor
OSCresponder.new(nil, "/testUDP_fromPD", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
                d.virchor.sendMsg('/testUDP'); // send message	
	} 
).add;

/////////////////////////////////////////////////////////////////////////////////
// UDP test received from VirChor
OSCresponder.new(nil, "/testUDP", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		"***********************************************". postln;
		"VIRCHOR UDP MESSAGE". postln;
		"***********************************************". postln;
	}
).add;

/////////////////////////////////////////////////////////////////////////////////////
// alvanoto on
// Delay on

OSCresponder.new(nil, "/PDdelayroom_on", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		msg.postln; 
		~delayroom= Synth(\delay_reverb, [\volume, 0.5 , \disto, 0.02 , \delay, 2, \maxdtime, 10, \ddecay, 20, \mix, 0.5, \room, 0.5]);
	} 
).add;


/////////////////////////////////////////////////////////////////////////////////////
// Dinosteps on
OSCresponder.new(nil, "/PDdinostep_on", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		msg.postln; 
		~dinosteps.play(SystemClock);
		~timeOutDino = Clock.seconds;
	} 
).add;




//alkemi backtrack
~buffertorden = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/alkemi_backtrack/alkemi_backtrack16.wav");
SynthDef(\bufferBTDCont, { arg out = 0, volume=0;
	Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*2*volume)
}).send(s);

//////////////////////////////////////////////////////////////////////////////////////
////////////////RESTARTS////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//Dance Macabre
//~buffertorden = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/alkemi_backtrack/RESTARTS/alkemi_restart.DanceMacabre.wav");
//SynthDef(\bufferBTDCont, { arg out = 0, volume=0;
//	Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*2*volume)
//}).send(s);

//C stykke
//~buffertorden = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/alkemi_backtrack/RESTARTS/alkemi_restart.Cstykke.wav");
//SynthDef(\bufferBTDCont, { arg out = 0, volume=0;
//	Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*2*volume)
//}).send(s);

//Quiet
//~buffertorden = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/alkemi_backtrack/RESTARTS/alkemi_restart.quiet.wav");
//SynthDef(\bufferBTDCont, { arg out = 0, volume=0;
//	Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*2*volume)
//}).send(s);





//===========================================================================
     //                         BUFFERS
//===========================================================================
~buffers = [
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/sample3.wav",//2 //0
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/sample5.wav",//3 // 1
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/cubase_instr_pling.wav",//5 // 2
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/barn_effect_11.wav",//6 // 3
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/barn_klung.wav",//8 //4
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/barn_effect_2.wav",//12 //5
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/barn_11.wav",//14 //6
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/barn_14.wav",//15 // 7
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/elektriskstoej.wav",//16 // 8
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/barn_klung.wav",//21 // 9
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/storm1.wav",//22 // 10
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/kniv.wav",//24 // 11
        //===================================================================================
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/storm1.wav",//18 // 12
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/baglaens.wav",//21 // 13
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/hammer3.wav",//23 // 14
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/hammer2_pling.wav",//24 // 15
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/tromme-distortion-delay.wav",//26 // 16
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/draabe.wav",//28 // 17
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/tromme-transformer.wav",//30 // 18
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/stortromme-dump.wav",//31 // 19
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/tromme-falling.wav",//32 // 20
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/empty.wav",//00 // 21
        //===================================================================================
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/barn_baandlyd.wav", // 00 //22
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/soft_stepfilter_ned2.wav", // 00 // 23
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/trappetrin1.wav", // 00 // 24
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/trappetrin2.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/morter.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/plik.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/morter2_132.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/crushing.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/crushing2.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/crushing3.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/bitcrush_stortromme.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/klikkelilik3_132.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/baglaens_delay3_132.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/c-stykke_alkemi.v11.wav",
        "C:/Users/Boe/Desktop/SC/RythmProject/testsamples/barn_pivehvin.wav" //36
        


].collect { |path| Buffer.read(s, path) };

//James' instrument
SynthDef(\PlayBuf, { |start, time, bufnum, pan, rate = 1, amp ,
    attack = 0.001, decay = 0.02, outbus|
    var sig;
    sig = PlayBuf.ar(1, bufnum, rate * BufRateScale.kr(bufnum), 1, start, 0)
    * EnvGen.kr(Env.linen(attack, time, decay), doneAction:2);
   Out.ar(outbus, Pan2.ar(sig, pan, amp ));
}).add;


~baseP = Pbind(
    \instrument, \PlayBuf,
    \time, Pkey(\dur) / Pfunc { TempoClock.tempo }
);
//============================================================================================
//                                 LOOPER CONNECTED TO DRAWING MACHINE 
//============================================================================================

//~num = 0;
~drumlooper = Pbind(
    \dur, Pseq([0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5], inf),
    \amp, 1,
    \bufnum, ~buffers[21],
    \x, Pseq(#[1,2,3,4,5,6,7,8], inf),
    \begin, Pfunc {|ev| 
        // first beat (new sequence)
        //if(ev[\x]==1,
            //{
                 // increments sequence counter
                // ~num=~num+1;
                // send messages to VirChor with sequence counts            {
                d.virchor.sendMsg("/beat "+ev[\x]);
                (ev[\x]+~loopNumber).postln;
                if(ev[\x]==8) {
                	if(~loopNumber==0){~loopNumber = 8;}{~loopNumber = 0;};
                }{};
                // d.virchor.sendMsg("/flashTrack "+ev[\x]);
                // d.virchor.sendMsg("/flashCA "+ev[\x]);
                // d.virchor.sendMsg("/flashPart "+ev[\x]);           // },
           // {
                1;
           // })
    }
);


//============================================================================================
//               THE FALL 
//============================================================================================


~baandlyd = Pbind(
    \dur, Pseq([2,2], inf),
    \amp, Pseq([4, 4], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[26], ~buffers[26]] ),
        inf
)
);

~klikkelilik = Pbind(
    \dur, Pseq([2, 2], inf),
    \amp, Pseq([2, 2], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
        //Pseq([ ~buffers[8], ~buffers[8], ~buffers[8], ~buffers[8]] ),
      Pseq([ ~buffers[33], ~buffers[33]] ),
        inf
)
);

~softstepfilter = Pbind(
    \dur, Pseq([8], inf),
    \amp, Pseq([6], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[23]] ),
        inf
)
);
~stortromme = Pbind(
    \dur, Pseq([1, 1, 1,1], inf),
    \amp, Pseq([2,2,2,2], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
        //Pseq([ ~buffers[8], ~buffers[8], ~buffers[8], ~buffers[8]] ),
      Pseq([ ~buffers[31], ~buffers[31], ~buffers[31], ~buffers[31]] ),
        inf
)
);

~hhPde = Pbind(
     \dur, Pseq([0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25], inf),
    \amp, Pseq([2, 1, 2, 1,2, 1, 2, 1 ], inf),
    \bufnum,  ~buffers[27]
    
);



~baglaens_delay = Pbind(
    \dur, Pseq([16], inf),
    \amp, Pseq([3], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[34]] ),
        inf
)
);


// ===========================================================================================
//             DARK EARTH
//============================================================================================

~cstykke = Pbind(
    \dur, Pseq([2000], 1),
    \amp, Pseq([2.5], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[35]] ),
        inf
)
);
~kikP2de = Pbind(
     \dur, Pseq([1, 1, 1, 1], inf),
    \amp, Pseq([1, 0.8, 1, 0.8], inf),
    \bufnum,  ~buffers[19],
);

~kikP3de = Pbind(
    \dur, Pseq([8], inf),
    \amp, Pseq([1.5], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[20]] ),
        inf
)
);

~hhPde = Pbind(
     \dur, Pseq([0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25], inf),
    \amp, Pseq([1, 0.5, 1, 0.5 ,1, 0.5, 1, 0.5 ], inf),
    \bufnum,  ~buffers[12]
    
);
~newdrumPde = Pbind(
    \dur, Pseq([4], inf),
    \amp, 1,
    \bufnum, ~buffers[16]
);


~snarePde = Pbind(
   \dur, Pseq([0.5, 0.25, 0.5, 0.5, 0.25, 1, 0.25, 0.5, 0.25 ], inf),
    \amp, Pseq([0.2, 0.1, 0.3, 0.2, 0.1, 0.3, 0.2, 0.1, 0.2], inf),
    \bufnum,  ~buffers[15]
);

~snareP4de = Pbind(
   \dur, Pseq([ 0.75, 0.25, 0.25, 0.5, 0.5, 0.25, 0.25, 0.5, 0.25, 0.25, 0.25 ], inf),
    \amp, Pseq([1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1], inf),
    \bufnum,  ~buffers[14]
);

~snareP2de = Pbind(
    \dur, Pseq([Rest (1), 1, Rest (2)], inf), 
    \amp, 1,
    \bufnum, ~buffers[13]
);

    ~snareP3de = Pbind(
    \dur, Pseq([Rest(2), 1, Rest(1)], inf), 
    \amp, 1,
    \bufnum, ~buffers[18]
);


//===========================================================================================
//                         TO BE QUIET
//===========================================================================================

~baseP = Pbind(
    \instrument, \PlayBuf,
    \time, Pkey(\dur) / Pfunc { TempoClock.tempo }
);


	~hhP2 = Pbind(
     \dur, Pseq([0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25], inf),
    \amp, Pseq([1, 0.5, 1, 0.5 ,1, 0.5, 1, 0.5 ], inf),
    //\volumedrum, ~volumedrumvariable,
    \bufnum,  ~buffers[10]
    
);
	

~kikP = Pbind(
    \dur, Pseq([1, 1, 1,1], inf),
    \amp, Pseq([6, 6, 2, 6], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
        //Pseq([ ~buffers[8], ~buffers[8], ~buffers[8], ~buffers[8]] ),
      Pseq([ ~buffers[9], ~buffers[9], ~buffers[9], ~buffers[9]] ),
      
        inf
)
);


~hhP = Pbind(
     \dur, Pseq([0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25], inf),
    \amp, Pseq([6, 10, 6, 10 ,6, 10, 6, 10  ], inf),
    \bufnum,  ~buffers[8],
    \x, Pseq(#[1,2,3,4,5,6,7,8], inf),
);

~newdrumP = Pbind(
    \dur, Pseq([1, Rest (3)], inf),
    \amp, 1,
    \bufnum, ~buffers[5]
);


~snareP = Pbind(
    \dur, Pseq([1,Rest (7)], inf), 
    \amp, 1,
    \bufnum, ~buffers[36]
);


~snareP3 = Pbind(
    \dur, Pseq([1, Rest(1.5), 1, 1, Rest(3.5)], inf),
    //\dur, Pseq([Rest(0.5), 1, Rest(2.5)], inf), 
    \amp, 1,
    \bufnum, ~buffers[11]
);
~snareP4 = Pbind(
    \dur, Pseq([Rest(1.5), 0.75, 1, Rest (4.75)], inf),
    //\dur, Pseq([Rest(0.5), 1, Rest(2.5)], inf), 
    \amp, 1,
    \bufnum, ~buffers[9]
);

~snareP2fp = Pbind(
    \dur, Pseq([Rest(0.5), 15.5], inf),
    \amp, 0.3,
    \bufnum, ~buffers[2]
);



~hhP1 = Pbind(
     \dur, Pseq([0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25], inf),
    \amp, Pseq([0.3, 0.7, 0.3, 0.7,0.3, 0.7, 0.3, 0.7], inf),
    \bufnum,  ~buffers[6]
);






//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       OSC RESONDERS
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888



//================================================================================================
//                        RYTME 1 on
//================================================================================================
OSCresponder.new(nil, "/Fallingman_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

////////////////Baandlyd (morter) ////////////////////////////////////
        if( (msg[3] > 0.5) && (~baandlydOn == 0.0) , {
            a = Pchain(~baseP, ~baandlyd).play(quant: 4);
            ~baandlydOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~baandlydOn == 1.0) , {
            a.stop;
            ~baandlydOn = 0.0;
        });

        //////////////// Klikkeliklik ////////////////////////////////////
        if( (msg[2] > 0.5) && (~klikkelilikOn == 0.0) , {
            f = Pchain(~baseP, ~klikkelilik).play(quant: 4);
            ~klikkelilikOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~klikkelilikOn == 1.0) , {
            f.stop;
            ~klikkelilikOn = 0.0;
        });

//////////////// Softstepfilter ////////////////////////////////////
        if( (msg[1] > 0.5) && (~softstepfilterOn == 0.0) , {
            b = Pchain(~baseP, ~softstepfilter).play(quant: 4);
            ~softstepfilterOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~softstepfilterOn == 1.0) , {
            b.stop;
            ~softstepfilterOn = 0.0;
        });

//////////////// baglaens_delay ////////////////////////////////////
        if( (msg[4] > 0.5) && (~baglaens_delayOn == 0.0) , {
            u = Pchain(~baseP, ~baglaens_delay).play(quant: 4);
            ~baglaens_delayOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~baglaens_delayOn == 1.0) , {
            u.stop;
            ~baglaens_delayOn = 0.0;
        });
            } 
).add;





//================================================================================================
//                        RYTME 2 on
//================================================================================================
OSCresponder.new(nil, "/Darkearth_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

//////////////// Dumpstortromme ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~kikP2deOn == 0.0) , {
        	msg[3].postln;
            ~dumpstortromme = Pchain(~baseP, ~kikP2de).play(quant: 4);
            ~kikP2deOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~kikP2deOn == 1.0) , {
            ~dumpstortromme.stop;
            ~kikP2deOn = 0.0;
        });

        //////////////// Sjovtromme ////////////////////////////////////
        if( (msg[2] > 0.5) && (~snareP3deOn == 0.0) , {
            ~sjovtromme = Pchain(~baseP, ~snareP3de).play(quant: 4);
            ~snareP3deOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~snareP3deOn == 1.0) , {
            ~sjovtromme.stop;
            ~snareP3deOn = 0.0;
        });

//////////////// Vandfosstromme ////////////////////////////////////
        if( (msg[1] > 0.5) && (~newdrumPdeOn == 0.0) , {
            ~vandfosstromme = Pchain(~baseP, ~newdrumPde).play(quant: 4);
            ~newdrumPdeOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~newdrumPdeOn == 1.0) , {
            ~vandfosstromme.stop;
            ~newdrumPdeOn = 0.0;
        });

//////////////// Voldsomstortromme ////////////////////////////////////
        if( (msg[4] > 0.5) && (~kikP3deOn == 0.0) , {
            ~voldsomstortromme = Pchain(~baseP, ~kikP3de).play(quant: 4);
            ~kikP3deOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~kikP3deOn == 1.0) , {
            ~voldsomstortromme.stop;
            ~kikP3deOn = 0.0;
        });
            } 
).add;


//================================================================================================
//                        RYTME 3 on
//================================================================================================
OSCresponder.new(nil, "/Quiet1_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

//////////////// Armbolt1 ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~snarePdeOn == 0.0) , {
        	msg[3].postln;
            ~armbolt1 = Pchain(~baseP, ~snarePde).play(quant: 4);
            ~snarePdeOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~snarePdeOn == 1.0) , {
            ~armbolt1.stop;
            ~snarePdeOn = 0.0;
        });

        //////////////// Armbolt2 ////////////////////////////////////
        if( (msg[2] > 0.5) && (~snareP4deOn == 0.0) , {
            ~armbolt2 = Pchain(~baseP, ~snareP4de).play(quant: 4);
            ~snareP4deOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~snareP4deOn == 1.0) , {
            ~armbolt2.stop;
            ~snareP4deOn = 0.0;
        });

///////////////st.st.st.st.st.st.st////////////////////////////////
        if( (msg[1] > 0.5) && (~hhPOn == 0.0) , {
            r = Pchain(~baseP, ~hhP).play(quant: 4);
            ~hhPOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~hhPOn == 1.0) , {
            r.stop;
            ~hhPOn = 0.0;
        });
        //////////dk.dk.dk.dk.dk.dk///////////////////////////////////
        if( (msg[4] > 0.5) && (~hhP1On == 0.0) , {
            h = Pchain(~baseP, ~hhP1).play(quant: 4);
            ~hhP1On = 1.0;
        });

        if( (msg[4] < 0.5) && (~hhP1On == 1.0) , {
            h.stop;
            ~hhP1On = 0.0;
        });

            } 
).add;


//================================================================================================
//                        RYTME 4 on
//================================================================================================
OSCresponder.new(nil, "/Quiet2_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

//////////////// piv paa 1-slaget ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~snarePOn == 0.0) , {
        	msg[3].postln;
            q = Pchain(~baseP, ~snareP).play(quant: 4);
            ~snarePOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~snarePOn == 1.0) , {
            q.stop;
            ~snarePOn = 0.0;
        });

        //////////////// KlapKlap ////////////////////////////////////
        if( (msg[2] > 0.5) && (~snareP4On == 0.0) , {
            m = Pchain(~baseP, ~snareP4).play(quant: 4);
            ~snareP4On = 1.0;
        });

        if( (msg[2] < 0.5) && (~snareP4On == 1.0) , {
            m.stop;
            ~snareP4On = 0.0;
        });
        //////////////// C-STYKKE  ////////////////////////////////////
        if( (msg[1] > 0.5) && (~cstykkeOn == 0.0) , {
            ~cstyk = Pchain(~baseP, ~cstykke).play(quant: 4);
            ~cstykkeOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~cstykkeOn == 1.0) , {
            ~cstyk.stop;
            ~cstykkeOn = 0.0;
        });

///////// StjernePLING //////////////////////////////////////////
if( (msg[4] > 0.5) && (~snareP2fpOn == 0.0) , {
            k = Pchain(~baseP, ~snareP2fp).play(quant: 4);
            ~snareP2fpOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~snareP2fpOn == 1.0) , {
            k.stop;
            ~snareP2fpOn = 0.0;
        });

            } 
).add;
)


//==================================================================================================
//                        TEMPOCLOCK
//==================================================================================================

//TempoClock.tempo = 50/50; //denne svare til 120 tempo paa min loopstation
//TempoClock.tempo = 100/50; //denne svare til 120 tempo paa min loopstation dobbelt tempo end 50/50
TempoClock.tempo = 55/50;
TempoClock.tempo = 50/50;


//==================================================================================================

