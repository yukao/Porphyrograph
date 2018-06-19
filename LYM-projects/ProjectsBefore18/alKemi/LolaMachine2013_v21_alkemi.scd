
// OSC messages receivers //////////////////////////////////////////////////////////////
// inputs:

//Server.default.options.inDevice_("ASIO : Focusrite USB ASIO Driver")


// outputs:
// Server.default.options.outDevice_("MME : Speakers (Realtek High Definitiion)")
//Server.local.options.sampleRate = 48000.00;
//Server.local.options.sampleRate = 44100.000000;

(
Server.default.options.inDevice_("ASIO : Fast Track Pro ASIO")
Server.default.options.outDevice_("ASIO : Fast Track Pro ASIO")
s.boot;
)

(
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
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
~alvanotodisto;
~delayroom;
~weav;
~matchesloop;
~dinosteps;
~volumedino;
~timeOutMatches;
~timeOutDino;
~alvaNotoOn = 0;
~buffercrush;
~buffercrushOn;
~synthbuffercrush = 1;


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
//AlvaNoto + Disto
// shell command: alsa_in -dhw:Pro,1 -jftpro &
SynthDef(\alvanoto_disto, { arg volume = 0.0 , dust = 0.0 , disto = 0.08 ;
	var source, trig;
	source = SoundIn.ar([0]) * volume;

	//AlvaNoto effect
	trig = Dust.kr(dust);
	SendReply.kr(trig, '/flash', trig);
	source= SoundIn.ar(
		[0,1],
		mul: EnvGen.kr(Env.perc(0.01, 0.3), 
		trig)) ;
	source = CombN.ar(source)+ source ;
	 
		


	// double disto
	source = source.fold2(disto)*2 ;
	//{SoundIn.ar(0).fold2(0.08)*6 + SoundIn.ar(0)}.play;
	//{SoundIn.ar(0).clip2(0.2) + SoundIn.ar(0)}.play;
	
	Out.ar( [0,1], source );
}).send(s);


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


//////////////////////////mathces///////////////////////////////////////////////////////////////////

~match1 = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/dcontmatches/GOL_match1.wav");
~match2 = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/dcontmatches/GOL_match2.wav");
~match3 = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/dcontmatches/GOL_match3.wav");
~match4 = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/dcontmatches/GOL_match4.wav");


SynthDef(\bufferMatch1, { arg out = 0, volumeM = 1.5;
 Out.ar([0,1],PlayBuf.ar(1, ~match1, BufRateScale.kr(~match1), doneAction:2)*volumeM*3); 
}).send(s);

SynthDef(\bufferMatch2, { arg out = 0, volumeM = 1.5;
 Out.ar([0,1],PlayBuf.ar(1, ~match2, BufRateScale.kr(~match2), doneAction:2)*volumeM*3);
}).send(s);

SynthDef(\bufferMatch3, { arg out = 0, volumeM = 1.5;
Out.ar([0,1],PlayBuf.ar(1, ~match3, BufRateScale.kr(~match3), doneAction:2)*volumeM*3); 
}).send(s);

SynthDef(\bufferMatch4, { arg out = 0, volumeM = 1.5;
Out.ar([0,1],PlayBuf.ar(1, ~match4, BufRateScale.kr(~match4), doneAction:2)*volumeM*3);
}).send(s);




~matchesloop = Routine{
block {|break|
   loop{
	var choice;
	choice = 1.rrand(4);
	choice.postln;
	(Clock.seconds-~timeOutMatches).postln;
	d.virchor.sendMsg("/flashmatch "+~volumeMatch); // send message
	switch(choice,
		1, {~synthMatch1 = Synth(\bufferMatch1);~synthMatch1.set(\volumeM, ~volumeMatch);},
		2, {~synthMatch2 = Synth(\bufferMatch1);~synthMatch2.set(\volumeM, ~volumeMatch);},
		3, {~synthMatch3 = Synth(\bufferMatch1);~synthMatch3.set(\volumeM, ~volumeMatch);},
		4, {~synthMatch4 = Synth(\bufferMatch1);~synthMatch4.set(\volumeM, ~volumeMatch);},
	);
	5.0.rrand(10.0).wait;
	if( (Clock.seconds-~timeOutMatches) > 123 ) {
   		break.value(183);
	}
    };
}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// PD responders//////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// AlvaNoto + disto

OSCresponder.new(nil, "/PDAlvaNoto", 
	{ 
		arg time, resp, msg; 
		var dusttrig , distotrig ;

		msg.postln; 
		if( msg[4] > 0.5 ) {
			dusttrig = msg[1];
			~alvaNotoOn = 1;
		}
		{
			dusttrig = 0;
		};		

		if( msg[2] > 0.5 ) {
			distotrig = msg[3];
		}
		{
			distotrig = 0;
			~alvaNotoOn = 0;
		};		

		~alvanotodisto.set(\dust, dusttrig, \disto, distotrig);
	} 
).add;	



/////////////////////////////////////////////////////////////////////////////////
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
			delayValue = msg[1];
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
		v  = msg[4];
		~alvanotodisto.set(\volume, msg[4] );
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

///////////////////////////////////////////////////////////////////////////

//weav vol
OSCresponder.new(nil, "/PDweav_vol", 
	{ 
		arg time, resp, msg; 
		var weav, volume;
		// time.postln;
		// resp.postln;
		msg.postln; 
		

		if( msg[1] > 0.5 ) {
			volume = msg[2];
		}
		{
			volume = 0;
		};		
		volume.postln;
		~weav.set( \volume, volume );
	} 
).add;

/////////////////////////////////////////////////////////////////////////////////
// Matches_vol
OSCresponder.new(nil, "/PDmatch_vol", 
	{ 
		arg time, resp, msg; 
		var weav, volume;
		// time.postln;
		// resp.postln;
		msg.postln; 		
		~volumeMatch = msg[1]*3;
		("volume Match: "++~volumeMatch).postln;
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
	} 
).add;

/////////////////////////////////////////////////////////////////////////////////////
// Crush On
OSCresponder.new(nil, "/Crush_on", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		msg.postln; 

		~buffercrushOn = 1;
		~synthbuffercrush = Synth(\bufferCrushon);
		~synthbuffercrush.set(\volume, 1);
//                d.virchor.sendMsg('/launch'); // send message	
	} 
).add;

/////////////////////////////////////////////////////////////////////////////////////
// Matches on
OSCresponder.new(nil, "/PDmatches_on", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		msg.postln; 
		~matchesloop.play;
		~timeOutMatches = Clock.seconds;
	} 
).add;

