(
Server.default.options.inDevice_("ASIO : Fast Track Pro ASIO");
Server.default.options.outDevice_("ASIO : Fast Track Pro ASIO");
s.boot;
)

(

// OSC messages receivers
d=();

// emission of messages made with a dictionary
d.pd = NetAddr.new("127.0.0.1", 1979); // create the NetAddr
d.pd.sendMsg("/flash"); // send message

// emission of messages made with a dictionary
// d.porphyrograph = NetAddr.new("192.168.0.2", 1978); // create the NetAddr
// d.renoise = NetAddr.new("192.168.0.2", 8000); // create the NetAddr

d.porphyrograph = NetAddr.new("127.0.0.1", 1978); // create the NetAddr
d.renoise = NetAddr.new("127.0.0.1", 8000); // create the NetAddr


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////variables to be evaluated first/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

~buffertorden;
~buffertordenOn = 0;
~synthbuffertorden;
~delayroom;
~drumlooper = 0;
~firstdrumlooper = 0;

// Sequencer
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


~knas= 0.0;
~knasps= 0.0;
~knasOn= 0.0;
~knit= 0.0;
~knitps= 0.0;
~knitOn= 0.0;
~knag= 0.0;
~knagps= 0.0;
~knagOn= 0.0;
~ghost= 0.0;
~ghostps= 0.0;
~ghostOn= 0.0;
~brand= 0.0;
~brandps= 0.0;
~brandOn = 0.0;
~legebil = 0.0;
~legebilps = 0.0;
~legebilOn = 0.0;
~maskine = 0.0;
~maskineps = 0.0;
~maskineOn = 0.0;
~sluk = 0.0;
~slukps = 0.0;
~slukOn = 0.0;
~space = 0.0;
~spaceps = 0.0;
~spaceOn = 0.0;
~bling1 = 0.0;
~bling1On = 0.0;
~bling1ps = 0.0;
~bling2 = 0.0;
~bling2On = 0.0;
~bling2ps = 0.0;
~babai_cello = 0.0;
~babai_celloOn = 0.0;
~babai_cellops = 0.0;
~babai_rytme = 0.0;
~babai_rytmeOn = 0.0;
~babai_rytmeps = 0.0;
~bitcrush_stortromme = 0.0;
~bitcrush_stortrommeOn = 0.0;
~bitcrush_stortrommeps = 0.0;
~femPath_cello = 0.0;
~femPath_celloOn = 0.0;
~femPath_cellops = 0.0;
~babai_synthpling = 0.0;
~babai_synthplingOn = 0.0;
~babai_synthplingps = 0.0;
~babai_heman = 0.0;
~babai_hemanOn = 0.0;
~babai_hemanps = 0.0;

~femPath_klik_synth = 0.0;
~femPath_klik_synthOn = 0.0;
~femPath_klik_synthps = 0.0;
~crashOn = 0.0;
~crashps = 0.0;
~crash = 0.0;
~pling = 0.0;
~plingOn = 0.0;
~plingps = 0.0;

~iyebassdrum = 0.0;
~iyebassdrumOn = 0.0;
~iyebassdrumps = 0.0;
~iyetiktak = 0.0;
~iyetiktakOn = 0.0;
~iyetiktakps = 0.0;

~lastbil = 0.0;
~lastbilOn = 0.0;
~lastbilps = 0.0;

~maskine = 0.0;
~maskineOn = 0.0;
~maskineps = 0.0;

~sluk = 0.0;
~slukOn = 0.0;
~slukps = 0.0;

~elekstoj = 0.0;
~elekstojOn = 0.0;
~elekstojps = 0.0;

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

//alkemi backtrack
~buffertorden = Buffer.read(s, "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_btFull8.wav");

SynthDef(\bufferBTDCont, { arg out = 0, volume=0;
	Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*2*volume)
}).send(s);

//===========================================================================
     //                         BUFFERS
