(
Server.default.options.inDevice_("ASIO : Focusrite USB 2.0 Audio Driver");
Server.default.options.outDevice_("ASIO : Focusrite USB 2.0 Audio Driver");
s.boot;
)

(

// OSC messages receivers
d=();
// emission of messages made with a dictionary
d.porphyrograph = NetAddr.new("192.168.0.2", 1978); // create the NetAddr
d.renoise = NetAddr.new("192.168.0.2", 8000); // create the NetAddr


//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       VARIABLES
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

// soundtrack
~buffertorden;
~synthbuffertorden;
~buffertordenOn = 0;

~drumlooper;
~firstdrumlooper = 0;

// SEQUENCER

//////////////Generel drums//////////////////////////
~dumpstortromme; ~dumpstortrommeps; ~dumpstortrommeOn = 0;
~bitcrush_stortromme; ~bitcrush_stortrommeps; ~bitcrush_stortrommeOn = 0;
~snareP3de; ~snareP3deps; ~snareP3deOn = 0;

//////////// In your Eyes ///////////////////////////
~bling1; ~bling1ps; ~bling1On = 0;
~bling2; ~bling2ps; ~bling2On = 0;
~iyebassdrum; ~iyebassdrumps; ~iyebassdrumOn = 0;
~iyetiktak; ~iyetiktakps; ~iyetiktakOn = 0;

/////////Feminine Path ///////////////////////////////
~femPath_cello; ~femPath_cellops; ~femPath_celloOn = 0;
~femPath_klik_synth; ~femPath_klik_synthps; ~femPath_klik_synthOn = 0;
~pling; ~plingps; ~plingOn = 0;

////////Babai //////////////////////////////////////
~babai_synthpling; ~babai_synthplingps; ~babai_synthplingOn = 0;
~babai_heman; ~babai_hemanps; ~babai_hemanOn = 0;
~babai_cello; ~babai_cellops; ~babai_celloOn = 0;
~babai_rytme; ~babai_rytmeps; ~babai_rytmeOn = 0;


//////// Sounds //////////////////////////////////////
~knas; ~knasps; ~knasOn = 0;
~knit; ~knitps; ~knitOn = 0;
~knag; ~knagps; ~knagOn = 0;
~ghost; ~ghostps; ~ghostOn = 0;
~brand; ~brandps; ~brandOn = 0;
~legebil; ~legebilps; ~legebilOn = 0;

//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       BACKTRACKS
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

~buffertorden = Buffer.cueSoundFile(s, "F:/boe/Desktop-F/SC/LolaMachine/lolamachineNEW2012/bactrack/GN2015/GN_btFull12_EQ.wav", 0, 2 );


SynthDef(\bufferBTDCont,{ arg out=0, volume=0;
      Out.ar([0,1], DiskIn.ar( 2,  ~buffertorden.bufnum, BufRateScale.kr(~buffertorden))*2*volume);
}).send(s);



//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       BUFFERS
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

~buffers = [
        "./samples/GN_dumpdrum_EQ2.wav",//0
        "./samples/tromme-transformer.wav",//1
        "./samples/GN_knas_11.wav",//2
        "./samples/GN_knit_15.wav",//3
        "./samples/GN_knag_9.wav",//4
        "./samples/GN_ghost_11.wav",//5
        "./samples/GN_brand_9.wav",//6
        "./samples/GN_legebil_16.wav",//7
        "./samples/GN_bling1.wav",//8
        "./samples/GN_bling2.wav",//9
        "./samples/GN_babai_cello.wav",//10
        "./samples/GN_babai_rytme.wav",//11
        "./samples/GN_FemPath_cello2.wav",//12
        "./samples/GN_bitcrush_stortromme.wav",//13
        "./samples/GN_babai_synth_pling.wav",//14
        "./samples/GN_babai_heman.wav",//15
        "./samples/GN_Fem_klik_synthv2.wav",//16
        "./samples/GN_cubase_instr_pling.wav",//17
        "./samples/GN_iye_bassdrum.wav",//18
        "./samples/GN_iye_tiktakdrum.wav",//19
        "./samples/empty.wav"//20
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
//                 LOOPER CONNECTED TO DRAWING MACHINE AND RENOISE FOR START
//============================================================================================

~drumlooper = Pbind(
    \dur, Pseq([1,1,1,1], inf),
    \amp, 1,
    \bufnum, ~buffers[20],
    \x, Pseq(#[1,2,3,4], inf),
    \begin, Pfunc {|ev| 
      // send messages to porphyrograph with sequence counts            {
      d.porphyrograph.sendMsg("/beat "+ev[\x]);
      // ("beat: "+ev[\x]).postln;
      if( ~firstdrumlooper == 0 ) {
        d.renoise.sendMsg("/renoise/transport/start ");
        ("start renoise: "+ev[\x]).postln;
        ~firstdrumlooper = 1;
      }{};    
      1;
    }
);


//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       DRUMS
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

//////////////Generel drums//////////////////////////
~dumpstortromme = Pbind(
     \dur, Pseq([1, 1, 1, 1], inf),
    \amp, Pseq([1, 0.8, 1, 0.8], inf),
    \bufnum,  ~buffers[0],
);

~bitcrush_stortromme = Pbind(
     \dur, Pseq([1, 1, 1, 1], inf),
    \amp, Pseq([2, 1.5, 2, 1.5], inf),
    \bufnum,  ~buffers[13],
);


~snareP3de = Pbind(
    \dur, Pseq([Rest(2), 1, Rest(1)], inf), 
    \amp, 1,
    \bufnum, ~buffers[1]
);

//////////// In your Eyes ///////////////////////////
~bling1 = Pbind(
    \dur, Pseq([8], inf), 
    \amp, 1,
    \bufnum, ~buffers[8]
);

~bling2 = Pbind(
    \dur, Pseq([8], inf), 
    \amp, 1,
    \bufnum, ~buffers[9]
);

~iyebassdrum = Pbind(
    \dur, Pseq([8], inf), 
    \amp, 2,
    \bufnum, ~buffers[18]
);

~iyetiktak = Pbind(
    \dur, Pseq([8], inf), 
    \amp, 1,
    \bufnum, ~buffers[19]
);

/////////Feminine Path ///////////////////////////////
~femPath_cello = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 1.3,
    \bufnum, ~buffers[12]
);

~femPath_klik_synth = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 2,
    \bufnum, ~buffers[16]
);

~pling = Pbind(
    \dur, Pseq([Rest(0.5), 15.5], inf),
    \amp, 1,
    \bufnum, ~buffers[17]
);

////////Babai //////////////////////////////////////
~babai_synthpling = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 1.5,
    \bufnum, ~buffers[14]
);

