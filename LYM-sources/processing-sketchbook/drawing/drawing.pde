import oscP5.*;
import netP5.*;

OscP5 oscP5;

// final String TouchOSC_IP_address_String = "192.168.2.65";
// final String TouchOSC_IP_address_String = "100.83.161.98";
final String TouchOSC_IP_address_String = "192.168.43.108";

NetAddress TouchOSC_IP_Address;

final int BRUSH_LINE = 0;
final int BRUSH_TRIANGLE = 1;
final int BRUSH_SQUARE = 2;

final float DAMP_MAX = 0.3;

final boolean MIRROR = true;

final int HUE_1 = 15;
final int HUE_2 = 48;
final int HUE_3 = 81;
final int NB_HUE_INDICES = 3;

final int  PEN_BRIGHT    = 0;
final int  PEN_SAT    = 1;
final int  PEN_HUE   = 2;
final int BLUR_RADIUS = 3;
final int  PEN_ON   = 4;
final int  DECAY   = 5;
final int  DYN_ON   = 6;

int currentHueIndex = 2;

int posX = -1000, posY = -1000;
int prec_posX = -1000, prec_posY = -1000;
boolean penWhite = true;
boolean pen_on = true;
boolean clrScreen = true;
boolean invert = false;
boolean dyn_on = false;
int pen_hue = 0;
int curHue = 0;
int pen_bright = 100;
int pen_sat = 0;
int pen_sat_memory = 0;
float trkDecay = 0;
boolean trkDecay_on = false;
int rad = 1;
int curRadius = rad;
int blur_radius = 0;
boolean blur = false;
int currentBrush = BRUSH_LINE;
float accX = 0, accY = 0;
float velX = 0, velY = 0;
float damp = DAMP_MAX;

void setup() {
  smooth();
  noCursor();
  fullScreen(3);
  // size(1024, 768);
  println("Frame size HxW ", width, "x",height);
  background(0);
  colorMode(HSB, 100);
  ellipseMode(CENTER);  // Set ellipseMode to CENTER
  rectMode(CENTER);  // Set rectMode to CENTER

  /* start oscP5, listening for incoming messages at port 9000 */
  oscP5 = new OscP5(this, 9000);

  /* TouchOSC_IP_Address is a NetAddress. a NetAddress takes 2 parameters,
   * an ip address and a port number. TouchOSC_IP_Address is used as parameter in
   * oscP5.send() when sending osc packets to another computer, device, 
   * application. usage see below. for testing purposes the listening port
   * and the port of the remote location address are the same, hence you will
   * send messages back to this sketch.
   */
  TouchOSC_IP_Address = new NetAddress(TouchOSC_IP_address_String, 8000);
  sendAllInitialValues();
  frameRate(30);
}

void sendAllInitialValues() {
  updateInterface(PEN_BRIGHT);
  updateInterface(PEN_SAT);
  updateInterface(PEN_HUE);
  updateInterface(BLUR_RADIUS);
  updateInterface(PEN_ON);
  updateInterface(DECAY);
  updateInterface(DYN_ON);
}

void updateInterface(int var) {
  OscMessage myMessage;
  switch (var){
    case PEN_BRIGHT:
      myMessage = new OscMessage("/pen_BW");
      myMessage.add(int(penWhite)); /* add an int to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
  
      myMessage = new OscMessage("/pen_bright");
      myMessage.add(float(pen_bright)/100); /* add an int to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
    break;
    case PEN_SAT:
      myMessage = new OscMessage("/pen_sat");
      myMessage.add(float(pen_sat)/100); /* add an int to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
    break;
    case PEN_HUE:
      myMessage = new OscMessage("/pen_color");
      myMessage.add(float(curHue)/100); /* add an int to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
    break;
    case BLUR_RADIUS:
      myMessage = new OscMessage("/blur_radius");
      myMessage.add(blur_radius); /* add an int to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
    break;
    case PEN_ON:
      myMessage = new OscMessage("/pen_on");
      myMessage.add(int(pen_on)); /* add an int to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
    break;
    case DECAY:
      myMessage = new OscMessage("/trkDecay_on");
      myMessage.add(int(trkDecay_on)); /* add an int to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
  
      myMessage = new OscMessage("/trkDecay");
      myMessage.add(trkDecay); /* add a float to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
    break;
    case DYN_ON:
      myMessage = new OscMessage("/dyn_on");
      myMessage.add(int(dyn_on)); /* add an int to the osc message */
      oscP5.send(myMessage, TouchOSC_IP_Address); 
    break;
  }
}