//===========================================================================
~buffers = [
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_dumpdrum.wav",//0
        "G:/home-win/LYM/GN/SC_PD/SC-samples/tromme-transformer.wav",//1
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_knas_11.wav",//2
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_knit_15.wav",//3
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_knag_9.wav",//4
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_ghost_11.wav",//5
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_brand_9.wav",//6
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_legebil_16.wav",//7
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_maskine.wav",//8
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_sluk.wav",//9
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_space_8.wav",//10
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_drill.wav",//11
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_bling1.wav",//12
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_bling2.wav",//13
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_babai_cello.wav",//14
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_babai_rytme.wav",//15
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_FemPath_cello2.wav",//16
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_bitcrush_stortromme.wav",//17
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_babai_synth_pling.wav",//18
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_babai_heman.wav",//19
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_Fem_klik_synthv2.wav",//20
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_bitcrush_longdelay.wav",//21
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_cubase_instr_pling.wav", // 22
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_iye_bassdrum.wav", // 23
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_iye_tiktakdrum.wav", // 24
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_lastbil.wav", // 25
        "G:/home-win/LYM/GN/SC_PD/SC-samples/GN_elekstoj.wav", // 26
        "G:/home-win/LYM/GN/SC_PD/SC-samples/empty.wav" // 27
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
    \dur, Pseq([1,1,1,1], inf),
    \amp, 1,
    \bufnum, ~buffers[27],
    \x, Pseq(#[1,2,3,4], inf),
    \begin, Pfunc {|ev| 
      // send messages to porphyrograph with sequence counts            {
      d.porphyrograph.sendMsg("/beat "+ev[\x]);
      // ("beat: "+ev[\x]).postln;
      if( ~firstdrumlooper == 0 ) {
        // delayValue = msg[1];
        d.renoise.sendMsg("/renoise/transport/start ");
        ("start renoise: "+ev[\x]).postln;
        ~firstdrumlooper = 1;
      }{};    
      1;
    }
);


//============================================================================================
//               DRUMS
//============================================================================================



~kikP2de = Pbind(
     \dur, Pseq([1, 1, 1, 1], inf),
    \amp, Pseq([1, 0.8, 1, 0.8], inf),
    \bufnum,  ~buffers[0],
);

~bitcrush_stortromme = Pbind(
     \dur, Pseq([1, 1, 1, 1], inf),
    \amp, Pseq([2, 1.5, 2, 1.5], inf),
    \bufnum,  ~buffers[17],
);

     ~pling = Pbind(
    \dur, Pseq([Rest(0.5), 15.5], inf),
    \amp, 1,
    \bufnum, ~buffers[22]
);


    ~snareP3de = Pbind(
    \dur, Pseq([Rest(2), 1, Rest(1)], inf), 
    \amp, 1,
    \bufnum, ~buffers[1]
);
        ~bling1 = Pbind(
    \dur, Pseq([8], inf), 
    \amp, 1,
    \bufnum, ~buffers[12]
);
            ~bling2 = Pbind(
    \dur, Pseq([8], inf), 
    \amp, 1,
    \bufnum, ~buffers[13]
);
     ~babai_synthpling = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 1.5,
    \bufnum, ~buffers[18]
);

~femPath_cello = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 1.3,
    \bufnum, ~buffers[16]
);
    ~femPath_klik_synth = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 2,
    \bufnum, ~buffers[20]
);
        ~babai_heman = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 1.3,
    \bufnum, ~buffers[19]
);
        ~babai_cello = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 2,
    \bufnum, ~buffers[14]
);
            ~babai_rytme = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 2,
    \bufnum, ~buffers[15]
);

            ~iyebassdrum = Pbind(
    \dur, Pseq([8], inf), 
    \amp, 2,
    \bufnum, ~buffers[23]
);

            ~iyetiktak = Pbind(
    \dur, Pseq([8], inf), 
    \amp, 1,
    \bufnum, ~buffers[24]
);


    ~knas = Pbind(
    \dur, Pseq([20], 1),
    \amp, Pseq([3], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[2]] ),
        inf
        )
);
        ~knit = Pbind(
    \dur, Pseq([28], 1),
    \amp, Pseq([3], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[3]] ),
        inf
        )
);
        ~knag = Pbind(
    \dur, Pseq([16], 1),
    \amp, Pseq([2.5], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[4]] ),
        inf
        )
);
        ~ghost = Pbind(
    \dur, Pseq([21], 1),
    \amp, Pseq([3], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[5]] ),
        inf
        )
);

        ~brand = Pbind(
    \dur, Pseq([17], 1),
    \amp, Pseq([3], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[6]] ),
        inf
        )
);



    ~legebil = Pbind(
    \dur, Pseq([35], 1),
    \amp, Pseq([2.5], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[7]] ),
        inf
        )
);
        ~crash = Pbind(
    \dur, Pseq([17], 1),
    \amp, Pseq([2.5], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[21]] ),
        inf
        )
);
        ~lastbil = Pbind(
    \dur, Pseq([17], 1),
    \amp, Pseq([2.5], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[25]] ),
        inf
        )
);
        ~maskine = Pbind(
    \dur, Pseq([13], 1),
    \amp, Pseq([2.5], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[8]] ),
        inf
        )
);
     ~sluk = Pbind(
    \dur, Pseq([6], 1),
    \amp, Pseq([1], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[9]] ),
        inf
        )
);
    ~elekstoj = Pbind(
    \dur, Pseq([16], 1),
    \amp, Pseq([1], inf),
   // \amp, Pseq([5,5,0.3,5], inf),
   // \bufnum, ~buffers[0]
    \bufnum, Pn(
      Pseq([ ~buffers[26]] ),
        inf
        )
);