~babai_heman = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 1.3,
    \bufnum, ~buffers[15]
);

~babai_cello = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 2,
    \bufnum, ~buffers[10]
);

~babai_rytme = Pbind(
    \dur, Pseq([16], inf), 
    \amp, 2,
    \bufnum, ~buffers[11]
);


//////// Sounds //////////////////////////////////////
~knas = Pbind(
    \dur, Pseq([20], 1),
    \amp, Pseq([3], inf),
    \bufnum, Pn(
      Pseq([ ~buffers[2]] ),
        inf
        )
);

~knit = Pbind(
    \dur, Pseq([28], 1),
    \amp, Pseq([3], inf),
    \bufnum, Pn(
      Pseq([ ~buffers[3]] ),
        inf
        )
);

~knag = Pbind(
    \dur, Pseq([16], 1),
    \amp, Pseq([2.5], inf),
    \bufnum, Pn(
      Pseq([ ~buffers[4]] ),
        inf
        )
);

~ghost = Pbind(
    \dur, Pseq([21], 1),
    \amp, Pseq([3], inf),
    \bufnum, Pn(
      Pseq([ ~buffers[5]] ),
        inf
        )
);

~brand = Pbind(
    \dur, Pseq([17], 1),
    \amp, Pseq([3], inf),
    \bufnum, Pn(
      Pseq([ ~buffers[6]] ),
        inf
        )
);


