
// OSC messages receivers //////////////////////////////////////////////////////////////
// inputs:

//Server.default.options.inDevice_("ASIO : Focusrite USB ASIO Driver")


// outputs:
// Server.default.options.outDevice_("MME : Speakers (Realtek High Definitiion)")
//Server.local.options.sampleRate = 48000.00;
//Server.local.options.sampleRate = 44100.000000;

s.boot;


(

/////////////////////////////////////////////////////////////////////////////////
// OSC messages receivers
d=();

// emission of messages made with a dictionary
// d.pd = NetAddr.new("127.0.0.1", 1979); // create the NetAddr
// d.pd.sendMsg("/flash"); // send message

// emission of messages made with a dictionary
d.virchor = NetAddr.new("127.0.0.1", 1978); // create the NetAddr
d.virchor.sendMsg("/flash"); // send message


//===========================================================================
     //                         BUFFERS
//===========================================================================
~buffers = [
        "G:/home-win/LYM/sounds/silence001.wav"   
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
//               THE FALL 
//============================================================================================


//~num = 0;
~drumlooper = Pbind(
//    \dur, Pseq([4], inf),
    \dur, Pseq([0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5], inf),
    \amp, 0.1,
    \bufnum, ~buffers[0],
    \x, Pseq(#[1,2,3,4,5,6,7,8], inf),
    \begin, Pfunc {|ev| 
        // first beat (new sequence)
        //if(ev[\x]==1,
            //{
                 // increments sequence counter
                // ~num=~num+1;
                // send messages to VirChor with sequence counts            {
                d.virchor.sendMsg("/beat "+ev[\x]);
                (TempoClock.default.beats+"/"+ev[\x]).postln;
                //d.virchor.sendMsg("/flashTrack "+ev[\x]);
                //d.virchor.sendMsg("/flashCA "+ev[\x]);
                //d.virchor.sendMsg("/flashPart "+ev[\x]);           // },
           // {
                1;
           // })
    }
);



//==================================================================================================
//                        TEMPOCLOCK
//==================================================================================================


TempoClock.tempo = 50/50; //denne svare til 120 tempo paa min loopstation
TempoClock.tempo = 55/50; //denne svare til 132 tempo paa min loopstation, falling man 
)
//==================================================================================================


//  d.virchor.sendMsg("/flashmatch "+~volumeMatch); // send message

//         LOOPER DRAWING MACHINE
w = Pchain(~baseP, ~drumlooper ).play(quant: 4);
w.stop;
