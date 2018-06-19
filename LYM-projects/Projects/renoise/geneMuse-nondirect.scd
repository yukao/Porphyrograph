// OSC messages receivers //////////////////////////////////////////////////////////////
// inputs:
s.quit;

(
Server.default.options.inDevice_("ASIO : Fast Track Pro ASIO");
Server.default.options.outDevice_("ASIO : Fast Track Pro ASIO");
s.options.maxNodes_(12000);
s.options.memSize_(170000); 
s.boot;
)

(

~buffertorden;
~buffertordenOn = 0;
~synthbuffertorden;

~delayroom = 0;
~audiobus = Bus.audio(s,2); 
~nbSensorMesssages = 0;
~currentSetup = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25];

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
// ~path = "G:/home-win/LYM/geneMuse/sounds/";
// ~path = "/home/jacquemi/3D/LYM/geneMuse/sounds/";
~path = "G:/home-win/LYM/geneMuse/sounds/";

//GN backtrack
~buffertorden = Buffer.read(s, ~path ++ "GN_backtrack1.wav");
SynthDef(\bufferBTDCont, { arg out = 0, volume=1.0;
    Out.ar( [0,1],PlayBuf.ar(1, ~buffertorden, BufRateScale.kr(~buffertorden))*2*volume)
}).send(s);