~legebil = Pbind(
    \dur, Pseq([35], 1),
    \amp, Pseq([2.5], inf),
    \bufnum, Pn(
      Pseq([ ~buffers[7]] ),
        inf
        )
);






//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//================================================================================================
//                                       OSC RESPONDERS
//================================================================================================
//888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888

/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//backtrack vol
OSCresponder.new(nil, "/PDbt_vol", 
  { 
    arg time, resp, msg; 
    msg.postln;
    ~synthbuffertorden.set(\volume, msg[1]*1);
  } 
).add;

/////////////////////////////////////////////////////////////////////////////////////
// backtrack on
OSCresponder.new(nil, "/PDbt1_on", 
  { 
    arg time, resp, msg; 
    msg.postln; 

   ~buffertordenOn = 1;
   ~synthbuffertorden = Synth(\bufferBTDCont);
   ~synthbuffertorden.set(\volume, 0);
   d.porphyrograph.sendMsg('/launch'); // send message 
   w = Pchain(~baseP, ~drumlooper ).play(quant: 4);
 }).add;


/////////////////////////////////////////////////////////////////////////////////
// UDP test received from PD and sent to porphyrograph
OSCresponder.new(nil, "/testUDP_fromPD", 
  { 
    arg time, resp, msg; 
        d.porphyrograph.sendMsg('/testUDP'); // send message  
  } 
).add;

/////////////////////////////////////////////////////////////////////////////////
// UDP test received from porphyrograph
OSCresponder.new(nil, "/testUDP", 
  { 
    arg time, resp, msg; 
    "***********************************************". postln;
    "porphyrograph UDP MESSAGE". postln;
    "***********************************************". postln;
  }
).add;

//================================================================================================
//                        RYTME In Your Eyes
//================================================================================================
OSCresponder.new(nil, "/RhythmA", 
    { 
        arg time, resp, msg; 
        msg.postln; 

		//////////////// bling1 ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~bling1On == 0) , {
        	msg[3].postln;
            ~bling1ps = Pchain(~baseP, ~bling1).play(quant: 4);
            ~bling1On = 1;
        });

        if( (msg[3] < 0.5) && (~bling1On == 1) , {
            ~bling1ps.stop;
            ~bling1On = 0;
        });

        //////////////// bling2 ////////////////////////////////////
        if( (msg[2] > 0.5)  && (~bling2On == 0) , {
        	msg[2].postln;
            ~bling2ps = Pchain(~baseP, ~bling2).play(quant: 4);
            ~bling2On = 1;
        });

        if( (msg[2] < 0.5) && (~bling2On == 1) , {
            ~bling2ps.stop;
            ~bling2On = 0;
        });

        //////////////// babai_cello  ////////////////////////////////////
        if( (msg[1] > 0.5) && (~babai_celloOn == 0) , {
            ~babai_cellops = Pchain(~baseP, ~babai_cello).play(quant: 4);
            ~babai_celloOn = 1;
        });

        if( (msg[1] < 0.5) && (~babai_celloOn == 1) , {
            ~babai_cellops.stop;
            ~babai_celloOn = 0;
        });

		/////////////////////////  babai_rytme //////////////////////////////////////////
		if( (msg[4] > 0.5) && (~babai_rytmeOn == 0) , {
            ~babai_rytmeps = Pchain(~baseP, ~babai_rytme).play(quant: 4);
            ~babai_rytmeOn = 1;
        });

        if( (msg[4] < 0.5) && (~babai_rytmeOn == 1) , {
            ~babai_rytmeps.stop;
            ~babai_rytmeOn = 0;
        });

	} 
).add;



