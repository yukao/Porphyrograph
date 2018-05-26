/*****************************************************************************
File: image-generator.cpp

Copyright (C) 2014 Lola and Yukao Meet
 
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#define GLM_FORCE_RADIANS

//#define WIN32
#ifdef WIN32
#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
using namespace std ;
#include <fstream>
#else
#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
using namespace std ;
#include <fstream>
#endif

// texture
cv::Mat image[2];

// texture loading
bool loadtexture( string fileName , int index ) {
  // texture load through OpenCV
  image[index] = cv::imread( fileName, CV_LOAD_IMAGE_COLOR);   // Read the file
  if(! image[index].data ) {                              // Check for invalid input
    printf(  "Could not open or find the image\n" );
    return false;
  }
  printf(  "Loaded %s\n" , fileName.c_str() );
  return true;
}

// texture saving
bool writetexture( string fileName , int index ) {
  std::vector<int> params;
  params.push_back(CV_IMWRITE_PNG_COMPRESSION);
  params.push_back(9);
  params.push_back(0);
  cv::imwrite( fileName , image[index] );

  return 0;
}

////////////////////////////////////////
// main file
int main () {
  // texture loading
  if( !loadtexture( "ParticleAcceleration_alK_3D_007_init.png" , 0 ) ) {
    return -1;
  }

  
  for( int j = 0 ; j < image[0].rows ; j++ ) {
    for( int i = 0 ; i < image[0].cols ; i++ ) {  
      cv::Vec3b& bgraColor = image[0].at<cv::Vec3b>(j,i);
      bgraColor.val[0] = rand() % 256;
      bgraColor.val[1] = rand() % 256;
      bgraColor.val[2] = rand() % 256;
    }
  }

  // texture loading
  if( !writetexture( "ParticleAcceleration_alK_3D_007.png" , 0 ) ) {
    return -1;
  }
}