~buffers = [
    // ~path ++ "samples_part1/GNgroups_part1-001.wav",//1 2 6 7
    // ~path ++ "samples_part1/GNgroups_part1-002.wav",//4 5 9 10
    // ~path ++ "samples_part1/GNgroups_part1-003.wav",//16 17 21 22
    // ~path ++ "samples_part1/GNgroups_part1-004.wav",//19 20 24 25
    // ~path ++ "samples_part1/GNgroups_part1-005.wav",//8 12 14 18
    // ~path ++ "samples_part1/GNgroups_part1-006.wav",//3 11 15 23

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
    ~path ++ "samples_part1/sample_part1-005.wav", // 17
    ~path ++ "samples_part1/sample_part1-006.wav", // 18
    ~path ++ "samples_part1/sample_part1-001.wav", // 19
    ~path ++ "samples_part1/sample_part1-002.wav", // 20
    ~path ++ "samples_part1/sample_part1-003.wav", // 21
    ~path ++ "samples_part1/sample_part1-004.wav", // 22
    ~path ++ "samples_part1/sample_part1-005.wav", // 23
    ~path ++ "samples_part1/sample_part1-006.wav", // 24
    ~path ++ "samples_part1/sample_part1-001.wav", // 25

    // ~path ++ "samples_part2/GNgroups_part2-001.wav",//26 27 31 32
    // ~path ++ "samples_part2/GNgroups_part2-002.wav",//29 30 34 35
    // ~path ++ "samples_part2/GNgroups_part2-003.wav",//41 42 46 48
    // ~path ++ "samples_part2/GNgroups_part2-004.wav",//44 45 49 50
    // ~path ++ "samples_part2/GNgroups_part2-005.wav",//33 37 39 43
    // ~path ++ "samples_part2/GNgroups_part2-006.wav",//28 36 40 48

    ~path ++ "samples_part2/sample_part2-001.wav", // 26
    ~path ++ "samples_part2/sample_part2-002.wav", // 27
    ~path ++ "samples_part2/sample_part2-003.wav", // 28
    ~path ++ "samples_part2/sample_part2-004.wav", // 29
    ~path ++ "samples_part2/sample_part2-005.wav", // 30
    ~path ++ "samples_part2/sample_part2-006.wav", // 31
    ~path ++ "samples_part2/sample_part2-007.wav", // 32
    ~path ++ "samples_part2/sample_part2-008.wav", // 33
    ~path ++ "samples_part2/sample_part2-009.wav", // 34
    ~path ++ "samples_part2/sample_part2-010.wav", // 35
    ~path ++ "samples_part2/sample_part2-011.wav", // 36
    ~path ++ "samples_part2/sample_part2-012.wav", // 37
    ~path ++ "samples_part2/sample_part2-013.wav", // 38
    ~path ++ "samples_part2/sample_part2-014.wav", // 39
    ~path ++ "samples_part2/sample_part2-015.wav", // 40
    ~path ++ "samples_part2/sample_part2-001.wav", // 41
    ~path ++ "samples_part2/sample_part2-002.wav", // 42
    ~path ++ "samples_part2/sample_part2-003.wav", // 43
    ~path ++ "samples_part2/sample_part2-004.wav", // 44
    ~path ++ "samples_part2/sample_part2-005.wav", // 45
    ~path ++ "samples_part2/sample_part2-006.wav", // 46
    ~path ++ "samples_part2/sample_part2-007.wav", // 47
    ~path ++ "samples_part2/sample_part2-008.wav", // 48
    ~path ++ "samples_part2/sample_part2-009.wav", // 49
    ~path ++ "samples_part2/sample_part2-010.wav", // 50

    // ~path ++ "samples_part3/GNgroups_part3-001.wav",//51 52 56 57
    // ~path ++ "samples_part3/GNgroups_part3-002.wav",//54 55 59 60
    // ~path ++ "samples_part3/GNgroups_part3-003.wav",//66 67 71 72
    // ~path ++ "samples_part3/GNgroups_part3-004.wav",//69 70 74 75
    // ~path ++ "samples_part3/GNgroups_part3-005.wav",//58 62 64 68
    // ~path ++ "samples_part3/GNgroups_part3-006.wav" //53 61 65 73

    ~path ++ "samples_part3/sample_part3-001.wav", // 51
    ~path ++ "samples_part3/sample_part3-002.wav", // 52
    ~path ++ "samples_part3/sample_part3-003.wav", // 53
    ~path ++ "samples_part3/sample_part3-004.wav", // 54
    ~path ++ "samples_part3/sample_part3-005.wav", // 55
    ~path ++ "samples_part3/sample_part3-006.wav", // 56
    ~path ++ "samples_part3/sample_part3-007.wav", // 57
    ~path ++ "samples_part3/sample_part3-008.wav", // 58
    ~path ++ "samples_part3/sample_part3-009.wav", // 59
    ~path ++ "samples_part3/sample_part3-010.wav", // 60
    ~path ++ "samples_part3/sample_part3-011.wav", // 61
    ~path ++ "samples_part3/sample_part3-012.wav", // 62
    ~path ++ "samples_part3/sample_part3-013.wav", // 63
    ~path ++ "samples_part3/sample_part3-014.wav", // 64
    ~path ++ "samples_part3/sample_part3-015.wav", // 65
    ~path ++ "samples_part3/sample_part3-016.wav", // 66
    ~path ++ "samples_part3/sample_part3-017.wav", // 67
    ~path ++ "samples_part3/sample_part3-018.wav", // 68
    ~path ++ "samples_part3/sample_part3-019.wav", // 69
    ~path ++ "samples_part3/sample_part3-020.wav", // 70
    ~path ++ "samples_part3/sample_part3-021.wav", // 71
    ~path ++ "samples_part3/sample_part3-022.wav", // 72
    ~path ++ "samples_part3/sample_part3-023.wav", // 73
    ~path ++ "samples_part3/sample_part3-024.wav", // 74
    ~path ++ "samples_part3/sample_part3-025.wav", // 75
    
    ~path ++ "samples_part1/GNgroups_part1-001.wav",//1 2 6 7
    ~path ++ "samples_part1/GNgroups_part1-002.wav",//4 5 9 10
    ~path ++ "samples_part1/GNgroups_part1-003.wav",//16 17 21 22
    ~path ++ "samples_part1/GNgroups_part1-004.wav",//19 20 24 25
    ~path ++ "samples_part1/GNgroups_part1-005.wav",//8 12 14 18
    ~path ++ "samples_part1/GNgroups_part1-006.wav",//3 11 15 23

    ~path ++ "samples_part2/GNgroups_part2-001.wav",//26 27 31 32
    ~path ++ "samples_part2/GNgroups_part2-002.wav",//29 30 34 35
    ~path ++ "samples_part2/GNgroups_part2-003.wav",//41 42 46 48
    ~path ++ "samples_part2/GNgroups_part2-004.wav",//44 45 49 50
    ~path ++ "samples_part2/GNgroups_part2-005.wav",//33 37 39 43
    ~path ++ "samples_part2/GNgroups_part2-006.wav",//28 36 40 48

    ~path ++ "samples_part3/GNgroups_part3-001.wav",//51 52 56 57
    ~path ++ "samples_part3/GNgroups_part3-002.wav",//54 55 59 60
    ~path ++ "samples_part3/GNgroups_part3-003.wav",//66 67 71 72
    ~path ++ "samples_part3/GNgroups_part3-004.wav",//69 70 74 75
    ~path ++ "samples_part3/GNgroups_part3-005.wav",//58 62 64 68
    ~path ++ "samples_part3/GNgroups_part3-006.wav" //53 61 65 73
].collect { |path| Buffer.read(s, path) };