void draw() {
  if( clrScreen ) {
    background(0);
    clrScreen = false;
    fill(0);
    rect(width/2, height/2, width+2*curRadius, height+2*curRadius); // draw transparent background
  }
  strokeWeight(0);
  if(trkDecay_on) {
    fill(0, trkDecay);  // fancy fade rendering (transparent background)
    rect(width/2, height/2, width+2, height+2); // draw transparent background
  }

  curRadius = rad;
  curHue = pen_hue;
  if (dyn_on) {
    curRadius = int(rad * ( 1 + 0.1 * sqrt(accX*accX+accY*accY) ));
    curHue = int(pen_hue + sqrt(accX*accX+accY*accY))%100;
    updateInterface(PEN_HUE);
  }

  if (pen_on && posX >= 0 && posY >= 0) {
    if (prec_posX >= 0 && prec_posY >= 0 
      && dist(posX, posY, prec_posX, prec_posY) < 50) {
      switch(currentBrush) {
      case BRUSH_LINE:
        strokeWeight(curRadius);
        // HSV
        if (!dyn_on) {
          stroke(pen_hue, pen_sat, pen_bright);
        } else {
          stroke(curHue, pen_sat, pen_bright);
        }

        if (!MIRROR) {
          line(posX, posY, prec_posX, prec_posY);
        } else {
          line(width - posX, posY, width - prec_posX, prec_posY);
        }
        break;
      case BRUSH_TRIANGLE:
        strokeWeight(0);
        // HSV
        if (!dyn_on) {
          fill(pen_hue, pen_sat, pen_bright);
        } else {
          fill(curHue, pen_sat, pen_bright);
        }
        int w = curRadius/2;
        if (!MIRROR) {
          triangle(posX-w, posY+w, posX-w, posY-w, posX+w, posY);
        } else {
          triangle(width - posX-w, posY+w, width - posX-w, posY-w, width - posX+w, posY);
        }
        break;
      case BRUSH_SQUARE:
        strokeWeight(0);
        // HSV
        if (!dyn_on) {
          fill(pen_hue, pen_sat, pen_bright);
        } else {
          fill(curHue, pen_sat, pen_bright);
        }
        if (!MIRROR) {
          rect(posX, posY, curRadius, curRadius);
        } else {
          rect(width - posX, posY, curRadius, curRadius);
        }
        break;
      default:
        strokeWeight(curRadius);
        // HSV
        if (!dyn_on) {
          stroke(pen_hue, pen_sat, pen_bright);
        } else {
          stroke(curHue, pen_sat, pen_bright);
        }
        if (!MIRROR) {
          line(posX, posY, prec_posX, prec_posY);
        } else {
          line(width - posX, posY, width - prec_posX, prec_posY);
        }
        break;
      }
    }
    prec_posX = posX;
    prec_posY = posY;
  }

  if (dyn_on) {
    if (accX != 0 || accY != 0) {
      velX += accX;
      velY += accY;
    }
    if ( damp > 0 ) {
      velX *= (1 - damp);
      velY *= (1 - damp);
    }
    if (velX != 0 || velY != 0) {
      posX += velX;
      posY += velY;
      if ( posX < 0 ) {
        posX = 0;
        velX *= -1;
      }
      if ( posY < 0 ) {
        posY = 0;
        velY *= -1;
      }
      if ( posX > width ) {
        posX = width;
        velX *= -1;
      }
      if ( posY > height ) {
        posY = height;
        velY *= -1;
      }
    }
  }
  if(blur) {
    filter(BLUR, blur_radius);
    blur = false;
  }
  if(invert) {
    filter(INVERT);
    invert = false;
  }
}

void mouseDragged() {
  posX = mouseX;  // get the first mouse coord
  posY = mouseY; // get the second mouse coord
  redraw();
}

