// CA OFFSETS
const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E NE N NW
                                 {1,1},{-1,1},{-1,-1},{1,-1}};  // W SW S SE
const vec2 neighborOffsetsPaths[8] = {{1,0},{2,0},{0,1},{0,2},      // EE EE N NN
                                      {-1,0},{-2,0},{0,-1},{0,-2}};  // W WW S SS
const vec2 neighborOffsetsNonPaths[16] = {{1,1},{2,1},{1,2},{2,2},
                                          {-1,-1},{-2,-1},{-1,-2},{-2,-2},
                                          {1,-1},{2,-1},{1,-2},{2,-2},
                                          {-1,1},{-2,1},{-1,2},{-2,2}};  // W WW S SS
const float neighborDistances[8]={1.0,0.707,1.0,0.707,1.0,0.707,1.0,0.707};


const float shadePrey[5] = {0,0.4,              // dead predator/prey
                                 0.6, 0.8,  1.0};       // prey hungry/full predator
const float inverseShadePrey[5] = {0.0,2.5,              // dead predator/prey
                                 1.67,1.25,  1.0};       // prey hungry/full predator
// const vec3 colorPrey[5] = {{0,0,0},{0.25,0.25,0.25},   // dead predator/prey
//                            {0.75,0.75,0.75},           // prey
//                            {0.5,0.5,0.5},{1,1,1}};  // hungry/full predator
const float shadeDisloc[5] = {0,0.4,              // empty new/old border
                                 0.6, 0.8,  1.0};       // new/old nucleus
const float inverseShadeDisloc[5] = {0.0,2.5,              // empty new/old border
                                 1.67,1.25,  1.0};       // new/old nucleus

// const vec3 colorDisloc[5] = {{0,0,0},     // empty
//                              {1.0,1.0,1.0},{0.6,0.6,0.6},   // new/old border
//                              {1.0,1.0,1.0},{0.3,0.3,0.3}};  // new/old nucleus

const float shadeProtocell[4] = {0,               // empty
                                 0.4,0.8,         // X Y
                                 1.0};            // Z
const float inverseShadeProtocell[4] = {0,                 // empty
                                        2.5, 1.25,         // X Y
                                        1.0};            // Z
const float shadeSocial[3] = {0,               // empty
                                 0.5, 1.0};    // Coorperator / Defector
const float inverseShadeSocial[3] = {0,                 // empty
                                        2.0 , 1.0};     // Coorperator / Defector