//James' instrument
SynthDef(\PlayBuffer, { |start = 0, time=0.2, bufnum, rate = 1, amp , attack = 0.001, decay = 0.002|
    var sig;
    sig = PlayBuf.ar(1, bufnum, rate * BufRateScale.kr(bufnum), 1, start, 0)
    	* EnvGen.kr(Env.linen(attack, time, decay), doneAction:2) *amp ;
    Out.ar(~audiobus, Pan2.ar(sig, [0,1], amp ));
}).add;

~ids=[
    "t1","t2","t3","t4","t5","t6","t7","t8","t9","t10",
    "t11","t12","t13","t14","t15","t16","t17","t18","t19","t20",
    "t21","t22","t23","t24","t25",

    "t26","t27","t28","t29","t30","t31","t32","t33","t34","t35",
    "t36","t37","t38","t39","t40","t41","t42","t43","t44","t45",
    "t46","t47","t48","t49","t50",

    "t51","t52","t53","t54","t55","t56","t57","t58","t59","t60",
    "t61","t62","t63","t64","t65","t66","t67","t68","t69","t70",
    "t71","t72","t73","t74","t75",

    "G1","G2","G3","G4","G5","G6",
    "G7","G8","G9","G10","G11","G12",
    "G13","G14","G15","G16","G17","G18"];
~idsAmp=[
    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,

    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,

    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,

    0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,0.25,
    0.25,0.25,0.25,0.25,0.25,0.25];
~idsQuant=[
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,

    1,2,3,4,1,1,2,3,4.1,
    1,2,3,4,1,1,2,3,4.1,
    1,2,3,4,1,

    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,


    1,1,1,1,1,1,
    1,1,1,1,1,1,
    1,1,1,1,1,1];

~ids.do({ arg item, i; Tdef(item, { var z; z = Synth(\PlayBuffer, [\amp , ~idsAmp[i] , \bufnum,  ~buffers[i], 
                        \time, 0.5 / Pfunc { TempoClock.tempo }] );z.release}
); });

/////////////////////////////////////////////////////////////////////////////////
// Delay + Room

SynthDef(\delay_reverb, { arg volume = 0.0 , maxdtime = 20, 
        ddecay = 20, delay = 2, mix = 0.02 , room = 0.02;
    var source;
    source = In.ar(~audiobus) * volume;

    // delay
    source = CombN.ar( source , maxdtime , delay, ddecay )  + source ;
    
    // reverb
    source = FreeVerb.ar( source, mix, room) + source * 0.4 ; 

    Out.ar( [0,1], source );
}).send(s); 

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
        msg[2].postln;
        
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

/////////////////////////////////////////////////////////////////////////////////////
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