/////////////////////////////////////////////////////////////////////////////////////
// Weav on

OSCresponder.new(nil, "/PDweav_on", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		msg.postln; 
		~weav = Synth(\weav, [\volume, 0.1]);
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
		~alvanotodisto= Synth(\alvanoto_disto, [\volume, 0.0 , \dust, 0.0, \disto, 0.08]);
                OSCpathResponder(s.addr, ['/flash', ~alvanotodisto.nodeID], { arg time, thisResponder, msg;
		        if( ~alvaNotoOn == 1 ) {
	 		    d.virchor.sendMsg('/flash'); // send message
			    "flash".postln;
			}
	        }).add;
		~delayroom= Synth(\delay_reverb, [\volume, 0.5 , \disto, 0.02 , \delay, 2, \maxdtime, 10, \ddecay, 20, \mix, 0.5, \room, 0.5], ~alvanotodisto, addAction:\addAfter);
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

/////////////////////////////////////////////////////////////////////////////////
////////////////backtrackplayer//////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//torden backtrack ~buffertorden
//~buffertorden = Buffer.read(s, "G:/lym-media/testsamples/torden-fadeinout.wav");
//SynthDef(\bufferBTDCont, { arg out = 0, volume=0;
//	Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*5*volume)
//}).send(s);


//alkemi backtrack
~buffertorden = Buffer.read(s, "C:/Users/Boe/Desktop/SC/LolaMachine/lolamachineNEW2012/bactrack/alkemi_backtrack/alkemi_backtrack7.wav");
SynthDef(\bufferBTDCont, { arg out = 0, volume=0;
	Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*2*volume)
}).send(s);