vec4 neighborValuesHV[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CELLULAR AUTOMATA UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

float payoffCalc( float state1 , float state2) {
  if(state1==1.0) {
    if (state2==1.0) {
      return CAParams1;
    }
    else {
      return CAParams3;
    }
  }
  else {
    if (state2==1.0) {
      return CAParams2;
    }
    else {
      return CAParams4;
    }
  }
}

void CA_out( vec4 currentCA ) {
//////////////////////////////////////////////////////
  // PREY/PREDATOR
  if( CAType == CA_PREY ) {
    int nbSurroundingHungryPredators =
      (round(neighborValues[0].a) == 3? 1:0) +
      (round(neighborValues[1].a) == 3? 1:0) +
      (round(neighborValues[2].a) == 3? 1:0) +
      (round(neighborValues[3].a) == 3? 1:0) +
      (round(neighborValues[4].a) == 3? 1:0) +
      (round(neighborValues[5].a) == 3? 1:0) +
      (round(neighborValues[6].a) == 3? 1:0) +
      (round(neighborValues[7].a) == 3? 1:0);
    
    int nbSurroundingFullPredators =
      (round(neighborValues[0].a) == 4? 1:0) +
      (round(neighborValues[1].a) == 4? 1:0) +
      (round(neighborValues[2].a) == 4? 1:0) +
      (round(neighborValues[3].a) == 4? 1:0) +
      (round(neighborValues[4].a) == 4? 1:0) +
      (round(neighborValues[5].a) == 4? 1:0) +
      (round(neighborValues[6].a) == 4? 1:0) +
      (round(neighborValues[7].a) == 4? 1:0);
    
    int nbSurroundingPreys =
      (round(neighborValues[0].a) == 2? 1:0) +
      (round(neighborValues[1].a) == 2? 1:0) +
      (round(neighborValues[2].a) == 2? 1:0) +
      (round(neighborValues[3].a) == 2? 1:0) +
      (round(neighborValues[4].a) == 2? 1:0) +
      (round(neighborValues[5].a) == 2? 1:0) +
      (round(neighborValues[6].a) == 2? 1:0) +
      (round(neighborValues[7].a) == 2? 1:0);
    
    int nbNeighbors =
      (round(neighborValues[0].a) != 0? 1:0) +
      (round(neighborValues[1].a) != 0? 1:0) +
      (round(neighborValues[2].a) != 0? 1:0) +
      (round(neighborValues[3].a) != 0? 1:0) +
      (round(neighborValues[4].a) != 0? 1:0) +
      (round(neighborValues[5].a) != 0? 1:0) +
      (round(neighborValues[6].a) != 0? 1:0) +
      (round(neighborValues[7].a) != 0? 1:0);
    
    uint nbStates = 5;
    uint newState = 0;
    
     vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadePrey[int(max(0,neighborValues[0].a))]) +
      (neighborValues[1].rgb * inverseShadePrey[int(max(0,neighborValues[1].a))]) +
      (neighborValues[2].rgb * inverseShadePrey[int(max(0,neighborValues[2].a))]) +
      (neighborValues[3].rgb * inverseShadePrey[int(max(0,neighborValues[3].a))]) +
      (neighborValues[4].rgb * inverseShadePrey[int(max(0,neighborValues[4].a))]) +
      (neighborValues[5].rgb * inverseShadePrey[int(max(0,neighborValues[5].a))]) +
      (neighborValues[6].rgb * inverseShadePrey[int(max(0,neighborValues[6].a))]) +
      (neighborValues[7].rgb * inverseShadePrey[int(max(0,neighborValues[7].a))]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadePrey[int(max(0,currentCA.a))]) +
      (neighborValues[0].rgb * inverseShadePrey[int(max(0,neighborValues[0].a))]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadePrey[int(max(0,neighborValues[ind].a))];
      if( graylevel(newColorFactor) > graylevel(maxColorFactors) ) {
        maxColorFactors = newColorFactor;
      }
    }

    vec3 averageSurrounding;
    if( nbNeighbors > 0 ) {
      if( CAcolorSpread ) {
        averageSurrounding = maxColorFactors;
      }
      else {
        averageSurrounding = gatherColorFactors / nbNeighbors;
      }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }

   // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      out4_CA.a = round(randomCA.x * (nbStates+1)); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      out4_CA.rgb = shadePrey[newState].r * currentCA.rgb;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      newState = state;

      // dead predator 
      if( state == 0 ) {
        // prey birth probability if preys are in neighborhood
        if( nbSurroundingPreys > 0
            && pow( randomCA.x , nbSurroundingPreys ) 
               < CAParams1 ) {
          newState = 2; // becomes a prey
        }
        // else {
        //   out4_CA.a = currentCA.a; // remains dead predator
        // }
      }
      // dead prey 
      else if( state == 1 ) {
        // predator birth probability if full predators are in neighborhood
        if( nbSurroundingFullPredators > 0
            && pow( randomCA.x , nbSurroundingFullPredators ) 
               < CAParams2 ) {
          newState = 3; // becomes a hungry predator
        }
        // else {
        //   newState = currentCA.a; // remains dead prey
        // }
      }
      // prey 
      else if( state == 2 ) {
        // death probability if hungry predators are in neighborhood
        if( nbSurroundingHungryPredators > 0
            && pow( randomCA.x , nbSurroundingHungryPredators ) 
               < CAParams3 ) {
          newState = 1; // dead prey
        }
        // else {
        //   newState = currentCA.a; // survives
        // }
      }
      // hungry predator 
      else if( state == 3 ) {
        // probability of eating if preys are in the neighborhood
        if( nbSurroundingPreys > 0
            && pow( randomCA.x , nbSurroundingPreys ) 
               < CAParams3 ) {
          newState = 4; // survives in state full
        }
        else { // dies with a certain probability
          if( randomCA.x < CAParams4 ) {
            newState = 0;  // dead predator
          }
          // else {
          //   newState = currentCA.a; // survives
          // }
        }
      }
      // full predator 
      else if( state == 4 ) {
        // probability of becoming hungry
        if( randomCA.x < CAParams5 ) {
          newState = 3; // becomes hungry
        }
        // else { 
        //   newState = currentCA.a; // stays in state full
        // }
      }
      else {
        newState = 0;
      }
      // newState = state;
      // out4_CA.a = float(newState);
      out4_CA.a = float(newState);
      averageSurrounding = clamp( averageSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadePrey[newState] * averageSurrounding;
      // out4_CA.rgb = vec3(shadePrey[newState]);
    }
  }

  //////////////////////////////////////////////////////
  // DISLOCATION
  // parameter values: 0.314  0.606 0.354 0.409
  else if( CAType == CA_DISLOCATION ) {
    int nbSurroundingNewBorders =
      (neighborValues[0].a == 1? 1:0) +
      (neighborValues[1].a == 1? 1:0) +
      (neighborValues[2].a == 1? 1:0) +
      (neighborValues[3].a == 1? 1:0) +
      (neighborValues[4].a == 1? 1:0) +
      (neighborValues[5].a == 1? 1:0) +
      (neighborValues[6].a == 1? 1:0) +
      (neighborValues[7].a == 1? 1:0);
    
    int nbSurroundingOldBorders =
      (neighborValues[0].a == 2? 1:0) +
      (neighborValues[1].a == 2? 1:0) +
      (neighborValues[2].a == 2? 1:0) +
      (neighborValues[3].a == 2? 1:0) +
      (neighborValues[4].a == 2? 1:0) +
      (neighborValues[5].a == 2? 1:0) +
      (neighborValues[6].a == 2? 1:0) +
      (neighborValues[7].a == 2? 1:0);

    vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadeDisloc[int(max(0,neighborValues[0].a))]) +
      (neighborValues[1].rgb * inverseShadeDisloc[int(max(0,neighborValues[1].a))]) +
      (neighborValues[2].rgb * inverseShadeDisloc[int(max(0,neighborValues[2].a))]) +
      (neighborValues[3].rgb * inverseShadeDisloc[int(max(0,neighborValues[3].a))]) +
      (neighborValues[4].rgb * inverseShadeDisloc[int(max(0,neighborValues[4].a))]) +
      (neighborValues[5].rgb * inverseShadeDisloc[int(max(0,neighborValues[5].a))]) +
      (neighborValues[6].rgb * inverseShadeDisloc[int(max(0,neighborValues[6].a))]) +
      (neighborValues[7].rgb * inverseShadeDisloc[int(max(0,neighborValues[7].a))]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeDisloc[int(max(0,currentCA.a))]) +
      (neighborValues[0].rgb * inverseShadeDisloc[int(max(0,neighborValues[0].a))]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeDisloc[int(max(0,neighborValues[ind].a))];
      if( graylevel(newColorFactor) > graylevel(maxColorFactors) ) {
        maxColorFactors = newColorFactor;
      }
    }

    vec3 averageSurrounding;
    int nbNeighbors = nbSurroundingNewBorders + nbSurroundingOldBorders;
    if( nbNeighbors > 0 ) {
      if( CAcolorSpread ) {
        averageSurrounding = maxColorFactors;
      }
      else {
        averageSurrounding = gatherColorFactors / nbNeighbors;
      }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }

    // 0 empty
    // 1 new border
    // 2 old border: border at preceding state
    // 3 new nucleus
    // 4 new nucleus at preceding step
    uint nbStates = 5;
    uint newState = 0;

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      out4_CA.a = randomCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      // newState = 3;
      out4_CA.rgb = shadeDisloc[newState].r * currentCA.rgb;
    }
    else {
      int indexOld = -1;
      int firstindex = int(randomCA.x * 8);
      // looks for an old border around
      for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
        if( neighborValues[ind % 8].a == 2 ) {
          indexOld = ind;
        }
      }
      int indexNew = -1;
      firstindex = int(randomCA.y * 8);
      // looks for a new border around
      for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
        if( neighborValues[ind % 8].a == 1 ) {
          indexNew = ind;
        }
      }

      uint state = int(clamp(currentCA.a,0,nbStates));
      // parameters
      // w: fluidity (penetration of old borders between cells)
      // z : celle density
      // y : permeability
      // x : freezing/dissolve

      // empty cell transformation
      if( state ==  0 ) {
        newState = 0; // stays empty
        // found a new border at indexNew
        if( nbSurroundingNewBorders == 1  ) {
          // prolongs the border
          newState = 1; // new border
        }
        else if( nbSurroundingOldBorders == 1 
                && randomCA.y < CAParams2 ) {
          // prolongs the border
          newState = 2; // old border
        }
        else if( nbSurroundingNewBorders >= CAParams1 * 8.0  ) {
          // creates a nucleus
          newState = 3; // nucleus
        }
      }
      // new border growth: new border pushed by old border
      else if( state ==  1 ) {
        newState = 1; // stays a border
        if( nbSurroundingNewBorders >= CAParams5 * 10.0 ) {
          newState = 3; //makes a nucleus
        }
        // found an old border at indexOld
        else if( indexOld >= 0 && neighborValues[(indexOld + 4) % 8].a == 0 ) {
          // a new border can be established in diametrically opposed cell
          newState = 2; // becomes an old border
        }
      }
      // border growth: old border pulled by new border
      else if( state ==  2 ) {
        newState = 2; // stays old border
        if( nbSurroundingOldBorders >= CAParams6 * 10.0 ) {
          newState = 0; //makes a hole
        }
        else if( indexNew >= 0 ) {
          newState = 0; // becomes an empty cell
        }
      }
       // nucleus
      else if( state ==  3 ) {
        newState = 3; // stays a nucleus
        if( nbSurroundingOldBorders + nbSurroundingNewBorders 
             >= CAParams4 * 16.0 ) {
          newState = 0; //makes a hole
        }
        else 
          if( randomCA.y < CAParams3 ) {
          newState = 1; // becomes a border
        }
      }
      // newState = state;
      out4_CA.a = float(newState);
      averageSurrounding = clamp( averageSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeDisloc[newState] * averageSurrounding;
      // out4_CA.rgb = colorDisloc[newState];
    }
    // const vec3 colorDisloc[5] = {{0.3,0.3,0.3},     // emptry
    //                              {0,1,0},{0,0,1},   // new/old border
    //                              {1,0,0},{1,1,0}};  // new/old nucleus
  }

  //////////////////////////////////////////////////////
  // PROTOCELLS
  else 
    if( CAType == CA_PROTOCELLS ) {

    int nbNeighbors =
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0) +
      (neighborValues[4].a > 0? 1:0) +
      (neighborValues[5].a > 0? 1:0) +
      (neighborValues[6].a > 0? 1:0) +
      (neighborValues[7].a > 0? 1:0);