//================================================================================================
//                        RYTME 1 on
//================================================================================================
OSCresponder.new(nil, "/RhythmB", 
    { 
        arg time, resp, msg; 
        msg.postln; 

		//////////////// Knas ////////////////////////////////////
        if( (msg[3] > 0.5) && (~knasOn == 0) , {
            ~knasps = Pchain(~baseP, ~knas).play(quant: 4);
            ~knasOn = 1;
        });

        if( (msg[3] < 0.5) && (~knasOn == 1) , {
            ~knasps.stop;
            ~knasOn = 0;
        });

        //////////////// Knit ////////////////////////////////////
        if( (msg[2] > 0.5) && (~knitOn == 0) , {
            ~knitps = Pchain(~baseP, ~knit).play(quant: 4);
            ~knitOn = 1;
        });

        if( (msg[2] < 0.5) && (~knitOn == 1) , {
            ~knitps.stop;
            ~knitOn = 0;
        });

		//////////////// Knag ////////////////////////////////////
        if( (msg[1] > 0.5) && (~knagOn == 0) , {
            ~knagps = Pchain(~baseP, ~knag).play(quant: 4);
            ~knagOn = 1;
        });

        if( (msg[1] < 0.5) && (~knagOn == 1) , {
            ~knagps.stop;
            ~knagOn = 0;
        });

		//////////////// ghost ////////////////////////////////////
        if( (msg[4] > 0.5) && (~ghostOn == 0) , {
            ~ghostps = Pchain(~baseP, ~ghost).play(quant: 4);
            ~ghostOn = 1;
        });

        if( (msg[4] < 0.5) && (~ghostOn == 1) , {
            ~ghostps.stop;
            ~ghostOn = 0;
        });
            } 
).add;


//================================================================================================
//                        RYTME 2 on
//================================================================================================
OSCresponder.new(nil, "/RhythmC", 
    { 
        arg time, resp, msg; 
        msg.postln; 

		//////////////// Dumpstortromme ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~dumpstortrommeOn == 0) , {
        	//msg[3].postln;
            ~dumpstortrommeps = Pchain(~baseP, ~dumpstortromme).play(quant: 4);
            ~dumpstortrommeOn = 1;
        });

        if( (msg[3] < 0.5) && (~dumpstortrommeOn == 1) , {
            ~dumpstortrommeps.stop;
            ~dumpstortrommeOn = 0;
        });

        //////////////// Sjovtromme ////////////////////////////////////
        if( (msg[2] > 0.5) && (~snareP3deOn == 0) , {
            ~snareP3deps = Pchain(~baseP, ~snareP3de).play(quant: 4);
            ~snareP3deOn = 1;
        });

        if( (msg[2] < 0.5) && (~snareP3deOn == 1) , {
            ~snareP3deps.stop;
            ~snareP3deOn = 0;
        });

		//////////////// brand ////////////////////////////////////
        if( (msg[1] > 0.5) && (~brandOn == 0) , {
            ~brandps = Pchain(~baseP, ~brand).play(quant: 4);
            ~brandOn = 1;
        });

        if( (msg[1] < 0.5) && (~brandOn == 1) , {
            ~brandps.stop;
            ~brandOn = 0;
        });

		//////////////// legebil////////////////////////////////////
        if( (msg[4] > 0.5) && (~legebilOn == 0) , {
            ~legebilps = Pchain(~baseP, ~legebil).play(quant: 4);
            ~legebilOn = 1;
        });

        if( (msg[4] < 0.5) && (~legebilOn == 1) , {
            ~legebilps.stop;
            ~legebilOn = 0;
        });
            } 
).add;