////////Crush////////////////////////////////////////////////////////////////////////////////
~buffercrush = Buffer.read(s, "G:/lym-media/testsamples/crush.wav");
SynthDef(\bufferCrushon, { arg out = 0, volume=0;
	Out.ar( [0,1],PlayBuf.ar(1, ~buffercrush, BufRateScale.kr(~buffercrush))*1*volume)
}).send(s);

)

(


//===========================================================================
     //                         BUFFERS
//===========================================================================
~buffers = [
        "G:/lym-media/testsamples/sample3.wav",//2 //0
        "G:/lym-media/testsamples/sample5.wav",//3 // 1
        "G:/lym-media/testsamples/cubase_instr_pling.wav",//5 // 2
        "G:/lym-media/testsamples/barn_effect_11.wav",//6 // 3
        "G:/lym-media/testsamples/barn_klung.wav",//8 //4
        "G:/lym-media/testsamples/barn_effect_2.wav",//12 //5
        "G:/lym-media/testsamples/barn_11.wav",//14 //6
        "G:/lym-media/testsamples/barn_14.wav",//15 // 7
        "G:/lym-media/testsamples/elektriskstoej.wav",//16 // 8
        "G:/lym-media/testsamples/barn_klung.wav",//21 // 9
        "G:/lym-media/testsamples/storm1.wav",//22 // 10
        "G:/lym-media/testsamples/kniv.wav",//24 // 11
        //===================================================================================
        "G:/lym-media/testsamples/storm1.wav",//18 // 12
        "G:/lym-media/testsamples/baglaens.wav",//21 // 13
        "G:/lym-media/testsamples/hammer3.wav",//23 // 14
        "G:/lym-media/testsamples/hammer2_pling.wav",//24 // 15
        "G:/lym-media/testsamples/tromme-distortion-delay.wav",//26 // 16
        "G:/lym-media/testsamples/draabe.wav",//28 // 17
        "G:/lym-media/testsamples/tromme-transformer.wav",//30 // 18
        "G:/lym-media/testsamples/stortromme-dump.wav",//31 // 19
        "G:/lym-media/testsamples/tromme-falling.wav",//32 // 20
        "G:/lym-media/testsamples/empty.wav",//00 // 21
        //===================================================================================
        "G:/lym-media/testsamples/barn_baandlyd.wav", // 00 //22
        "G:/lym-media/testsamples/soft_stepfilter_ned2.wav", // 00 // 23
        "G:/lym-media/testsamples/trappetrin1.wav", // 00 // 24
        "G:/lym-media/testsamples/trappetrin2.wav",
        "G:/lym-media/testsamples/morter.wav",
        "G:/lym-media/testsamples/plik.wav",
        "G:/lym-media/testsamples/morter2_132.wav",
        "G:/lym-media/testsamples/crushing.wav",
        "G:/lym-media/testsamples/crushing2.wav",
        "G:/lym-media/testsamples/crushing3.wav",
        "G:/lym-media/testsamples/bitcrush_stortromme.wav",
        "G:/lym-media/testsamples/klikkelilik3_132.wav",
        "G:/lym-media/testsamples/baglaens_delay3_132.wav"
        


].collect { |path| Buffer.read(s, path) };

//James' instrument
SynthDef(\PlayBuf, { |start, time, bufnum, pan, rate = 1, amp ,
    attack = 0.001, decay = 0.02, outbus|
    var sig;
    sig = PlayBuf.ar(1, bufnum, rate * BufRateScale.kr(bufnum), 1, start, 0)
    * EnvGen.kr(Env.linen(attack, time, decay), doneAction:2);
   Out.ar(outbus, Pan2.ar(sig, pan, amp ));
}).add;


)

