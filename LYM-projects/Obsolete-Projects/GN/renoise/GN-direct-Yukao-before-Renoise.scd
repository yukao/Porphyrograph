s.quit;

// Server.default.options.inDevice_("ASIO : Fast Track Pro ASIO");
// Server.default.options.outDevice_("ASIO : Fast Track Pro ASIO");
// s.recSampleFormat = "int16";
// s.recChannels = 2;
// s.recHeaderFormat = "WAV";
// s.prepareForRecord("G:/home-win/LYM/GN/GN_SC_machine.wav");
// s.record;
// wait(5);
// s.stopRecording;

(
s.options.maxNodes_(12000);
s.options.memSize_(170000); 
s.boot;
)

(
~cpuRatio = 1.0;

// ~buffertorden;
// ~buffertordenOn = 0;
// ~synthbuffertorden;

// chaining has been made
~synthIsOn = 0;

~globalVolume = 1.0;

~nbSensorMesssages = 0;
~currentSetup = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];

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
d.virchor = NetAddr.new("127.0.0.1", 1978); // create the NetAddr 192.168.0.2
d.virchor.sendMsg("/flash"); // send message

// ~path = "F:/boe/Desktop-F/SC/Projects/2015_generativeNights/ClassifiedSamples/all_samples/";
// ~path = "G:/home-win/LYM/GN/sounds/";
// ~path = "/home/jacquemi/3D/LYM/GN/sounds/";
~path = "G:/home-win/LYM/GN/sounds/";

//GN backtrack
// ~buffertorden = Buffer.read(s, ~path ++ "GN_backtrack_v3.wav");
// SynthDef(\bufferBTDCont, { arg out = 0, volume=1.0;
// Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*2*volume)
// }).send(s);