OSCresponder.new(nil, "/PDdelayroom_off", 
    { 
        arg time, resp, msg; 
        // time.postln;
        // resp.postln;
        msg.postln; 
        ~delayroom.free;
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
// sensor messages reading and playing
OSCresponder.new(nil, "/sensors", 
    { 
        arg time, resp, msg; 
        // time.postln;
        // resp.postln;
        // msg.postln; 
        // msg[1].postln;  // red sensor 0
        // msg[4].postln;  // red sensor 1
        // msg[7].postln;  // red sensor 2
        // msg[10].postln;  // red sensor 3
        ~totAmp = msg[43 + 1];
        if( ~totAmp > 4.0 , {
            ~totAmp = 5.0 - ((~totAmp - 3.0) ** 0.1);
        }
        ,{
            ~totAmp = 4.0;
        });

        // (msg[43 + 1]+" "+~totAmp).postln;

        if( (s.avgCPU < 0.3)  && (s.peakCPU < 0.3) ,
        {
          for (25, 42, { arg i; 
              if( msg[i + 1] > 0, { j = i + 75 - 25; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
          }); // sample aliases
          for (0, 24, { arg i; 
             if( msg[i + 1] > 0, { j = ~currentSetup[i]-1; ~idsAmp[j]=(msg[i + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
          }); // individual samples
        },
        {
            if( (s.avgCPU < 0.5)  && (s.peakCPU < 0.5) ,
            {
                for (25, 42, { arg i; 
                   if( msg[i + 1] > 0, { j = i + 75 - 25; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
                }); // sample aliases
                ~rest = ~nbSensorMesssages % 6;
                if( msg[~rest + 1] > 0, { j = ~currentSetup[~rest]-1; ~idsAmp[j]=(msg[~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                if( msg[2 * ~rest + 1] > 0, { j = ~currentSetup[2 * ~rest]-1; ~idsAmp[j]=(msg[2 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                if( msg[3 * ~rest + 1] > 0, { j = ~currentSetup[3 * ~rest]-1; ~idsAmp[j]=(msg[3 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                if( msg[4 * ~rest + 1] > 0, { j = ~currentSetup[4 * ~rest]-1; ~idsAmp[j]=(msg[4 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
            },
            {
                if( (s.avgCPU < 0.6)  && (s.peakCPU < 0.6) ,
                {
                    for (25, 42, { arg i; 
                      if( msg[i + 1] > 0, { j = i + 75 - 25; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
                    }); // sample aliases
                    ~rest = ~nbSensorMesssages % 12;
                    if( msg[~rest + 1] > 0, { j = ~currentSetup[~rest]-1; ~idsAmp[j]=(msg[~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                    if( msg[2 * ~rest + 1] > 0, { j = ~currentSetup[2 * ~rest]-1; ~idsAmp[j]=(msg[2 * ~rest + 1]/~totAmp);Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
                },
                {
                    if( (s.avgCPU < 0.7)  && (s.peakCPU < 0.7) ,
                    {
                        ~rest = ~nbSensorMesssages % 6;
                        if( msg[~rest + 25 + 1] > 0, { i = ~rest + 25; j = ~rest + 75; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                        if( msg[2 * ~rest + 25 + 1] > 0, { i = 2 * ~rest + 25; j = 2 * ~rest + 75; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])});
                        if( msg[3 * ~rest + 25 + 1] > 0, { i = 3 * ~rest + 25; j = 3 * ~rest + 75; ~idsAmp[j]=(msg[i + 1]/(4*~totAmp));Tdef(~ids[j]).asStream.play(quant:~idsQuant[j])})
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
        // time.post
        // resp.postln;
		for (0, 24, { arg i; ~currentSetup[i] = msg[i + 1]; } );
        ~currentSetup.postln;
    } 
).add;

// tempo clock change
OSCresponder.new(nil, "/TempoClock", 
    { 
        arg time, resp, msg; 
        // time.postln;
        // resp.postln;
        TempoClock.tempo = msg[1].round(1)/50;
        TempoClock.tempo.postln;
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

//~num = 0;
~drumlooper = Pbind(
    \dur, Pseq([0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5], inf),
    \amp, 1,
    \bufnum, ~buffers[21],
    \x, Pseq(#[1,2,3,4,5,6,7,8], inf),
    \begin, Pfunc {|ev| 
                d.virchor.sendMsg("/beat "+ev[\x]);
                (ev[\x]).postln;
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

        ~buffertordenOn = 1;
        ~synthbuffertorden = Synth(\bufferBTDCont);
        ~synthbuffertorden.set(\volume, 0.5);
        Pchain(~baseP, ~drumlooper ).play(quant: 4);
    } 
).add;

///////////////////////////////////////////////////////////////////////////////////
// OLD STUFF
///////////////////////////////////////////////////////////////////////////////////
// Tdef(~ids[79.rand.floor] ).asStream.play(quant:1);
// Tdef(~ids[50] ).asStream.play(quant:1);

// TempoClock.tempo = 400/50;
TempoClock.tempo = 500/50;




//TempoClock.tempo = 50/50; //denne svare til 120 tempo paa min loopstation
//TempoClock.tempo = 100/50; //denne svare til 120 tempo paa min loopstation dobbelt tempo end 50/50