(

~baseP = Pbind(
    \instrument, \PlayBuf,
    \time, Pkey(\dur) / Pfunc { TempoClock.tempo }
);
//============================================================================================
//                                 LOOPER CONNECTED TO DRAWING MACHINE 
//============================================================================================

//~num = 0;
~drumlooper = Pbind(
    \dur, Pseq([4], inf),
    \amp, 1,
    \bufnum, ~buffers[21],
    \x, Pseq(#[1,2,3,4], inf),
    \begin, Pfunc {|ev| 
        // first beat (new sequence)
        //if(ev[\x]==1,
            //{
                 // increments sequence counter
                // ~num=~num+1;
                // send messages to VirChor with sequence counts            {
                d.virchor.sendMsg("/beat "+TempoClock.default.beats+" "+ev[\x]);
                (TempoClock.default.beats+"/"+ev[\x]).postln;
                d.virchor.sendMsg("/flashTrack "+ev[\x]);
                d.virchor.sendMsg("/flashCA "+ev[\x]);
                d.virchor.sendMsg("/flashPart "+ev[\x]);           // },
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


//~kikP2de = Pbind(
//     \dur, Pseq([1, 1, 1, 1], inf),
//    \amp, Pseq([1, 0.8, 1, 0.8], inf),
//    \bufnum,  ~buffers[19],
//);

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

//~newdrumPde = Pbind(
//    \dur, Pseq([4], inf),
//    \amp, 1,
//    \bufnum, ~buffers[16]
//);


//~snarePde = Pbind(
    //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
   // \dur, Pseq([Rest (2.5), 0.5, Rest (1)], inf), 
   // \amp, 5,
   // \bufnum, ~buffers[7]
//   \dur, Pseq([0.5, 0.25, 0.5, 0.5, 0.25, 1, 0.25, 0.5, 0.25 ], inf),
//    \amp, Pseq([1, 0.5, 1.5, 1, 0.5, 1.5, 1, 0.5, 1], inf),
//    \bufnum,  ~buffers[25]
//);

//~snareP4de = Pbind(
    //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
   // \dur, Pseq([Rest (2.5), 0.5, Rest (1)], inf), 
   // \amp, 5,
   // \bufnum, ~buffers[7]
//   \dur, Pseq([ 0.75, 0.25, 0.25, 0.5, 0.5, 0.25, 0.25, 0.5, 0.25, 0.25, 0.25 ], inf),
//    \amp, Pseq([1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1], inf),
//    \bufnum,  ~buffers[14]
//);

//~snareP2de = Pbind(
    //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
//    \dur, Pseq([1, 0.25, 0.25, 1, 0.5, 0.75, 0.25], inf), 
//    \amp, 1,
//    \bufnum, ~buffers[24]
//);

//    ~snareP3de = Pbind(
 //   //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
 //   \dur, Pseq([Rest(2), 1, Rest(1)], inf), 
 //   \amp, 1,
//    \bufnum, ~buffers[18]
//);

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


~kikP2de = Pbind(
     \dur, Pseq([1, 1, 1, 1], inf),
    \amp, Pseq([1, 0.8, 1, 0.8], inf),
    \bufnum,  ~buffers[19],
);

~kikP3de = Pbind(
    \dur, Pseq([8], inf),
    \amp, Pseq([4], inf),
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
    //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
   // \dur, Pseq([Rest (2.5), 0.5, Rest (1)], inf), 
   // \amp, 5,
   // \bufnum, ~buffers[7]
   \dur, Pseq([0.5, 0.25, 0.5, 0.5, 0.25, 1, 0.25, 0.5, 0.25 ], inf),
    \amp, Pseq([1, 0.5, 1.5, 1, 0.5, 1.5, 1, 0.5, 1], inf),
    \bufnum,  ~buffers[15]
);

~snareP4de = Pbind(
    //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
   // \dur, Pseq([Rest (2.5), 0.5, Rest (1)], inf), 
   // \amp, 5,
   // \bufnum, ~buffers[7]
   \dur, Pseq([ 0.75, 0.25, 0.25, 0.5, 0.5, 0.25, 0.25, 0.5, 0.25, 0.25, 0.25 ], inf),
    \amp, Pseq([1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1, 0.5, 1], inf),
    \bufnum,  ~buffers[14]
);

~snareP2de = Pbind(
    //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
    \dur, Pseq([Rest (1), 1, Rest (2)], inf), 
    \amp, 1,
    \bufnum, ~buffers[13]
);

    ~snareP3de = Pbind(
    //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
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
    //\dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
    \dur, Pseq([Rest (2.5), 0.5, Rest (1)], inf), 
    \amp, 5,
    \bufnum, ~buffers[6]
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

/* ~hhP1 = Pbind(
     \dur, Pseq([0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25], inf),
    \amp, Pseq([0.3, 0.7, 0.3, 0.7,0.3, 0.7, 0.3, 0.7], inf),
    \bufnum,  ~buffers[14]
    
);
*/


~hhP1 = Pbind(
     \dur, Pseq([0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25], inf),
    \amp, Pseq([0.3, 0.7, 0.3, 0.7,0.3, 0.7, 0.3, 0.7], inf),
    \bufnum,  ~buffers[6]
);



//===========================================================================================
                      // FEMININ PATHS
//===========================================================================================

//~basePfp = Pbind(
//    \instrument, \PlayBufWoEnv,
//    \time, Pkey(\dur) / Pfunc { TempoClock.tempo }
//);

~kikPfp = Pbind(
    \dur, Pseq([1, 1, 1,1], inf),
    \amp, Pseq([0.8, 0.8, 0.8, 0.8], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
        Pseq([ ~buffers[0], ~buffers[0], ~buffers[1], ~buffers[0]] ),
        inf
)
);

~kikP2fp = Pbind(
     \dur, Pseq([1, Rest(1), 1.75, 0.25], inf),
    \amp, Pseq([2,2], inf),
    \bufnum,  ~buffers[3]
    
);



~hhPfp = Pbind(
     \dur, Pseq([0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25], inf),
    \amp, Pseq([0.3, 0.7, 0.3, 0.7,0.3, 0.7, 0.3, 0.7], inf),
    \bufnum,  ~buffers[0]
    
);
~newdrumPfp = Pbind(
    \dur, Pseq([1.5, 0.75, 0.25, 1.5], inf),
    \amp, 1.5,
    \bufnum, ~buffers[1]
);

~snarePfp = Pbind(
    \dur, Pseq([1, Rest(2), 1, 1, Rest(3)], inf),
    \amp, 0.2,
    \bufnum, ~buffers[4]
);
~snareP2fp = Pbind(
    \dur, Pseq([Rest(0.5), 15.5], inf),
    \amp, 0.8,
    \bufnum, ~buffers[2]
);

~snareP3fp = Pbind(
    \dur, Pseq([Rest(1), 1.60, 1, 0.4], inf),
    \amp, 0.5,
    \bufnum, ~buffers[4]
);

)
//==================================================================================================
//                        TEMPOCLOCK
//==================================================================================================

//TempoClock.tempo = 50/50; //denne svare til 120 tempo paa min loopstation
//TempoClock.tempo = 100/50; //denne svare til 120 tempo paa min loopstation dobbelt tempo end 50/50
TempoClock.tempo = 50/50;
TempoClock.tempo = 100/50;
TempoClock.tempo = 55/50;
//==================================================================================================


//  d.virchor.sendMsg("/flashmatch "+~volumeMatch); // send message

//         LOOPER DRAWING MACHINE
w = Pchain(~baseP, ~drumlooper ).play(quant: 4);
w.stop;
//         THE FALL
a = Pchain(~baseP, ~baandlyd).play(quant: 4);
b = Pchain(~baseP, ~softstepfilter).play(quant: 4);
//c = Pchain(~baseP, ~stortromme).play(quant: 4);
f = Pchain(~baseP, ~klikkelilik).play(quant: 4);
u = Pchain(~baseP, ~baglaens_delay).play(quant: 4);
v = Pchain(~baseP, ~hhPde).play(quant: 4);
TempoClock.tempo = 55/50;// loopstation = 132

//         DARK EARTH
~dumpstortromme = Pchain(~baseP, ~kikP2de).play(quant: 4); // stortrommen
~voldsomstortromme = Pchain(~baseP, ~kikP3de).play(quant: 4); // voldsomme stortromme
~storm = Pchain(~baseP, ~hhPde).play(quant: 4);
~armbolt1= Pchain(~baseP, ~snarePde).play(quant: 4); //armbolt

~baglaens = Pchain(~baseP, ~snareP2de).play(quant: 4); //driller stortrommen
~armbolt2 = Pchain(~baseP, ~snareP4de).play(quant: 4); //armbolt 2

~sjovtromme = Pchain(~baseP, ~snareP3de).play(quant: 4); //sjove trommelyd
~vandfosstromme = Pchain(~baseP, ~newdrumPde).play(quant: 4); // vandfosstromme
TempoClock.tempo = 50/50;// loopstation = 120


//         TO BE QUIET
p = Pchain(~baseP, ~kikP).play(quant: 4); // pladetromme
q = Pchain(~baseP, ~snareP).play(quant: 4); // klunkidunk
d = 
t = Pchain(~baseP, ~snareP3).play(quant: 4); // kniv
m = Pchain(~baseP, ~snareP4).play(quant: 4); // klapklap
r = Pchain(~baseP, ~hhP).play(quant: 4); // knase hihat
h = Pchain(~baseP, ~hhP1).play(quant: 4); // dukduk hihat
g = Pchain(~baseP, ~newdrumP).play(quant: 4); // boouum
j = Pchain(~baseP, ~hhP2).play(quant: 4); // hihat dikke-dikke

//          FEMININ PATHS
e = Pchain(~baseP, ~kikPfp).play(quant: 4); // langsomme stortromme stemme
l = Pchain(~baseP, ~kikP2fp).play(quant: 4); //salsa stortromme
k = Pchain(~baseP, ~snareP2fp).play(quant: 4);// stjernelyd
n = Pchain(~baseP, ~hhPfp).play(quant: 4); // hurtige rytme
c = Pchain(~baseP, ~newdrumPfp).play(quant: 4); // giver spaending
o = Pchain(~baseP, ~snarePfp).play(quant: 4); // klap1 virker ikke
i = Pchain(~baseP, ~snareP3fp).play(quant: 4); // klap2 virker ikke

TempoClock.tempo = 50/50;

x = 
y = 
z = 

p.stop;
q.stop;
d.stop;
t.stop;
m.stop;
r.stop;
h.stop;
g.stop;
j.stop;
f.stop;
k.stop;

e.stop;
l.stop;
n.stop;
c.stop;
k.stop;

n.stop;
c.stop;
e.stop;
k.stop;


a.stop;
b.stop;
c.stop;
f.stop;
u.stop;
v.stop;
~voldsomstortromme.stop;
~dumpstortromme.stop;
~sjovtromme.stop;
~dumpstortromme.stop;
~armbolt1.stop;
~armbolt2.stop;
~vandfosstromme.stop;
~storm.stop;
~baglaens.stop;

(
~kikPisOn = 0.0;
~hhPisOn = 0.0;
~hhP1isOn = 0.0;
~hhP2isOn = 0.0;
~newdrumPisOn = 0.0;
~snarePisOn = 0.0;
~snareP4isOn = 0.0;

~kikPfpisOn = 0.0;
~kikP2fpisOn = 0.0;
~hhPfpisOn = 0.0;
~newdrumPfpisOn = 0.0;
~snarePfpisOn = 0.0;
~snareP2fpisOn = 0.0;
~snareP3fpisOn = 0.0;
)


//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       OSC RESONDERS
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888


//================================================================================================
//                    LOOPER TO DRAWING MACHINE
//================================================================================================
// Looper on
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
    } 
).add;


//================================================================================================
//                        RYTME 1 on
//================================================================================================
(OSCresponder.new(nil, "/Quiet1_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

////////////////Kickdrum////////////////////////////////////
        if( (msg[3] > 0.5) && (~kikPisOn == 0.0) , {
            ~volumekikP = 3.0 * (msg[3] - 0.5) + 0.5;
            p = Pchain(~baseP, ~kikP).play(quant: 4);
            ~kikPisOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~kikPisOn == 1.0) , {
            p.stop;
            ~kikPisOn = 0.0;
        });

///////////////st.st.st.st.st.st.st////////////////////////////////
        if( (msg[2] > 0.5) && (~hhPisOn == 0.0) , {
            r = Pchain(~baseP, ~hhP).play(quant: 4);
            ~hhPisOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~hhPisOn == 1.0) , {
            r.stop;
            ~hhPisOn = 0.0;
        });

//////////dk.dk.dk.dk.dk.dk///////////////////////////////////
        if( (msg[1] > 0.5) && (~hhP1isOn == 0.0) , {
            h = Pchain(~baseP, ~hhP1).play(quant: 4);
            ~hhP1isOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~hhP1isOn == 1.0) , {
            h.stop;
            ~hhP1isOn = 0.0;
        });

/////////stormlyden//////////////////////////////////////////
if( (msg[4] > 0.5) && (~hhP2isOn == 0.0) , {
            j = Pchain(~baseP, ~hhP2).play(quant: 4);
            ~hhP2isOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~hhP2isOn == 1.0) , {
            j.stop;
            ~hhP2isOn = 0.0;
        });

    } 
).add;
)
//================================================================================================
//                        RYTME 2 on
//================================================================================================
(OSCresponder.new(nil, "/Quiet2_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

////////////////druuuoom////////////////////////////////////
        if( (msg[4] > 0.5) && (~newdrumPisOn == 0.0) , {
            g = Pchain(~baseP, ~newdrumP).play(quant: 4);
            ~newdrumPisOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~newdrumPisOn == 1.0) , {
            g.stop;
            ~newdrumPisOn = 0.0;
        });

/////////baggidak//////////////////////////////////////////
if( (msg[1] > 0.5) && (~snarePisOn == 0.0) , {
            q = Pchain(~baseP, ~snareP).play(quant: 4);
            ~snarePisOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~snarePisOn == 1.0) , {
            q.stop;
            ~snarePisOn = 0.0;
        });

//////////dk.dk.dk.dk.dk.dk///////////////////////////////////
    //  if( (msg[1] > 0.5) && (~hhP1isOn == 0.0) , {
    //      h = Pchain(~baseP, ~hhP1).play(quant: 4);
    //      ~hhP1isOn = 1.0;
    //  });

    //  if( (msg[1] < 0.5) && (~hhP1isOn == 1.0) , {
    //      h.stop;
    //      ~hhP1isOn = 0.0;
    //  });

/////////stormlyden//////////////////////////////////////////
if( (msg[3] > 0.5) && (~snareP4isOn == 0.0) , {
            m = Pchain(~baseP, ~snareP4).play(quant: 4);
            ~snareP4isOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~snareP4isOn == 1.0) , {
            m.stop;
            ~snareP4isOn = 0.0;
        });

/////////baggidak//////////////////////////////////////////
//if( (msg[4] > 0.5) && (~hhP2isOn == 0.0) , {
    //      j = Pchain(~baseP, ~hhP2).play(quant: 4);
    //      ~hhP2isOn = 1.0;
    //  });

    //  if( (msg[4] < 0.5) && (~hhP2isOn == 1.0) , {
    //      j.stop;
    //      ~hhP2isOn = 0.0;
    //  });
    } 
).add;
)
//================================================================================================
//                        RYTME 3 on
//================================================================================================
(OSCresponder.new(nil, "/fempath1_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

////////////////Kickdrumfp////////////////////////////////////
        if( (msg[3] > 0.5) && (~kikPfpisOn == 0.0) , {
            p = Pchain(~baseP, ~kikPfp).play(quant: 4);
            ~kikPfpisOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~kikPfpisOn == 1.0) , {
            p.stop;
            ~kikPfpisOn = 0.0;
        });

///////////////KickP2fp//////////////////////////////// KiKP2fp
        if( (msg[2] > 0.5) && (~kikP2fpisOn == 0.0) , {
            r = Pchain(~baseP, ~kikP2fp).play(quant: 4);
            ~kikP2fpisOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~kikP2fpisOn == 1.0) , {
            r.stop;
            ~kikP2fpisOn = 0.0;
        });

//////////hhPfp///////////////////////////////////
        if( (msg[1] > 0.5) && (~hhPfpisOn == 0.0) , {
            h = Pchain(~baseP, ~hhPfp).play(quant: 4);
            ~hhPfpisOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~hhPfpisOn == 1.0) , {
            h.stop;
            ~hhPfpisOn = 0.0;
        });

/////////newdrumPf//////////////////////////////////////////
if( (msg[4] > 0.5) && (~newdrumPfpisOn == 0.0) , {
            j = Pchain(~baseP, ~newdrumPfp).play(quant: 4);
            ~newdrumPfpisOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~newdrumPfpisOn == 1.0) , {
            j.stop;
            ~newdrumPfpisOn = 0.0;
        });

    } 
).add;
)
//================================================================================================
//                        RYTME 4 on
//================================================================================================
(OSCresponder.new(nil, "/fempath2_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

////////////////snarePfp////////////////////////////////////
        //if( (msg[4] > 0.5) && (~snarePfpisOn == 0.0) , {
        //  g = Pchain(~baseP, ~snarePfp).play(quant: 4);
        //  ~snarePfpisOn = 1.0;
        //});

        //if( (msg[4] < 0.5) && (~snarePfpisOn == 1.0) , {
        //  g.stop;
        //  ~snarePfpisOn = 0.0;
        //});

/////////snareP2fp//////////////////////////////////////////
if( (msg[4] > 0.5) && (~snareP2fpisOn == 0.0) , {
            q = Pchain(~baseP, ~snareP2fp).play(quant: 4);
            ~snareP2fpisOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~snareP2fpisOn == 1.0) , {
            q.stop;
            ~snareP2fpisOn = 0.0;
        });

/////////snareP3fp//////////////////////////////////////////
if( (msg[1] > 0.5) && (~snareP3fpisOn == 0.0) , {
            m = Pchain(~baseP, ~snareP3fp).play(quant: 4);
            ~snareP3fpisOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~snareP3fpisOn == 1.0) , {
            m.stop;
            ~snareP3fpisOn = 0.0;
        });

/////////baggidak//////////////////////////////////////////
//if( (msg[4] > 0.5) && (~hhP2isOn == 0.0) , {
    //      j = Pchain(~baseP, ~hhP2).play(quant: 4);
    //      ~hhP2isOn = 1.0;
    //  });

    //  if( (msg[4] < 0.5) && (~hhP2isOn == 1.0) , {
    //      j.stop;
    //      ~hhP2isOn = 0.0;
    //  });
    } 
).add;
)