//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       OSC RESONDERS
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

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
    //  delayValue = msg[1];
    //}
    //{
    //  delayValue = 0;
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
    d.porphyrograph.sendMsg('/launch'); // send message 
    w = Pchain(~baseP, ~drumlooper ).play(quant: 4);
  } 
).add;


/////////////////////////////////////////////////////////////////////////////////
// UDP test received from PD and sent to porphyrograph
OSCresponder.new(nil, "/testUDP_fromPD", 
  { 
    arg time, resp, msg; 
    // time.postln;
    // resp.postln;
        d.porphyrograph.sendMsg('/testUDP'); // send message  
  } 
).add;

/////////////////////////////////////////////////////////////////////////////////
// UDP test received from porphyrograph
OSCresponder.new(nil, "/testUDP", 
  { 
    arg time, resp, msg; 
    // time.postln;
    // resp.postln;
    "***********************************************". postln;
    "porphyrograph UDP MESSAGE". postln;
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

//================================================================================================
//                        RYTME In Your Eyes
//================================================================================================
OSCresponder.new(nil, "/PDEyes", 
    { 
        arg time, resp, msg; 
        msg.postln; 

//////////////// bling1 ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~bling1On == 0.0) , {
        	msg[3].postln;
            ~bling1ps = Pchain(~baseP, ~bling1).play(quant: 4);
            ~bling1On = 1.0;
        });

        if( (msg[3] < 0.5) && (~bling1On == 1.0) , {
            ~bling1ps.stop;
            ~bling1On = 0.0;
        });

        //////////////// bling2 ////////////////////////////////////
        if( (msg[2] > 0.5)  && (~bling2On == 0.0) , {
        	msg[2].postln;
            ~bling2ps = Pchain(~baseP, ~bling2).play(quant: 4);
            ~bling2On = 1.0;
        });

        if( (msg[2] < 0.5) && (~bling2On == 1.0) , {
            ~bling2ps.stop;
            ~bling2On = 0.0;
        });
        //////////////// babai_cello  ////////////////////////////////////
        if( (msg[1] > 0.5) && (~babai_celloOn == 0.0) , {
            ~babai_cellops = Pchain(~baseP, ~babai_cello).play(quant: 4);
            ~babai_celloOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~babai_celloOn == 1.0) , {
            ~babai_cellops.stop;
            ~babai_celloOn = 0.0;
        });

/////////////////////////  babai_rytme //////////////////////////////////////////
if( (msg[4] > 0.5) && (~babai_rytmeOn == 0.0) , {
            ~babai_rytmeps = Pchain(~baseP, ~babai_rytme).play(quant: 4);
            ~babai_rytmeOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~babai_rytmeOn == 1.0) , {
            ~babai_rytmeps.stop;
            ~babai_rytmeOn = 0.0;
        });

            } 
).add;