/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {
  // print the address pattern and the typetag of the received OscMessage 
  print("### received an osc message.");
  print(" addrpattern: "+theOscMessage.addrPattern());
  println(" typetag: "+theOscMessage.typetag());

  /* check if theOscMessage has the address pattern we are looking for. */
  if (theOscMessage.checkAddrPattern("/pen_xy")==true) {
    posY = int(height-theOscMessage.get(0).floatValue()*height);  // get the first osc argument
    posX = int(theOscMessage.get(1).floatValue()*width); // get the second osc argument
    // println("received /pen_xy "+posX+", "+posY);
    redraw();
    return;
  } else if (theOscMessage.checkAddrPattern("/pen_radius")==true) {
    rad = int(theOscMessage.get(0).floatValue());  // get the first osc argument
    // println("received /pen_radius "+rad);
    if(rad < 1) {
      rad = 1;
    }
    curRadius = rad;
    return;
  } else if (theOscMessage.checkAddrPattern("/blur_radius")==true) {
    blur_radius = int(theOscMessage.get(0).floatValue());  // get the first osc argument
    updateInterface(BLUR_RADIUS);
    return;
  } else if (theOscMessage.checkAddrPattern("/blur")==true) {
    blur = true;  // get the first osc argument
    return;
  } else if (theOscMessage.checkAddrPattern("/pen_BW")==true) {
    penWhite = !penWhite;
    if(penWhite) {
      pen_bright = 100;
      pen_sat_memory = pen_sat;
      pen_sat = 0;
    }
    else {
      pen_bright = 0;
      pen_sat = pen_sat_memory;
    }
    updateInterface(PEN_BRIGHT);
    updateInterface(PEN_SAT);
    return;
  } else if (theOscMessage.checkAddrPattern("/pen_color")==true) {
    pen_hue = int(theOscMessage.get(0).floatValue()*100);  // get the first osc argument
    curHue = pen_hue;
    updateInterface(PEN_HUE);
    return;
  } else if (theOscMessage.checkAddrPattern("/pen_bright")==true) {
    pen_bright = int(theOscMessage.get(0).floatValue()*100);  // get the first osc argument
    updateInterface(PEN_BRIGHT);
    return;
  } else if (theOscMessage.checkAddrPattern("/pen_sat")==true) {
    pen_sat = int(theOscMessage.get(0).floatValue()*100);  // get the first osc argument
    pen_sat_memory = pen_sat;
    updateInterface(PEN_SAT);
    return;
  } else if (theOscMessage.checkAddrPattern("/trkDecay")==true) {
    trkDecay = theOscMessage.get(0).floatValue();  // get the first osc argument
    updateInterface(DECAY);
    return;
  } else if (theOscMessage.checkAddrPattern("/trkDecay_on")==true) {
    trkDecay_on = boolean(int(theOscMessage.get(0).floatValue()));
    updateInterface(DECAY);
    return;
  } else if (theOscMessage.checkAddrPattern("/pen_on")==true) {
    pen_on = boolean(int(theOscMessage.get(0).floatValue()));
    posX = -1000; 
    posY = -1000;
    prec_posX = -1000; 
    prec_posY = -1000;
    updateInterface(PEN_ON);
    return;
  } else if (theOscMessage.checkAddrPattern("/clrScreen")==true) {
    clrScreen = true;
    return;
  } else if (theOscMessage.checkAddrPattern("/invert")==true) {
    invert = true;
    return;
  } else if (theOscMessage.checkAddrPattern("/pen_brush_plus")==true) {
    currentBrush = (currentBrush + 1) % (BRUSH_SQUARE + 1);
    return;
  } else if (theOscMessage.checkAddrPattern("/pen_color_plus")==true) {
    currentHueIndex = (currentHueIndex + 1) % NB_HUE_INDICES;
    switch (currentHueIndex)
    {
    case 0:
      pen_hue = HUE_1;
      break;
    case 1:
      pen_hue = HUE_2;
      break;
    case 2:
      pen_hue = HUE_3;
      break;
    default:
      pen_hue = HUE_1;
      break;
    }
    curHue = pen_hue;
    updateInterface(PEN_HUE);
    return;
  } else if (theOscMessage.checkAddrPattern("/accxyz")==true) {
    float val_accY = theOscMessage.get(0).floatValue();  // get the first osc argument
    float val_accX = theOscMessage.get(1).floatValue();  // get the 2nd osc argument
    if (abs(val_accX) >1 || abs(val_accY) >1 ) {
      // println("received /accxyz "+val_accX+", "+val_accY);
      accX = val_accX;
      accY = val_accY;
    }
    return;
  } else if (theOscMessage.checkAddrPattern("/damp")==true) {
    damp = DAMP_MAX * theOscMessage.get(0).floatValue();  // get the first osc argument
    return;
  } else if (theOscMessage.checkAddrPattern("/dyn_on")==true) {
    dyn_on = boolean(int(theOscMessage.get(0).floatValue()));
     updateInterface(DYN_ON);
   return;
  }
}