/*
    vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadeProtocell[int(max(0,neighborValues[0].a))]) +
      (neighborValues[1].rgb * inverseShadeProtocell[int(max(0,neighborValues[1].a))]) +
      (neighborValues[2].rgb * inverseShadeProtocell[int(max(0,neighborValues[2].a))]) +
      (neighborValues[3].rgb * inverseShadeProtocell[int(max(0,neighborValues[3].a))]) +
      (neighborValues[4].rgb * inverseShadeProtocell[int(max(0,neighborValues[4].a))]) +
      (neighborValues[5].rgb * inverseShadeProtocell[int(max(0,neighborValues[5].a))]) +
      (neighborValues[6].rgb * inverseShadeProtocell[int(max(0,neighborValues[6].a))]) +
      (neighborValues[7].rgb * inverseShadeProtocell[int(max(0,neighborValues[7].a))]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeProtocell[int(max(0,currentCA.a))]) +
      (neighborValues[0].rgb * inverseShadeProtocell[int(max(0,neighborValues[0].a))]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeProtocell[int(max(0,neighborValues[ind].a))];
      if( graylevel(newColorFactor) > graylevel(maxColorFactors) ) {
        maxColorFactors = newColorFactor;
      }
    }

    vec3 averageSurrounding;
    if( nbNeighbors > 0 ) {
      // if( CAcolorSpread ) {
        averageSurrounding = maxColorFactors;
      // }
      // else {
      //   averageSurrounding = gatherColorFactors / nbNeighbors;
      // }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }
*/
    vec3 maxSurrounding = (neighborValues[0].rgb);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb;
      if( graylevel(newColorFactor) > graylevel(maxSurrounding) ) {
        maxSurrounding = newColorFactor;
      }
    }
    if( graylevel(currentCA.rgb) > graylevel(maxSurrounding) ) {
      maxSurrounding = currentCA.rgb;
    }

    vec3 gatherColorFactors =
      (neighborValues[0].rgb) +
      (neighborValues[1].rgb) +
      (neighborValues[2].rgb) +
      (neighborValues[3].rgb) +
      (neighborValues[4].rgb) +
      (neighborValues[5].rgb) +
      (neighborValues[6].rgb) +
      (neighborValues[7].rgb);

    vec3 averageSurrounding;
    if( nbNeighbors > 0 ) {
      // if( CAcolorSpread ) {
        averageSurrounding = (gatherColorFactors + currentCA.rgb)/8;
      // }
      // else {
      //   averageSurrounding = gatherColorFactors / nbNeighbors;
      // }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }

    // 0 empty
    // 1 X
    // 2 Y
    // 3 Z
    uint nbStates = 4;
    uint newState = 0;

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      out4_CA.a = randomCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      newState = 3;
      out4_CA.rgb = shadeProtocell[newState].r * currentCA.rgb;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      newState = state;
      // parameters CA thresholds
      // CAParams1: px - X duplication from X+Y (randomCA.x)
      // CAParams2: py - Y duplication from Y+Z (randomCA.y)
      // CAParams3: pz - Z duplication from Z+X (randomCA.z)

      // empty cell: replication or migration from neighboring cells
      if( state ==  0 ) { // empty
        newState = 0; // stays empty
        int firstindex = clamp(int(randomCA.x * 8),0,8);
        // has a neighboring X and a neighboring Y slot
        for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
          if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 2 
              && randomCA.y < CAParams1) {
            newState = 1; // new X
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Y and a neighboring Z slot
          int firstindex = clamp(int(randomCA.z * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 6 
                && randomCA.w < CAParams2) {
              newState = 2; // new Y
            }
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Z and a neighboring X slot
          int firstindex = clamp(int(randomCA2.x * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 3 
                && randomCA2.y < CAParams3) {
              newState = 3; // new Z
            }
          }
        }

        // migration
        if( newState == 0 && nbNeighbors < 4 ) {
          // random selection of a neighbor
          int firstindex = clamp(int(randomCA2.z * 8),0,8);
          newState = 0;
          // looks for an old border around
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a != 0 ) {
              newState = int(neighborValues[ind % 8].a);
            }
          }
        }
      }
      // CAParams5: px - X/Y/Z degradation (randomCA.w)
      // randomCA2.x choice of a first neighbor
      // degradation or migration of X
      else if( randomCA.x < CAParams4 
        || nbNeighbors < 8 * CAParams5 ) { 
        newState = 0; 
      }

      // if( state ==  1 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( randomCA.w < CAParams4
      //       || nbNeighbors < 8 ) {
      //     newState = 0; //dies or migrates
      //   }
      //    out4_CA.a = float(newState);
      // }
      // // degradation or migration of Y
      // else if( state ==  2 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( randomCA2.x < CAParams5
      //       || nbNeighbors < 8 ) {
      //     newState = 0; //dies or migrates
      //   }
      //    out4_CA.a = float(newState);
      // }
      // // degradation or migration of Z
      // else if( state ==  3 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( randomCA2.y < CAParams6
      //       || nbNeighbors < 8 ) {
      //     newState = 0; //dies or migrates
      //   }
      // }


    out4_CA.a = float(newState);
    // state change: takes the color of the current stage
    if( currentCA.a != out4_CA.a ) {
      out4_CA.rgb = shadeProtocell[newState].r * currentCA.rgb;
    }
    else {
      //averageSurrounding  maxSurrounding
      maxSurrounding = clamp( maxSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeProtocell[newState] * maxSurrounding;
      // out4_CA.rgb = vec3(shadeProtocell[newState]);
    }
  }
}
  //////////////////////////////////////////////////////
  // SOCIAL COLLABORATION: PRISONNER'S DILEMNA
  else if( CAType == CA_SOCIAL_PD ) {
    // NW  N  NE
    // S   *  E
    // SW  S  SE
    //
    // 3  2  1
    // 4  *  0
    // 5  6  7
    //
    // in the Moore neighborhood, each neighbor has 3 neighbors
    // N has NE NW & Center
    // NE has N E & Center...
    // the payoff for each neighbor is calculated for these 3 neighbors
    // 3 payoffs: 
    //     . CAParams1 Cooperation
    //     . CAParams2 Defect (with Coop)
    //     . CAParams3 Coop (with Defect)
    //     . CAParams4 double defect
    // 2 states 1: coop - 2: defect
    // first state is state of player, second state is state of partner

    // const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
    //                                  {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    uint nbStates = 3;
    uint newState = 0;

    int nbNeighbors =
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0) +
      (neighborValues[4].a > 0? 1:0) +
      (neighborValues[5].a > 0? 1:0) +
      (neighborValues[6].a > 0? 1:0) +
      (neighborValues[7].a > 0? 1:0);

    vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadeSocial[int(max(0,neighborValues[0].a))]) +
      (neighborValues[1].rgb * inverseShadeSocial[int(max(0,neighborValues[1].a))]) +
      (neighborValues[2].rgb * inverseShadeSocial[int(max(0,neighborValues[2].a))]) +
      (neighborValues[3].rgb * inverseShadeSocial[int(max(0,neighborValues[3].a))]) +
      (neighborValues[4].rgb * inverseShadeSocial[int(max(0,neighborValues[4].a))]) +
      (neighborValues[5].rgb * inverseShadeSocial[int(max(0,neighborValues[5].a))]) +
      (neighborValues[6].rgb * inverseShadeSocial[int(max(0,neighborValues[6].a))]) +
      (neighborValues[7].rgb * inverseShadeSocial[int(max(0,neighborValues[7].a))]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeSocial[int(max(0,currentCA.a))]) +
      (neighborValues[0].rgb * inverseShadeSocial[int(max(0,neighborValues[0].a))]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeSocial[int(max(0,neighborValues[ind].a))];
      if( graylevel(newColorFactor) > graylevel(maxColorFactors) ) {
        maxColorFactors = newColorFactor;
      }
    }

    vec3 averageSurrounding;
    if( nbNeighbors > 0 ) {
      if( CAcolorSpread ) {
        averageSurrounding = maxColorFactors;
      }
      else {
        averageSurrounding = gatherColorFactors / nbNeighbors;
      }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }

    if( currentCA.a < 0 ) {
      out4_CA.a = randomCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      // newState = 3;
      out4_CA.rgb = shadeSocial[newState].r * currentCA.rgb;
    }
    else {
      // float payoffs[8];
      float state = clamp(currentCA.a,0,nbStates);
      int indMax = -1;
      float maxPayoff = -1;
      float sumPayoff = 0;
      int firstIndex = int(randomCA.y * 9)%8;
      for( int i = firstIndex ; i < firstIndex + 8 ; i++ ) {
        int curInd = i%8;
        float curPayoff = payoffCalc(neighborValues[curInd].a,state)
                    + payoffCalc(neighborValues[curInd].a,neighborValues[(i+1)%8].a)
                    + payoffCalc(neighborValues[curInd].a,neighborValues[(i+7)%8].a);
        sumPayoff += curPayoff;
        if( curPayoff > maxPayoff ) {
          maxPayoff = curPayoff;
          indMax = curInd;
        }
      }
      newState = int(state);

      // BASIC : adopts behavior of most successful neighbor
      if( CASubType == 1 ) {
        newState = int(neighborValues[indMax].a);
      }
      // VARIANT 2 : adopts behavior of most successful neighbor if it is above a certain minimal value
      // otherwise dies or adopts opposite behavior with a certain probability
      // ****** payoffCalc uses parameters 1 to 4
      else if( CASubType == 2 ) {
        if( sumPayoff > 24 * CAParams5 ) { // ****** payoffCalc uses parameters 1 to 4
          newState = int(neighborValues[indMax].a);
        }
        else {
          if( randomCA.z > CAParams6 ) { // ****** payoffCalc uses parameters 1 to 4
            newState = 3 - int(neighborValues[indMax].a);
          }
          else {
            newState = 0;
          }
        }
      }
      // VARIANT 3 : adopts behavior of most successful neighbor if it is above a certain minimal value
      // otherwise if alive dies or stays as it is with a certain probability
      // if dead copies most succesful neighbor with a certain probability
      else if( CASubType == 3 ) {
        if( maxPayoff > 24 * CAParams5 ) { // ****** payoffCalc uses parameters 1 to 4
          newState = int(neighborValues[indMax].a);
        }
        else {
          // alive and adopts opposite behavior
          if( neighborValues[indMax].a > 0 ) {
              if( randomCA.z > CAParams6 ) { // ****** payoffCalc uses parameters 1 to 4
                newState = 3 - int(neighborValues[indMax].a);
              }
              // or stays in its current state
          }
          //dead
          else {
              if( randomCA.z > CAParams7 ) {
                newState = int(neighborValues[indMax].a);
              }
              // or stays dead
          }
        }
      }
      // VARIANT 4 : adopts behavior of most successful neighbor if it is above a certain minimal value
      // or the opposite behavior with a given probability
      else if( CASubType == 4 ) {
        if( sumPayoff > 24 * CAParams5 ) {
          if( randomCA.z > CAParams6 ) {
             newState = int(neighborValues[indMax].a);
          }
          else {
                newState = 3 - int(neighborValues[indMax].a);
          }
        }
        else {
          if( randomCA.z > 1 - CAParams7 ) {
             newState = int(neighborValues[indMax].a);
          }
          else {
             newState = 3 - int(neighborValues[indMax].a);
          }
        }
      }

      out4_CA.a = float(newState);
      averageSurrounding = clamp( averageSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeSocial[newState] * averageSurrounding;
    }
  }

  //////////////////////////////////////////////////////
  // PATHS
  // parameter values: 0.314  0.606 0.354 0.409
  else if( CAType == CA_PATHS ) {

    for( int ind = 0 ; ind < 8 ; ind++ ) {
      neighborValuesHV[ind] = texture(fs_lookupTable2, 
                  decalCoords + neighborOffsetsPaths[ind] );
    }
    for( int ind = 0 ; ind < 16 ; ind++ ) {
      neighborValuesDiag[ind] = texture(fs_lookupTable2, 
                  decalCoords + neighborOffsetsNonPaths[ind] );
      // existsPathsDiag *= neighborValuesDiag[ind].a;
    }

    // const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
    //                                  {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
    int nbSurroundingPathsHV =
      (neighborValuesHV[0].a + neighborValuesHV[1].a > 0? 1:0) +
      (neighborValuesHV[2].a + neighborValuesHV[3].a > 0? 1:0) +
      (neighborValuesHV[4].a + neighborValuesHV[5].a > 0? 1:0) +
      (neighborValuesHV[6].a + neighborValuesHV[7].a > 0? 1:0);
    int existsPathsDiag =
      (neighborValuesDiag[0].a * neighborValuesDiag[1].a *
       neighborValuesDiag[2].a * neighborValuesDiag[3].a *
       neighborValuesDiag[4].a * neighborValuesDiag[5].a *
       neighborValuesDiag[6].a * neighborValuesDiag[7].a *
       neighborValuesDiag[8].a * neighborValuesDiag[9].a *
       neighborValuesDiag[10].a * neighborValuesDiag[11].a *
       neighborValuesDiag[12].a * neighborValuesDiag[13].a *
       neighborValuesDiag[14].a * neighborValuesDiag[15].a > 0? 1:0);

    // 0 empty
    // 1 paths
    uint nbStates = 2;
    float newState = 0;

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      if( randomCA.x > 0.5 ) {
        out4_CA.a = 1; 
        newState = 1;
        out4_CA.rgb = vec3(1);
      }
      else {
        out4_CA.a = 0; 
        newState = 0;
        out4_CA.rgb = vec3(0);
      }
    }
    else {
      float state = clamp(currentCA.a,0,1);
      if(existsPathsDiag == 0) {
        // continues a path H or V (1)
        // or joins a path H or V or with a square angle (2)
        if(nbSurroundingPathsHV == 1){ //|| nbSurroundingPathsHV == 2) {
          if( state > 0 ) {
            newState = state - CAdecay; // keeps the same state to take decay into account
          }
          else {
            newState = 1; 
          }
        }
        // 3 -> would make a T
        // 4 -> would make a X
        else {
          newState = state - CAdecay;
        }
      }
      else {
        newState = state - CAdecay; // is empty, otherwise would create a diagonal line
      }
      // newState = state;
      out4_CA.a = newState;
      if( newState > 0 ) {
        out4_CA.rgb = vec3(newState);
      }
      else {
        out4_CA.rgb = vec3(0);
      }
    }
  }

  //////////////////////////////////////////////////////
  // NO CA
  else {
    out4_CA = clamp(vec4(currentCA.rgb-vec3(CAdecay),currentCA.a),0,1); // ready
  }

}