//================================================================================================
//                        RYTME 1 on
//================================================================================================
OSCresponder.new(nil, "/Fallingman_on", 
    { 
        arg time, resp, msg; 
        msg.postln; 

//////////////// Knas ////////////////////////////////////
        if( (msg[3] > 0.5) && (~knasOn == 0.0) , {
            ~knasps = Pchain(~baseP, ~knas).play(quant: 4);
            ~knasOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~knasOn == 1.0) , {
            ~knasps.stop;
            ~knasOn = 0.0;
        });

        //////////////// Knit ////////////////////////////////////
        if( (msg[2] > 0.5) && (~knitOn == 0.0) , {
            ~knitps = Pchain(~baseP, ~knit).play(quant: 4);
            ~knitOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~knitOn == 1.0) , {
            ~knitps.stop;
            ~knitOn = 0.0;
        });

//////////////// Knag ////////////////////////////////////
        if( (msg[1] > 0.5) && (~knagOn == 0.0) , {
            ~knagps = Pchain(~baseP, ~knag).play(quant: 4);
            ~knagOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~knagOn == 1.0) , {
            ~knagps.stop;
            ~knagOn = 0.0;
        });

//////////////// ghost ////////////////////////////////////
        if( (msg[4] > 0.5) && (~ghostOn == 0.0) , {
            ~ghostps = Pchain(~baseP, ~ghost).play(quant: 4);
            ~ghostOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~ghostOn == 1.0) , {
            ~ghostps.stop;
            ~ghostOn = 0.0;
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

//////////////// brand ////////////////////////////////////
        if( (msg[1] > 0.5) && (~brandOn == 0.0) , {
            ~brandps = Pchain(~baseP, ~brand).play(quant: 4);
            ~brandOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~brandOn == 1.0) , {
            ~brandps.stop;
            ~brandOn = 0.0;
        });

//////////////// legebil////////////////////////////////////
        if( (msg[4] > 0.5) && (~legebilOn == 0.0) , {
            ~legebilps = Pchain(~baseP, ~legebil).play(quant: 4);
            ~legebilOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~legebilOn == 1.0) , {
            ~legebilps.stop;
            ~legebilOn = 0.0;
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

//////////////// ~bitcrush_stortromme ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~bitcrush_stortrommeOn == 0.0) , {
        	msg[3].postln;
            ~bitcrush_stortrommeps = Pchain(~baseP, ~bitcrush_stortromme).play(quant: 4);
            ~bitcrush_stortrommeOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~bitcrush_stortrommeOn == 1.0) , {
            ~bitcrush_stortrommeps.stop;
            ~bitcrush_stortrommeOn = 0.0;
        });

        //////////////// ~femPath_cello////////////////////////////////////
        if( (msg[2] > 0.5) && (~femPath_celloOn == 0.0) , {
            ~femPath_cellops = Pchain(~baseP, ~femPath_cello).play(quant: 4);
            ~femPath_celloOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~femPath_celloOn == 1.0) , {
            ~femPath_cellops.stop;
            ~femPath_celloOn = 0.0;
        });

/////////////// ~babai_synthpling////////////////////////////////
        if( (msg[1] > 0.5) && (~babai_synthplingOn == 0.0) , {
            ~babai_synthplingps = Pchain(~baseP, ~babai_synthpling).play(quant: 4);
            ~babai_synthplingOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~babai_synthplingOn == 1.0) , {
            ~babai_synthplingps.stop;
            ~babai_synthplingOn = 0.0;
        });
        ////////// ~babai_heman///////////////////////////////////
        if( (msg[4] > 0.5) && (~babai_hemanOn == 0.0) , {
            ~babai_hemanps = Pchain(~baseP, ~babai_heman).play(quant: 4);
            ~babai_hemanOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~babai_hemanOn == 1.0) , {
            ~babai_hemanps.stop;
            ~babai_hemanOn = 0.0;
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

//////////////// ~femPath_klik_synth ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~femPath_klik_synthOn == 0.0) , {
        	msg[3].postln;
            ~femPath_klik_synthps = Pchain(~baseP, ~femPath_klik_synth).play(quant: 4);
            ~femPath_klik_synthOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~femPath_klik_synthOn == 1.0) , {
            ~femPath_klik_synthps.stop;
            ~femPath_klik_synthOn = 0.0;
        });

//////////////// ~iyebassdrum ////////////////////////////////////
        if( (msg[2] > 0.5)  && (~iyebassdrumOn == 0.0) , {
        	msg[2].postln;
            ~iyebassdrumps = Pchain(~baseP, ~iyebassdrum).play(quant: 4);
            ~iyebassdrumOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~iyebassdrumOn == 1.0) , {
            ~iyebassdrumps.stop;
            ~iyebassdrumOn = 0.0;
        });

        //////////////// ~iyetiktak ////////////////////////////////////
        if( (msg[1] > 0.5)  && (~iyetiktakOn == 0.0) , {
        	msg[1].postln;
            ~iyetiktakps = Pchain(~baseP, ~iyetiktak).play(quant: 4);
            ~iyetiktakOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~iyetiktakOn == 1.0) , {
            ~iyetiktakps.stop;
            ~iyetiktakOn = 0.0;
        });
        //////////////// ~crash  ////////////////////////////////////
        //if( (msg[2] > 0.5) && (~crashOn == 0.0) , {
            //~crashps = Pchain(~baseP, ~crash).play(quant: 4);
            //~crashOn = 1.0;
        //});

        //if( (msg[2] < 0.5) && (~crashOn == 1.0) , {
           // ~crashps.stop;
           // ~crashOn = 0.0;
        //});
        //////////////// C-STYKKE  ////////////////////////////////////
        //if( (msg[1] > 0.5) && (~cstykkeOn == 0.0) , {
          //  ~cstyk = Pchain(~baseP, ~cstykke).play(quant: 4);
           // ~cstykkeOn = 1.0;
       // });

        //if( (msg[1] < 0.5) && (~cstykkeOn == 1.0) , {
         //   ~cstyk.stop;
          //  ~cstykkeOn = 0.0;
        //});

///////// StjernePLING //////////////////////////////////////////
        if( (msg[4] > 0.5) && (~plingOn == 0.0) , {
            ~plingps = Pchain(~baseP, ~pling).play(quant: 4);
            ~plingOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~plingOn == 1.0) , {
            ~plingps.stop;
            ~plingOn = 0.0;
        });



            } 
).add;

//================================================================================================
//                        RYTME 5 on
//================================================================================================
OSCresponder.new(nil, "/machines", 
    { 
        arg time, resp, msg; 
        msg.postln; 

//////////////// ~lastbil ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~lastbilOn == 0.0) , {
        	msg[3].postln;
            ~lastbilps = Pchain(~baseP, ~lastbil).play(quant: 4);
            ~lastbilOn = 1.0;
        });

        if( (msg[3] < 0.5) && (~lastbilOn == 1.0) , {
            ~lastbilps.stop;
            ~lastbilOn = 0.0;
        });

//////////////// ~maskine ////////////////////////////////////
        if( (msg[2] > 0.5)  && (~maskineOn == 0.0) , {
        	msg[2].postln;
            ~maskineps = Pchain(~baseP, ~maskine).play(quant: 4);
            ~maskineOn = 1.0;
        });

        if( (msg[2] < 0.5) && (~maskineOn == 1.0) , {
            ~maskineps.stop;
            ~maskineOn = 0.0;
        });

        //////////////// ~sluk ////////////////////////////////////
        if( (msg[1] > 0.5)  && (~slukOn == 0.0) , {
        	msg[1].postln;
            ~slukps = Pchain(~baseP, ~sluk).play(quant: 4);
            ~slukOn = 1.0;
        });

        if( (msg[1] < 0.5) && (~slukOn == 1.0) , {
            ~slukps.stop;
            ~slukOn = 0.0;
        });

        //////////////// ~elekstoj ////////////////////////////////////
        if( (msg[4] > 0.5)  && (~elekstojOn == 0.0) , {
        	msg[4].postln;
            ~elekstojps = Pchain(~baseP, ~elekstoj).play(quant: 4);
            ~elekstojOn = 1.0;
        });

        if( (msg[4] < 0.5) && (~elekstojOn == 1.0) , {
            ~elekstojps.stop;
            ~elekstojOn = 0.0;
        });

            } 
).add;
)


//==================================================================================================
//                        TEMPOCLOCK
//==================================================================================================

TempoClock.tempo = 50/50;


//==================================================================================================
s.record;
s.stopRecording;