~buffers = [
    ~path ++ "samples_part1/sample_part1-001.wav", // 1
    ~path ++ "samples_part1/sample_part1-002.wav", // 2
    ~path ++ "samples_part1/sample_part1-003.wav", // 3
    ~path ++ "samples_part1/sample_part1-004.wav", // 4
    ~path ++ "samples_part1/sample_part1-005.wav", // 5
    ~path ++ "samples_part1/sample_part1-006.wav", // 6
    ~path ++ "samples_part1/sample_part1-001.wav", // 7
    ~path ++ "samples_part1/sample_part1-002.wav", // 8
    ~path ++ "samples_part1/sample_part1-003.wav", // 9
    ~path ++ "samples_part1/sample_part1-004.wav", // 10
    ~path ++ "samples_part1/sample_part1-005.wav", // 11
    ~path ++ "samples_part1/sample_part1-006.wav", // 12
    ~path ++ "samples_part1/sample_part1-001.wav", // 13
    ~path ++ "samples_part1/sample_part1-002.wav", // 14
    ~path ++ "samples_part1/sample_part1-003.wav", // 15
    ~path ++ "samples_part1/sample_part1-004.wav", // 16

    ~path ++ "samples_part2/sample_part2-001.wav", // 17
    ~path ++ "samples_part2/sample_part2-002.wav", // 18
    ~path ++ "samples_part2/sample_part2-003.wav", // 19
    ~path ++ "samples_part2/sample_part2-004.wav", // 20
    ~path ++ "samples_part2/sample_part2-005.wav", // 21
    ~path ++ "samples_part2/sample_part2-006.wav", // 22
    ~path ++ "samples_part2/sample_part2-007.wav", // 23
    ~path ++ "samples_part2/sample_part2-008.wav", // 24
    ~path ++ "samples_part2/sample_part2-009.wav", // 25
    ~path ++ "samples_part2/sample_part2-010.wav", // 26
    ~path ++ "samples_part2/sample_part2-011.wav", // 27
    ~path ++ "samples_part2/sample_part2-012.wav", // 28
    ~path ++ "samples_part2/sample_part2-013.wav", // 29
    ~path ++ "samples_part2/sample_part2-014.wav", // 30
    ~path ++ "samples_part2/sample_part2-015.wav", // 31
    ~path ++ "samples_part2/sample_part2-001.wav", // 32

    ~path ++ "samples_part3/sample_part3-001.wav", // 33
    ~path ++ "samples_part3/sample_part3-002.wav", // 34
    ~path ++ "samples_part3/sample_part3-003.wav", // 35
    ~path ++ "samples_part3/sample_part3-004.wav", // 36
    ~path ++ "samples_part3/sample_part3-005.wav", // 37
    ~path ++ "samples_part3/sample_part3-006.wav", // 38
    ~path ++ "samples_part3/sample_part3-007.wav", // 39
    ~path ++ "samples_part3/sample_part3-008.wav", // 40
    ~path ++ "samples_part3/sample_part3-009.wav", // 41
    ~path ++ "samples_part3/sample_part3-010.wav", // 42
    ~path ++ "samples_part3/sample_part3-011.wav", // 43
    ~path ++ "samples_part3/sample_part3-012.wav", // 44
    ~path ++ "samples_part3/sample_part3-013.wav", // 45
    ~path ++ "samples_part3/sample_part3-014.wav", // 46
    ~path ++ "samples_part3/sample_part3-015.wav", // 47
    ~path ++ "samples_part3/sample_part3-016.wav", // 48

    ~path ++ "samples_part1/GNgroups_part1-001.wav",//1, 2, 5, 6
    ~path ++ "samples_part1/GNgroups_part1-002.wav",//3, 4, 7, 8 
    ~path ++ "samples_part1/GNgroups_part1-003.wav",//9, 10, 13, 14 
    ~path ++ "samples_part1/GNgroups_part1-004.wav",//11, 12, 15, 16 

    ~path ++ "samples_part2/GNgroups_part2-001.wav",//17, 18, 21, 22
    ~path ++ "samples_part2/GNgroups_part2-002.wav",//19, 20, 23, 24
    ~path ++ "samples_part2/GNgroups_part2-003.wav",//25, 26, 29, 30
    ~path ++ "samples_part2/GNgroups_part2-004.wav",//27, 28, 31, 32

    ~path ++ "samples_part3/GNgroups_part3-001.wav",//33, 34, 37, 38
    ~path ++ "samples_part3/GNgroups_part3-002.wav",//35, 36, 39, 40
    ~path ++ "samples_part3/GNgroups_part3-003.wav",//41, 42, 45, 46
    ~path ++ "samples_part3/GNgroups_part3-004.wav" //43, 44, 47, 48
].collect { |path| Buffer.read(s, path) };

//James' instrument
SynthDef(\PlayBuffer, { |start = 0, time=0.2, bufnum, rate = 1, amp , attack = 0.001, decay = 0.002 , panning=0|
    var sig;
    sig = PlayBuf.ar(1, bufnum, rate * BufRateScale.kr(bufnum), 1, start, 0)
    	* EnvGen.kr(Env.linen(attack, time, decay), doneAction:2) *amp ;
    Out.ar([0,1], Pan2.ar(sig, panning , amp ));
}).add;

~ids=[
	"t1","t2","t3","t4","t5","t6","t7","t8","t9","t10",
	"t11","t12","t13","t14","t15","t16",

    "t17","t18","t19","t20","t21","t22","t23","t24","t25",
	"t26","t27","t28","t29","t30","t31","t32",

    "t33","t34","t35","t36","t37","t38","t39","t40","t41",
    "t42","t43","t44","t45","t46","t47","t48",

	"G1","G2","G3","G4",
    "G5","G6","G7","G8",
    "G9","G10","G11","G12"];
~idsAmp=[
    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,0.25,

    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,0.25,

    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,0.25,

    0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25];