//================================================================================================
//                        RYTME 3 on
//================================================================================================
OSCresponder.new(nil, "/RhythmD", 
    { 
        arg time, resp, msg; 
        msg.postln; 

		//////////////// ~bitcrush_stortromme ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~bitcrush_stortrommeOn == 0) , {
        	msg[3].postln;
            ~bitcrush_stortrommeps = Pchain(~baseP, ~bitcrush_stortromme).play(quant: 4);
            ~bitcrush_stortrommeOn = 1;
        });

        if( (msg[3] < 0.5) && (~bitcrush_stortrommeOn == 1) , {
            ~bitcrush_stortrommeps.stop;
            ~bitcrush_stortrommeOn = 0;
        });

        //////////////// ~femPath_cello////////////////////////////////////
        if( (msg[2] > 0.5) && (~femPath_celloOn == 0) , {
            ~femPath_cellops = Pchain(~baseP, ~femPath_cello).play(quant: 4);
            ~femPath_celloOn = 1;
        });

        if( (msg[2] < 0.5) && (~femPath_celloOn == 1) , {
            ~femPath_cellops.stop;
            ~femPath_celloOn = 0;
        });

		/////////////// ~babai_synthpling////////////////////////////////
        if( (msg[1] > 0.5) && (~babai_synthplingOn == 0) , {
            ~babai_synthplingps = Pchain(~baseP, ~babai_synthpling).play(quant: 4);
            ~babai_synthplingOn = 1;
        });

        if( (msg[1] < 0.5) && (~babai_synthplingOn == 1) , {
            ~babai_synthplingps.stop;
            ~babai_synthplingOn = 0;
        });
        ////////// ~babai_heman///////////////////////////////////
        if( (msg[4] > 0.5) && (~babai_hemanOn == 0) , {
            ~babai_hemanps = Pchain(~baseP, ~babai_heman).play(quant: 4);
            ~babai_hemanOn = 1;
        });

        if( (msg[4] < 0.5) && (~babai_hemanOn == 1) , {
            ~babai_hemanps.stop;
            ~babai_hemanOn = 0;
        });

            } 
).add;


//================================================================================================
//                        RYTME 4 on
//================================================================================================
OSCresponder.new(nil, "/RhythmE", 
    { 
        arg time, resp, msg; 
        msg.postln; 

		//////////////// ~femPath_klik_synth ////////////////////////////////////
        if( (msg[3] > 0.5)  && (~femPath_klik_synthOn == 0) , {
        	msg[3].postln;
            ~femPath_klik_synthps = Pchain(~baseP, ~femPath_klik_synth).play(quant: 4);
            ~femPath_klik_synthOn = 1;
        });

        if( (msg[3] < 0.5) && (~femPath_klik_synthOn == 1) , {
            ~femPath_klik_synthps.stop;
            ~femPath_klik_synthOn = 0;
        });

		//////////////// ~iyebassdrum ////////////////////////////////////
        if( (msg[2] > 0.5)  && (~iyebassdrumOn == 0) , {
        	msg[2].postln;
            ~iyebassdrumps = Pchain(~baseP, ~iyebassdrum).play(quant: 4);
            ~iyebassdrumOn = 1;
        });

        if( (msg[2] < 0.5) && (~iyebassdrumOn == 1) , {
            ~iyebassdrumps.stop;
            ~iyebassdrumOn = 0;
        });

        //////////////// ~iyetiktak ////////////////////////////////////
        if( (msg[1] > 0.5)  && (~iyetiktakOn == 0) , {
        	msg[1].postln;
            ~iyetiktakps = Pchain(~baseP, ~iyetiktak).play(quant: 4);
            ~iyetiktakOn = 1;
        });

        if( (msg[1] < 0.5) && (~iyetiktakOn == 1) , {
            ~iyetiktakps.stop;
            ~iyetiktakOn = 0;
        });


		///////// StjernePLING //////////////////////////////////////////
        if( (msg[4] > 0.5) && (~plingOn == 0) , {
            ~plingps = Pchain(~baseP, ~pling).play(quant: 4);
            ~plingOn = 1;
        });

        if( (msg[4] < 0.5) && (~plingOn == 1) , {
            ~plingps.stop;
            ~plingOn = 0;
        });

	} 
).add;
)




//==================================================================================================
s.record;
s.stopRecording;
//==================================================================================================