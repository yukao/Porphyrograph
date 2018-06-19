// OSC messages receivers //////////////////////////////////////////////////////////////
// inputs:
s.quit;

(
Server.default.options.inDevice_("ASIO : Fast Track Pro ASIO")
Server.default.options.outDevice_("ASIO : Fast Track Pro ASIO")
s.options.maxNodes_(40960);
s.options.memSize_(163840); 
s.boot;
)

(

~delayroom = 0;
~audiobus = Bus.audio(s,2); 

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

~buffers = [
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-001.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-002.wav",//1
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-003.wav",//2
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-004.wav",//3
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-005.wav",//4
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-006.wav",//5
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-007.wav",//6
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-008.wav",//7
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-009.wav",//8
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-010.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-011.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-012.wav",//1
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-013.wav",//2
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-014.wav",//3
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-015.wav",//4
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-016.wav",//5
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-017.wav",//6
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-018.wav",//7
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-019.wav",//8
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-020.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-021.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-022.wav",//1
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-023.wav",//2
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-024.wav",//3
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-025.wav",//4
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-026.wav",//5
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-027.wav",//6
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-028.wav",//7
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-029.wav",//8
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-030.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-031.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-032.wav",//1
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-033.wav",//2
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-034.wav",//3
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-035.wav",//4
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-036.wav",//5
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-037.wav",//6
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-038.wav",//7
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-039.wav",//8
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-040.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-041.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-042.wav",//1
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-043.wav",//2
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-044.wav",//3
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-045.wav",//4
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-046.wav",//5
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-047.wav",//6
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-048.wav",//7
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-049.wav",//8
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-050.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-051.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-052.wav",//1
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-053.wav",//2
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-054.wav",//3
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-055.wav",//4
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-056.wav",//5
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-057.wav",//6
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-058.wav",//7
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-059.wav",//8
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-060.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-061.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-062.wav",//1
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-063.wav",//2
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-064.wav",//3
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-065.wav",//4
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-066.wav",//5
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-067.wav",//6
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-068.wav",//7
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-069.wav",//8
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-070.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-071.wav",//0
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-072.wav",//1
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-073.wav",//2
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-074.wav",//3
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-075.wav",//4
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-076.wav",//5
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-077.wav",//6
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-078.wav",//7
    "G:/home-win/LYM/geneMuse/sounds/GNsamples-079.wav"//8
].collect { |path| Buffer.read(s, path) };

//James' instrument
SynthDef(\PlayBuffer, { |start = 0, time=0.5, bufnum, rate = 1, amp , attack = 0.001, decay = 0.002|
    var sig;
    sig = PlayBuf.ar(1, bufnum, rate * BufRateScale.kr(bufnum), 1, start, 0)
    	* EnvGen.kr(Env.linen(attack, time, decay), doneAction:2) *amp ;
    Out.ar(~audiobus, Pan2.ar(sig, [0,1], amp ));
}).add;

~ids=["t0","t1","t2","t3","t4","t5","t6","t7","t8","t9","t10","t11","t12","t13","t14","t15","t16","t17","t18","t19","t20","t21","t22","t23","t24","t25","t26","t27","t28","t29","t30","t31","t32","t33","t34","t35","t36","t37","t38","t39","t40","t41","t42","t43","t44","t45","t46","t47","t48","t49","t50","t51","t52","t53","t54","t55","t56","t57","t58","t59","t60","t61","t62","t63","t64","t65","t66","t67","t68","t69","t70","t71","t72","t73","t74","t75","t76","t77","t78"];
~idsAmp=[0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25];
~ids.do({ arg item, i; Tdef(item, { var z; z = Synth(\PlayBuffer, [\amp , ~idsAmp[i] , \bufnum,  ~buffers[i], 
                        \time, 0.5 / Pfunc { TempoClock.tempo }] );z.release}
); });
~setup1=[49,28,29,28,49,28,2,2,2,28,29,2,1,2,29,28,2,2,2,28,49,28,29,28,49];
~setup2=[11,5,38,7,11,7,39,43,39,5,38,43,49,43,38,5,39,43,39,7,11,7,38,5,11];
~setup3=[9,21,52,21,9,21,58,20,53,21,52,20,18,20,52,21,53,20,58,21,21,9,52,21,9];
// ~setup50=[51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51];
TempoClock.tempo = 500/50;

// examples
// Tdef(~ids[79.rand.floor]).asStream.play(quant:1);
// ~idsAmp[50]=0.25; Tdef(~ids[50]).asStream.play(quant:1);
// ~idsAmp[50]=1.0; Tdef(~ids[50]).asStream.play(quant:1);

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
// Dinosteps on
OSCresponder.new(nil, "/sensors", 
	{ 
		arg time, resp, msg; 
		// time.postln;
		// resp.postln;
		//msg.postln; 
		// msg[1].postln;  // red sensor 0
		// msg[4].postln;  // red sensor 1
		// msg[7].postln;  // red sensor 2
		// msg[10].postln;  // red sensor 3
		for (0, 24, { arg i; 
			if( msg[3 * i + 3] > 0, { j = ~setup3[i]-1; ~idsAmp[j]=msg[3 * i + 3];Tdef(~ids[j]).asStream.play(quant:1)})
			}); // prints values 3 through 7
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

)


///////////////////////////////////////////////////////////////////////////////////
// OLD STUFF
///////////////////////////////////////////////////////////////////////////////////
Tdef(~ids[79.rand.floor] ).asStream.play(quant:1);
Tdef(~ids[50] ).asStream.play(quant:1);

//TempoClock.tempo = 50/50; //denne svare til 120 tempo paa min loopstation
//TempoClock.tempo = 100/50; //denne svare til 120 tempo paa min loopstation dobbelt tempo end 50/50