~idsPan=[
    -0.6,-0.3,0.3,0.6,-0.6,-0.3,0.3,0.6,
    -0.6,-0.3,0.3,0.6,-0.6,-0.3,0.3,0.6,

    -0.6,-0.3,0.3,0.6,-0.6,-0.3,0.3,0.6,
    -0.6,-0.3,0.3,0.6,-0.6,-0.3,0.3,0.6,

    -0.6,-0.3,0.3,0.6,-0.6,-0.3,0.3,0.6,
    -0.6,-0.3,0.3,0.6,-0.6,-0.3,0.3,0.6,

    -0.4,0.4,-0.4,0.4,
    -0.4,0.4,-0.4,0.4,
    -0.4,0.4,-0.4,0.4 ];
~idsQuant=[
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,

    1,2,3,4,1,2,3,4,
    1,2,3,4,1,2,3,4,

    1,2,3,4,1,2,3,4,
    1,2,3,4,1,2,3,4,

	1,1,1,1,
	1,2,3,4,
	1,1,1,1 ];

~ids.do({ arg item, i; Tdef(item, { var z; z = Synth(\PlayBuffer, [\amp , ~idsAmp[i], 
                        \bufnum,  ~buffers[i], \panning,  ~idsPan[i], 
                        \time, 0.5 / Pfunc { TempoClock.tempo }] );z.release}
); });


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
// Dinosteps on
OSCresponder.new(nil, "/sensors", 
    { 
        arg time, resp, msg; 
        // time.postln;
        // resp.postln;
        // msg.postln; 

        ~totAmp = msg[28 + 1];
        if( ~totAmp > 4.0 , {
            ~totAmp = 2.5 - 0.5 * ((~totAmp - 3.0) ** 0.1);
        }
        ,{
            ~totAmp = 2.0;
        });

        if(~globalVolume > 0, {
                ~totAmp = ~totAmp / ~globalVolume;
            },
            {
                 ~totAmp = 1000.0;   
            });


        if( (s.avgCPU/~cpuRatio < 0.3)  && (s.peakCPU/~cpuRatio < 0.3) ,
        {
          for (16, 27, { arg i; 
              if( msg[i + 1] > 0, { j = i + 32; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
          }); // sample aliases
          for (0, 15, { arg i; 
             if( msg[i + 1] > 0, { j = ~currentSetup[i]; ~idsAmp[j]=(msg[i + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
          }); // individual samples
        },
        {
            if( (s.avgCPU/~cpuRatio < 0.5)  && (s.peakCPU/~cpuRatio < 0.5) ,
            {
                for (16, 27, { arg i; 
                   if( msg[i + 1] > 0, { j = i + 32; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
                }); // sample aliases
                ~rest = ~nbSensorMesssages % 4;
                if( msg[~rest + 1] > 0, { j = ~currentSetup[~rest]; ~idsAmp[j]=(msg[~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                if( msg[2 * ~rest + 1] > 0, { j = ~currentSetup[2 * ~rest]; ~idsAmp[j]=(msg[2 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                if( msg[3 * ~rest + 1] > 0, { j = ~currentSetup[3 * ~rest]; ~idsAmp[j]=(msg[3 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                if( msg[4 * ~rest + 1] > 0, { j = ~currentSetup[4 * ~rest]; ~idsAmp[j]=(msg[4 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                if( msg[5 * ~rest + 1] > 0, { j = ~currentSetup[5 * ~rest]; ~idsAmp[j]=(msg[4 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
            },
            {
                if( (s.avgCPU/~cpuRatio < 0.6)  && (s.peakCPU/~cpuRatio < 0.6) ,
                {
                    for (16, 27, { arg i; 
                      if( msg[i + 1] > 0, { j = i + 32; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
                    }); // sample aliases
                    ~rest = ~nbSensorMesssages % 8;
                    if( msg[~rest + 1] > 0, { j = ~currentSetup[~rest]; ~idsAmp[j]=(msg[~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                    if( msg[2 * ~rest + 1] > 0, { j = ~currentSetup[2 * ~rest]; ~idsAmp[j]=(msg[2 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
                },
                {
                    if( (s.avgCPU/~cpuRatio < 0.7)  && (s.peakCPU/~cpuRatio < 0.7) ,
                    {
                        ~rest = ~nbSensorMesssages % 4;
                        if( msg[~rest + 16 + 1] > 0, { i = ~rest + 16; j = i + 32; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                        if( msg[2 * ~rest + 16 + 1] > 0, { i = 2 * ~rest + 16; j = i + 32; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                        if( msg[3 * ~rest + 16 + 1] > 0, { i = 3 * ~rest + 16; j = i + 32; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                    });
                });
            });
        });
        ~nbSensorMesssages = ~nbSensorMesssages + 1;
        // if( ~nbSensorMesssages % 50 == 0, {("Avg: "++s.avgCPU).postln; ("peak: "++s.peakCPU).postln; ("UGens: "++s.numUGens).postln;"------------".postln;});
    } 
).add;

////////////////////////////////////////////////////////////////////////////////////
// setup change
OSCresponder.new(nil, "/sample_setUp", 
    { 
        arg time, resp, msg; 
        // msg.postln;
        // time.post
        // resp.postln;
		for (0, 15, { arg i; ~currentSetup[i] = msg[i + 1]; } );
        // ~currentSetup.postln;
    } 
).add;

// tempo clock change
OSCresponder.new(nil, "/TempoClock", 
    { 
        arg time, resp, msg; 
        // time.postln;
        // resp.postln;
        TempoClock.tempo = msg[1].round(1)/50;
        // TempoClock.tempo.postln;
    } 
).add;

/////////////////////////////////////////////////////////////////////////////////////
// beat generator

SynthDef(\PlayBuf, { |start, time, bufnum, pan, rate = 1, amp ,
    attack = 0.001, decay = 0.02, outbus|
}).add;


~baseP = Pbind(
    \instrument, \PlayBuf,
    \time, Pkey(\dur) / Pfunc { TempoClock.tempo }
);

//~num = 0;
~drumlooper = Pbind(
    \dur, Pseq([4.0,4.0,4.0,4.0,4.0,4.0,4.0,4.0], inf),
    \amp, 1,
    \bufnum, 0,
    \x, Pseq(#[1,2,3,4,5,6,7,8], inf),
    \begin, Pfunc {|ev| 
                d.virchor.sendMsg("/beat "+ev[\x]);
                // (ev[\x]).postln;
                1;
    }
);

/////////////////////////////////////////////////////////////////////////////////////
// Bt1 on
OSCresponder.new(nil, "/PDbt1_on", 
    { 
        arg time, resp, msg; 
        // time.postln;
        // resp.postln;
        msg.postln; 

        if( ~synthIsOn == 0 ) {
            w = Pchain(~baseP, ~drumlooper ).play(quant:4);
             ~synthIsOn = 1;
        }

        // if( ~buffertordenOn == 0 , {
            // ~buffertordenOn = 1;
            // synthbuffertorden = Synth(\bufferBTDCont);
            // ~synthbuffertorden.set(\volume, 0.5);
        // },
        // {
            // ~synthbuffertorden.free;
            // ~synthbuffertorden = Synth(\bufferBTDCont);
        // });
    } 
).add;

/////////////////////////////////////////////////////////////////////////////////
//backtrack vol
OSCresponder.new(nil, "/volSC", 
    { 
        arg time, resp, msg; 
        // time.postln;
        // resp.postln;
        // msg.postln; 

        // if( ~buffertordenOn == 1 , {
        //    ~synthbuffertorden.set(\volume, msg[1]*0.5);
        // });
        ~globalVolume = msg[1];
        // ~globalVolume.postln;
    } 
).add;

Platform.case(    
\osx,       { ~cpuRatio = 100.0; },    
\linux,     { ~cpuRatio = 100.0; },    
\windows,   { ~cpuRatio = 1.0; });
)

TempoClock.tempo = 500/50;
~synthIsOn = 0;


Tdef(~ids[45.rand.floor] ).asStream.play(quant:1);
Tdef(~ids[25] ).asStream.play(quant:1);

~synthbuffertorden.free;
// ~buffertordenOn = 0;



