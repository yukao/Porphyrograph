/*! \file vc-subdivisions.cpp
 *  \brief the implementation of \a vc-subdivisions.h
 *
 *
 *     This source file is part of Virtual Choreographer
 *     (3D interactive multimedia scene rendering)
 *     For the latest info, see http://virchor.sourceforge.net/
 *
 *     Copyright � 2002-2006 CNRS-LIMSI and Universit� Paris 11
 *     Also see acknowledgements in Readme.html
 *
 *     File vc-subdivisions.cpp
 *
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "vc-all_include.h"

const char *SubdivisionAlgorithmString[EmptySubdivisionAlgorithm + 1] = { "Loop" , "Modified-Butterfly" , "emptysubdivisionalgorithm" };

const char *RenderingPrimitiveString[EmptyRenderingPrimitive + 1] = { "triangle" , "line" , "point" , "point-sprite" };

vc_Edge::vc_Edge( void ) {
  init();
}
void vc_Edge::init( void ) {
  indVertex1 = 1;
  indVertex2 = 1;
}

vc_Edge::~vc_Edge(void) {
}

void vc_Edge::print( FILE *file , int depth , int index ) {
  indent( file , depth );
  if( index >= 0 ) {
    fprintf( file , "<edge index=%d>\n" , index + 1 );
  }
  else {
    fprintf( file , "<edge>\n" );
  }
  indent( file , depth + 1 );
  fprintf( file , "%d %d\n" , indVertex1, indVertex2 );
  indent( file , depth );
  fprintf( file , "</edge>\n" );
}
void vc_Edge::print( FILE *file , int depth ) {
  print( file , depth , 0 );
}
void vc_Edge::operator=(vc_Edge& e) {
  indVertex1 = e.indVertex1;
  indVertex2 = e.indVertex2;
  indFace1 = e.indFace1;
  indFace2 = e.indFace2;
}

vc_Face::vc_Face( void ) {
  init();
}
vc_Face::~vc_Face(void) {
}

void vc_Face::init( void ) {
  indVertex1 = -1;
  indVertex2 = -1;
  indVertex3 = -1;
  indNormal1 = -1;
  indNormal2 = -1;
  indNormal3 = -1;
  indTangent1 = -1;
  indTangent2 = -1;
  indTangent3 = -1;
//   indColor1 = -1;
//   indColor2 = -1;
//   indColor3 = -1;
  indTextureCoord1 = -1;
  indTextureCoord2 = -1;
  indTextureCoord3 = -1;
}
void vc_Face::normal( vc_vector3 *normal ,
		      vc_Vertex *tabVertices ) {
  normal->prodVect(
		  tabVertices[indVertex1] ,
		  tabVertices[indVertex2] ,
		  tabVertices[indVertex3] );
}

void vc_Face::tangent( vc_vector3 *tangent ,
		       vc_vector3 *normal,
		       int indCurrentVertex ,
		       vc_Vertex *tabVertices ,
		       vc_TextureCoord *tabTextureCoords ) {
  int indV1 = indVertex1;
  int indV2 = indVertex2;
  int indTC1 = indTextureCoord1;
  int indTC2 = indTextureCoord2;

  // indV1 is the index of the current vertex (indCurrentVertex)
  // indV2 is the index of the next vertex in the triangle
  // (after indV1)
  // similar computation for texture coordinates
  // indV2 can only be found if indCurrentVertex is passed
  // to the the function (-1 is passed in the case of a unique tangent.
  // in this case we use indVertex1 and indVertex2 as indices)
  if( indCurrentVertex >= 0 ) {
    if( indCurrentVertex == indVertex1 ) {
      indV1 = indVertex1;
      indV2 = indVertex2;
      indTC1 = indTextureCoord1;
      indTC2 = indTextureCoord2;
    }
    else if( indCurrentVertex == indVertex2 ) {
      indV1 = indVertex2;
      indV2 = indVertex3;
      indTC1 = indTextureCoord2;
      indTC2 = indTextureCoord3;
    }
    else if( indCurrentVertex == indVertex3 ) {
      indV1 = indVertex3;
      indV2 = indVertex1;
      indTC1 = indTextureCoord3;
      indTC2 = indTextureCoord1;
    }
    else {
      sprintf( ErrorStr , "Error: incorrect tangent index %d!" , indCurrentVertex ); ReportError( ErrorStr ); throw 203;
    }
  }
    
  // computes two tangent vectors 
  // let M be the current point
  // and N be the next point in the current face

  // t1 is the projection of MN on the tangent plane
  // (the plane perpendicular to normal)
  // and 2 it the vector perpendicular to t1 and to normal
  vc_vector3 t1, t2;
  t2.prodVect( tabVertices[indV1] ,
	       tabVertices[indV2] , 
	       *normal );
  t2 *= -1;
  t2.normalize();
  t1.prodVect( t1 , *normal );
  t1.normalize();

  // we now compute the angle alpha between t1 and the first vector
  // of the texture coordinate system in the tangent plance
  // we first compute the MN vector in the texture coordinate system
  float uprimeU = tabTextureCoords[indTC2].u
    - tabTextureCoords[indTC1].u;
  float vprimeV = tabTextureCoords[indTC2].v
    - tabTextureCoords[indTC1].v;
  // alpha is the ratio between y and x coordinates in the 
  // texture coordinate system
  float alpha;
  if( uprimeU != 0 ) {
    alpha = atan( vprimeV / uprimeU );
    if( uprimeU < 0 ) {
      alpha += (float)PI;
    }
  }
  else {
    alpha = (float)PI / 2.0F;
    if( vprimeV < 0 ) {
      alpha += (float)PI;
    }
  }
  //rotates the tangent vectors so that they align with
  // the texture coordinates
  t1 *= cos( alpha );
  *tangent = t1;
  t2 *= sin( alpha );
  *tangent += t2;
  tangent->normalize();
}

void vc_Face::print( FILE *file , int depth , int index ) {
  indent( file , depth );
  if( index >= 0 ) {
    fprintf( file , "<face index=\"%d\">\n" , index + 1 );
  }
  else {
    fprintf( file , "<face>\n" );
  }
  indent( file , depth + 1 );
  fprintf( file , "%d %d %d\n" , indVertex1 + 1 , indVertex2 + 1 , indVertex3  + 1 );
  indent( file , depth );
  fprintf( file , "</face>\n" );

  if( indTextureCoord1 >= 0 &&  indTextureCoord2 >= 0 && indTextureCoord3 >= 0 ) {
    indent( file , depth );
    if( index >= 0 ) {
      fprintf( file , "<facetexture index=\"%d\">\n" , index + 1 );
    }
    else {
      fprintf( file , "<facetexture>\n" );
    }
    indent( file , depth + 1 );
    fprintf( file , "%d %d %d\n" , indTextureCoord1 + 1 , indTextureCoord2 + 1 , indTextureCoord3 + 1  );
    indent( file , depth );
    fprintf( file , "</facetexture>\n" );
  }

  if( indNormal1 >= 0 &&  indNormal2 >= 0 && indNormal3 >= 0 ) {
    indent( file , depth );
    if( index >= 0 ) {
      fprintf( file , "<facenormal index=\"%d\">\n" , index + 1 );
    }
    else {
      fprintf( file , "<facenormal>\n" );
    }
    indent( file , depth + 1 );
    fprintf( file , "%d %d %d\n" , indNormal1 + 1 , indNormal2 + 1 , indNormal3 + 1  );
    indent( file , depth );
    fprintf( file , "</facenormal>\n" );
  }

  if( indTangent1 >= 0 &&  indTangent2 >= 0 && indTangent3 >= 0 ) {
    indent( file , depth );
    if( index >= 0 ) {
      fprintf( file , "<facetangent index=\"%d\">\n" , index + 1 );
    }
    else {
      fprintf( file , "<facetangent>\n" );
    }
    indent( file , depth + 1 );
    fprintf( file , "%d %d %d\n" , indTangent1 + 1 , indTangent2 + 1 , indTangent3 + 1  );
    indent( file , depth );
    fprintf( file , "</facetangent>\n" );
  }

//   if( indColor1 >= 0 &&  indColor2 >= 0 && indColor3 >= 0 ) {
//     indent( file , depth );
//     if( index >= 0 ) {
//       fprintf( file , "<facecolor index=\"%d\">\n" , index + 1 );
//     }
//     else {
//       fprintf( file , "<facecolor>\n" );
//     }
//     indent( file , depth + 1 );
//     fprintf( file , "%d %d %d\n" , indColor1 + 1 , indColor2 + 1 , indColor3 + 1  );
//     indent( file , depth );
//     fprintf( file , "</facecolor>\n" );
//   }
}
void vc_Face::print( FILE *file , int depth ) {
  print( file , depth , 0 );
}
void vc_Face::operator=(vc_Face& f) {
  indVertex1 = f.indVertex1;
  indVertex2 = f.indVertex2;
  indVertex3 = f.indVertex3;
  indTextureCoord1 = f.indTextureCoord1;
  indTextureCoord2 = f.indTextureCoord2;
  indTextureCoord3 = f.indTextureCoord3;
  indNormal1 = f.indNormal1;
  indNormal2 = f.indNormal2;
  indNormal3 = f.indNormal3;
  indTangent1 = f.indTangent1;
  indTangent2 = f.indTangent2;
  indTangent3 = f.indTangent3;
//   indColor1 = f.indColor1;
//   indColor2 = f.indColor2;
//   indColor3 = f.indColor3;
}

vc_Subdivision::vc_Subdivision( void ):vc_GeometricalObject() {
  pVerticesBuffer = NULL;
  pTexCoordsBuffer = NULL;
  pNormalsBuffer = NULL;
  pTangentsBuffer = NULL;
  bufferFaceNumber = 0;

  //   maxArity = 0;
  averageNormalFace = NULL;
  averageTangentFace = NULL;

  nbLevels = 0;
  shadowLevel = 0;
  nbMaxVertices = 0;
  max_degree = 30;
  nbMaxTextureCoords = 0;
  nbMaxNormals = 0;
  nbMaxTangents = 0;
//   nbMaxColors = 0;
  nbMaxFaces = 0;
  tabVertices = NULL;
#ifdef VC_CRYSTAL_CAHEN
  tabDistances = NULL;
#endif
  tabTextureCoords = NULL;
  tabNormals = NULL;
  tabTangents = NULL;

//   tabColors = NULL;
  tabFaces = NULL;
  tabEdges = NULL;
  indVerticesIni = NULL;
  indVerticesEnd = NULL;
  indTextureCoordsIni = NULL;
  indTextureCoordsEnd = NULL;
  indNormalsIni = NULL;
  indNormalsEnd = NULL;
  indTangentsIni = NULL;
  indTangentsEnd = NULL;
//   indColorsIni = NULL;
//   indColorsEnd = NULL;
  indFacesIni = NULL;
  indFacesEnd = NULL;
  indEdgesIni = NULL;
  indEdgesEnd = NULL;

  // unique normal for each face
  flatShading = false;

  // reversed orientation for automatically computer normals
  reversedNormals = false;

  subdivisionAlgorithm = EmptySubdivisionAlgorithm;
  subdivisionRenderingPrimitive = VC_TRIANGLE;

  coefLoop1=.375;
  coefLoop2=.125;
  coefMB1=.5;
  coefMB2=.125;
  coefMB3=-(1.0/16.0);
  autoscale = false;
  autocenter = false;
  gpu_KPanim = false;
  scale = 0.0;
  tx = 0.0;
  ty = 0.0;
  tz = 0.0;
  keypointSize = 0;
  keypointSize_1 = 0;
  keypointSize_2 = 0;
  nbKeypointWeightedVertices = 0;
  weight_normalization = false;
  normal_normalization = true;
  tangent_normalization = true;
  keypointTransfs = NULL;
  keypointPrecedingTransfs = NULL;
  keypoint_mask_channel1 = NULL;
  keypoint_mask_channel2 = NULL; 
  keypoint_mask_channel1 = NULL;
  keypoint_mask_channel1_ID = NULL; 
  keypoint_mask_channel2_ID = NULL; 
  indirectionTableKeypointIndices_1 = NULL;
  indirectionTableKeypointIndices_2 = NULL;

  boneSize = 0;
  nbBoneWeightedVertices = 0;
  boneNodes = NULL;
  bonePrecedingMatrices = NULL;
  boneIndVertex = NULL;

  //Lissage
  struct timeval time;
  gettimeofday(&time, 0);

#ifdef VC_LUMISSON
  for (int i = 0; i < 5; i++)
    {
      cosAngleLight1Old[i] = 0;
      cosAngleLight2Old[i] = 0;
      cosAngleOfViewOld[i] = 0;
      distanceOld[i] = 0;
      it_value[i] = 0;
    }
  sous_echantillonage = 0;
  first = time.tv_sec;
  launch = 0;
  list_buffer = new std::list<vc_info *>();
#endif
}

vc_Subdivision::~vc_Subdivision( void ) {
  if (pVerticesBuffer) {
    delete [] pVerticesBuffer;
    pVerticesBuffer = NULL;
  }
  
  if (pTexCoordsBuffer) {
    delete [] pTexCoordsBuffer;
    pTexCoordsBuffer = NULL;
  }
  
  if (pNormalsBuffer) {
    delete [] pNormalsBuffer;
    pNormalsBuffer = NULL;
  }
  
  if (pTangentsBuffer) {
    delete [] pTangentsBuffer;
    pTangentsBuffer = NULL;
  }
  
  if (averageNormalFace) {
    delete [] averageNormalFace;
    averageNormalFace = NULL;
  }

  if (averageTangentFace) {
    delete [] averageTangentFace;
    averageTangentFace = NULL;
  }

  if (tabVertices) {
    delete [] tabVertices;
    tabVertices = NULL;
  }

#ifdef VC_CRYSTAL_CAHEN
  if (tabDistances) {
    delete [] tabDistances;
    tabDistances = NULL;
  }
#endif

  if (tabTextureCoords) {
    delete [] tabTextureCoords;
    tabTextureCoords = NULL;
  }

  if (tabNormals) {
    delete [] tabNormals;
    tabNormals = NULL;
  }

  if (tabTangents) {
    delete [] tabTangents;
    tabTangents = NULL;
  }

  if (tabFaces) {
    delete [] tabFaces;
    tabFaces = NULL;
  }

  if (tabEdges) {
    delete [] tabEdges;
    tabEdges = NULL;
  }

  if (indVerticesIni) {
    delete [] indVerticesIni;
    indVerticesIni = NULL;
  }
  
  if (indVerticesEnd) {
    delete [] indVerticesEnd;
    indVerticesEnd = NULL;
  }
  
  if (indTextureCoordsIni) {
    delete [] indTextureCoordsIni;
    indTextureCoordsIni = NULL;
  }

  if (indTextureCoordsEnd) {
    delete [] indTextureCoordsEnd;
    indTextureCoordsEnd = NULL;
  }

  if (indNormalsIni) {
    delete [] indNormalsIni;
    indNormalsIni = NULL;
  }

  if (indNormalsEnd) {
    delete [] indNormalsEnd;
    indNormalsEnd = NULL;
  }
  
  if (indTangentsIni) {
    delete [] indTangentsIni;
    indTangentsIni = NULL;
  }

  if (indTangentsEnd) {
    delete [] indTangentsEnd;
    indTangentsEnd = NULL;
  }
  
  if (indFacesIni) {
    delete [] indFacesIni;
    indFacesIni = NULL;
  }
  
  if (indFacesEnd) {
    delete [] indFacesEnd;
    indFacesEnd = NULL;
  }
  
  if (indEdgesIni) {
    delete [] indEdgesIni;
    indEdgesIni = NULL;
  }
  
  if (indEdgesEnd) {
    delete [] indEdgesEnd;
    indEdgesEnd = NULL;
  }

  if (keypointTransfs) {
    // the transformations associated with the keypointPrecedingTransfs 
    // should not be deleted because they have been registered
    // in allObjects and will be deallocated independently
//     for (int ind = 0; ind < keypointSize; ind++) {
//       if( keypointTransfs[ind] ) {
// 	delete keypointTransfs[ind];
//       }
//       keypointTransfs[ind] = NULL;
//     }
    
    delete [] keypointTransfs;
    keypointTransfs = NULL;
  }

  if (keypointPrecedingTransfs) {
    // the transformations associated with the keypointPrecedingTransfs 
    // should not be deleted because they have been registered
    // in allObjects and will be deallocated independently
    
    // should not be deallocated otherwise the corresponding
    // object will be deallocated twice
    
    //     for (int ind = 0; ind < keypointSize; ind++) {
    //       if( keypointPrecedingTransfs[ind] ) {
    // 	delete keypointPrecedingTransfs[ind];
    //       }
    //       keypointPrecedingTransfs[ind] = NULL;
    //     }
    
    delete [] keypointPrecedingTransfs;
    keypointPrecedingTransfs = NULL;
  }
  
  
  // the keypoint_mask_channel1 is pointing to a node table
  // that will be deallocated independently
  
  // should not be deallocated otherwise the corresponding
  // object will be deallocated twice
  //   if (keypoint_mask_channel1) {
  //     delete [] keypoint_mask_channel1;
  //     keypoint_mask_channel1 = NULL;
  //   }
  
  //   if (keypoint_mask_channel2) {
  //     delete [] keypoint_mask_channel2;
  //     keypoint_mask_channel2 = NULL;
  //   }
  
  if (keypoint_mask_channel1_ID) {
    delete [] keypoint_mask_channel1_ID;
    keypoint_mask_channel1_ID = NULL;
  }
  
  if (keypoint_mask_channel2_ID) {
    delete [] keypoint_mask_channel2_ID;
    keypoint_mask_channel2_ID = NULL;
  }
  
  if (indirectionTableKeypointIndices_1) {
    delete [] indirectionTableKeypointIndices_1;
    indirectionTableKeypointIndices_1 = NULL;
  }

  if (indirectionTableKeypointIndices_2) {
    delete [] indirectionTableKeypointIndices_2;
    indirectionTableKeypointIndices_2 = NULL;
  }

  if (boneNodes) {
    for (int ind = 0; ind < boneSize; ind++) {
      delete boneNodes[ind];
      boneNodes[ind] = NULL;
    }
    
    delete [] boneNodes;
    boneNodes = NULL;
  }
  
  if (bonePrecedingMatrices) {
    delete [] bonePrecedingMatrices;
    bonePrecedingMatrices = NULL;
  }
  
  if (boneIndVertex) {
    delete [] boneIndVertex;
    boneIndVertex = NULL;
  }

#ifdef VC_LUMISSON
  delete [] cosAngleLight1Old;
  cosAngleLight1Old = NULL;
  
  delete [] cosAngleLight2Old;
  cosAngleLight2Old = NULL;
  
  delete [] cosAngleOfViewOld;
  cosAngleOfViewOld = NULL;

  delete [] distanceOld;
  distanceOld = NULL;

  delete [] it_value;
  it_value = NULL;

  if (list_buffer.empty() != NULL) {
    vc_info * info;
    for (int ind = 0; ind < list_buffer.size(); ind++) {
      info = list_buffer.front();
      if (info) delete info;
      list_buffer.pop_back();
    }
  }
#endif
}

void vc_Subdivision::setParameters( vc_XMLTag *XMLtag , bool atRunTime ,
				    FloatBinOp oper ,
				    IntBinOp operI ) {
  if( !atRunTime ||
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }


  if( !atRunTime ) {
    nbLevels = 0;
  }
  if( XMLtag->hasAttribute( "levels" ) ) {
    operI( nbLevels , XMLtag->getAttributeIntValue( "levels" ,
						    atRunTime ));
  }

  if( !atRunTime ) {
    shadowLevel = 0;
  }
  if( XMLtag->hasAttribute( "shadow_level" ) ) {
    operI( shadowLevel , XMLtag->getAttributeIntValue( "shadow_level" ,
						       atRunTime ));
  }

  if( !atRunTime ) {
    nbMaxVertices = 0;
  }
  if( XMLtag->hasAttribute( "max_vertices" ) ) {
    operI( nbMaxVertices , XMLtag->getAttributeIntValue( "max_vertices" ,
							 atRunTime ));
  }

  if( !atRunTime ) {
    max_degree = 30;
  }
  if( XMLtag->hasAttribute( "max_degree" ) ) {
    operI( max_degree , XMLtag->getAttributeIntValue( "max_degree" ,
						      atRunTime ));
  }

  if( !atRunTime ) {
    nbMaxTextureCoords = 0;
  }
  if( XMLtag->hasAttribute( "max_texturecoords" ) ) {
    operI( nbMaxTextureCoords , XMLtag->getAttributeIntValue( "max_texturecoords" ,
							      atRunTime ));
  }

  if( !atRunTime ) {
    nbMaxNormals = 0;
  }
  if( XMLtag->hasAttribute( "max_normals" ) ) {
    operI( nbMaxNormals , XMLtag->getAttributeIntValue( "max_normals" ,
							atRunTime ));
  }

  // default max normals is max tangents to be compatible
  // with previous versions where number of tangents was not given
  if( !atRunTime ) {
    nbMaxTangents = nbMaxNormals;
  }
  if( XMLtag->hasAttribute( "max_tangents" ) ) {
    operI( nbMaxTangents , XMLtag->getAttributeIntValue( "max_tangents" ,
							 atRunTime ));
  }
  // printf("nbMaxTangents %d\n", nbMaxTangents);

//   if( !atRunTime ) {
//     nbMaxColors = 0;
//   }
//   if( XMLtag->hasAttribute( "max_colors" ) ) {
//     operI( nbMaxColors , XMLtag->getAttributeIntValue( "max_colors" ,
// 						       atRunTime ));
//   }

  if( !atRunTime ) {
    nbMaxFaces = 0;
  }
  if( XMLtag->hasAttribute( "max_faces" ) ) {
    operI( nbMaxFaces , XMLtag->getAttributeIntValue( "max_faces" ,
						      atRunTime ));
  }

  if( !atRunTime ) {
    flatShading = false;
  }
  if( XMLtag->hasAttribute( "flat_shading" ) ) {
    XMLtag->BooleanFieldValue( "flat_shading" ,
			       &flatShading );
  }

  if( !atRunTime ) {
    reversedNormals = false;
  }
  if( XMLtag->hasAttribute( "reversed_normals" ) ) {
    XMLtag->BooleanFieldValue( "reversed_normals" ,
			       &reversedNormals );
  }

  if( !atRunTime ) {
    keypointSize = 0;
  }
  if( XMLtag->hasAttribute( "keypoints" ) ) {
    operI( keypointSize , XMLtag->getAttributeIntValue( "keypoints" ,
							atRunTime ));
  }

  if( !atRunTime ) {
    boneSize = 0;
  }
  if( XMLtag->hasAttribute( "bones" ) ) {
    operI( boneSize , XMLtag->getAttributeIntValue( "bones" ,
						    atRunTime ));
  }

  if( !atRunTime ) {
    subdivisionAlgorithm = LOOP;
  }
  if( XMLtag->hasAttribute( "subdivision_algorithm" ) ) {
    char *algo_str = XMLtag->getAttributeStringValue( "subdivision_algorithm" );
    subdivisionAlgorithm = EmptySubdivisionAlgorithm;
    for( int ind = 0 ; ind < EmptySubdivisionAlgorithm ; ind++ ) {
      if( strcmp( algo_str , SubdivisionAlgorithmString[ind] ) == 0 ) {
	subdivisionAlgorithm = (SubdivisionAlgorithm)ind;
	break;
      }
    }
    if( subdivisionAlgorithm == EmptySubdivisionAlgorithm ) {
      sprintf( ErrorStr , "Error: unknown subdivision algorithm [%s] in component ID [%s]!" , algo_str , id ); ReportError( ErrorStr ); throw 203;
    }
  }

  if( !atRunTime ) {
    subdivisionRenderingPrimitive = VC_TRIANGLE;
  }

  if( XMLtag->hasAttribute( "subdivision_rendering" ) ) {
    char *render_str = XMLtag->getAttributeStringValue( "subdivision_rendering" );
    subdivisionRenderingPrimitive = EmptyRenderingPrimitive;
    for( int ind = 0 ; ind < EmptyRenderingPrimitive ; ind++ ) {
      if( strcmp( render_str , RenderingPrimitiveString[ind] ) == 0 ) {
	subdivisionRenderingPrimitive = (RenderingPrimitive)ind;
	break;
      }
    }
    if( subdivisionRenderingPrimitive == EmptyRenderingPrimitive ) {
      sprintf( ErrorStr , "Error: unknown subdivision algorithm [%s] in component ID [%s]!" , render_str , id ); ReportError( ErrorStr ); throw 203;
    }
  }

  if( !atRunTime ) {
    coefLoop1 = 0.375;
  }
  if( XMLtag->hasAttribute( "coefLoop1" ) ) {
    oper( coefLoop1 , (float)XMLtag->getAttributeDoubleValue( "coefLoop1" ,
							      atRunTime ));
  }

  if( !atRunTime ) {
    coefLoop2 = 0.375;
  }
  if( XMLtag->hasAttribute( "coefLoop2" ) ) {
    oper( coefLoop2 , (float)XMLtag->getAttributeDoubleValue( "coefLoop2" ,
							      atRunTime ));
  }

  if( !atRunTime ) {
    coefMB1 = .5;
  }
  if( XMLtag->hasAttribute( "coefMB1" ) ) {
    oper( coefMB1 , (float)XMLtag->getAttributeDoubleValue( "coefMB1" ,
							    atRunTime ));
  }

  if( !atRunTime ) {
    coefMB2 = .125;
  }
  if( XMLtag->hasAttribute( "coefMB2" ) ) {
    oper( coefMB2 , (float)XMLtag->getAttributeDoubleValue( "coefMB2" ,
							    atRunTime ));
  }

  if( !atRunTime ) {
    coefMB3 = -(1.0/16.0);
  }
  if( XMLtag->hasAttribute( "coefMB3" ) ) {
    oper( coefMB3 , (float)XMLtag->getAttributeDoubleValue( "coefMB3" ,
							    atRunTime ));
  }

  if( !atRunTime ) {
    autoscale = false;
  }
  if( XMLtag->hasAttribute( "autoscale" ) ) {
    XMLtag->BooleanFieldValue( "autoscale" ,
			       &autoscale );
  }

  if( !atRunTime ) {
    normal_normalization = false;
  }
  if( XMLtag->hasAttribute( "normal_normalization" ) ) {
    XMLtag->BooleanFieldValue( "normal_normalization" ,
			       &normal_normalization );
  }
  
  if( !atRunTime ) {
    tangent_normalization = false;
  }
  if( XMLtag->hasAttribute( "tangent_normalization" ) ) {
    XMLtag->BooleanFieldValue( "tangent_normalization" ,
			       &tangent_normalization );
  }

  if( !atRunTime ) {
    autocenter = false;
  }
  if( XMLtag->hasAttribute( "autocenter" ) ) {
    XMLtag->BooleanFieldValue( "autocenter" ,
			       &autocenter );
  }

  if( !atRunTime 
      && XMLtag->hasAttribute( "gpu_keypoint_animation" ) ) {
    XMLtag->BooleanFieldValue( "gpu_keypoint_animation" ,
			       &gpu_KPanim );
#ifdef VC_HAVE_VBO
    if( gpu_KPanim ) {
      sprintf( ErrorStr , "Error: gpu keypoint animation is currently not compatible with VBO compiling configuration [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
#endif
  }

  if( !atRunTime ) {
    if( tabVertices ) {
      sprintf( ErrorStr , "Error: vertex table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabVertices = new vc_Vertex[ nbMaxVertices ];
    if( !tabVertices ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nbMaxVertices ; indVertex++ ) {
      tabVertices[ indVertex ].init();
    }

#ifdef VC_CRYSTAL_CAHEN
    if( tabDistances ) {
      sprintf( ErrorStr , "Error: distance table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabDistances = new float[ nbMaxVertices ];
    if( !tabDistances ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nbMaxVertices ; indVertex++ ) {
      tabDistances[ indVertex ] = 0.0;
    }
#endif

    if( tabTextureCoords ) {
      sprintf( ErrorStr , "Error: texture coord table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabTextureCoords = new vc_TextureCoord[ nbMaxTextureCoords ];
    if( !tabTextureCoords ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indTextureCoord = 0 ; indTextureCoord < nbMaxTextureCoords ; indTextureCoord++ ) {
      tabTextureCoords[ indTextureCoord ].u = 0.0;
      tabTextureCoords[ indTextureCoord ].v = 0.0;
    }

    if( tabNormals ) {
      sprintf( ErrorStr , "Error: normal table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabNormals = new vc_vector3[ nbMaxNormals ];
    if( !tabNormals ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indNormal = 0 ; indNormal < nbMaxNormals ; indNormal++ ) {
      tabNormals[ indNormal ].x = 0.0;
      tabNormals[ indNormal ].y = 0.0;
      tabNormals[ indNormal ].z = 0.0;
    }

    if( tabTangents ) {
      sprintf( ErrorStr , "Error: tangent table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabTangents = new vc_vector3[ nbMaxTangents ];
    if( !tabTangents ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indTangent = 0 ; indTangent < nbMaxTangents ; indTangent++ ) {
      tabTangents[ indTangent ].x = 0.0;
      tabTangents[ indTangent ].y = 0.0;
      tabTangents[ indTangent ].z = 0.0;
    }

//     if( tabColors ) {
//       sprintf( ErrorStr , "Error: color table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
//     }
//     tabColors = new GLfloat[ nbMaxColors * 4 ];
//     if( !tabColors ) {
//       sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
//     }
//     for( int indColor = 0 ; indColor < nbMaxColors ; indColor++ ) {
//       GLfloat * color = &tabColors[ indColor * 4 ];
//       for( int ind = 0 ; ind < 4 ; ind++ ) {
// 	color[ ind ] = 1.0;
//       }
//     }

    if( tabFaces ) {
      sprintf( ErrorStr , "Error: face table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabFaces = new vc_Face[ nbMaxFaces ];
    if( !tabFaces ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indFace = 0 ; indFace < nbMaxFaces ; indFace++ ) {
      tabFaces[ indFace ].init();
    }

    indVerticesIni = new int[ nbLevels ];
    indVerticesEnd = new int[ nbLevels ];
    indTextureCoordsIni = new int[ nbLevels ];
    indTextureCoordsEnd = new int[ nbLevels ];
    indNormalsIni = new int[ nbLevels ];
    indNormalsEnd = new int[ nbLevels ];
    indTangentsIni = new int[ nbLevels ];
    indTangentsEnd = new int[ nbLevels ];
//     indColorsIni = new int[ nbLevels ];
//     indColorsEnd = new int[ nbLevels ];
    indFacesIni = new int[ nbLevels ];
    indFacesEnd = new int[ nbLevels ];
    indEdgesIni = new int[ nbLevels ];
    indEdgesEnd = new int[ nbLevels ];
  }
}

bool vc_Subdivision::getParameters( char *attribute ,
				    DataType * dataType ,
				    double * valDouble ,
				    int * valInt ,
				    bool * valBool ,
				    char ** valString ,
				    int * tableOrMatrixSize ,
				    vc_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "levels" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbLevels;
    return true;
  }
  else if( strcmp( attribute , "shadow_level" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = shadowLevel;
    return true;
  }
  else if( strcmp( attribute , "max_vertices" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbMaxVertices;
    return true;
  }
  else if( strcmp( attribute , "max_degree" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = max_degree;
    return true;
  }
  else if( strcmp( attribute , "max_texturecoords" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbMaxTextureCoords;
    return true;
  }
  else if( strcmp( attribute , "max_normals" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbMaxNormals;
    return true;
  }
  else if( strcmp( attribute , "max_tangents" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbMaxTangents;
    return true;
  }
//   else if( strcmp( attribute , "max_colors" ) == 0 ) {
//     *dataType = DataTypeInt;
//     *valInt = nbMaxColors;
//     return true;
//  }
  else if( strcmp( attribute , "max_faces" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbMaxFaces;
    return true;
  }
  else if( strcmp( attribute , "keypoints" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = keypointSize;
    return true;
  }
  else if( strcmp( attribute , "flat_shading" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = flatShading;
    return true;
  }
  else if( strcmp( attribute , "reversed_normals" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = reversedNormals;
    return true;
  }
  else if( strcmp( attribute , "bones" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = boneSize;
    return true;
  }
  else if( strcmp( attribute , "subdivision_algorithm" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)SubdivisionAlgorithmString[subdivisionAlgorithm];
    return true;
  }
  else if( strcmp( attribute , "subdivision_rendering" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)RenderingPrimitiveString[subdivisionRenderingPrimitive];
    return true;
  }
  else if( strcmp( attribute , "coefLoop1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)coefLoop1;
    return true;
  }
  else if( strcmp( attribute , "coefLoop2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)coefLoop2;
    return true;
  }
  else if( strcmp( attribute , "coefMB1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)coefMB1;
    return true;
  }
  else if( strcmp( attribute , "coefMB2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)coefMB2;
    return true;
  }
  else if( strcmp( attribute , "coefMB3" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)coefMB3;
    return true;
  }
  else if( strcmp( attribute , "autoscale" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = autoscale;
    return true;
  }
  else if( strcmp( attribute , "autocenter" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = autocenter;
    return true;
  }
  else if( strcmp( attribute , "normal_normalization" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = normal_normalization;
    return true;
  }
  else if( strcmp( attribute , "tangent_normalization" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = tangent_normalization;
    return true;
  }
  else if( strcmp( attribute , "gpu_keypoint_animation" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = gpu_KPanim;
    return true;
  }
  else if( *attribute == 'v' && strchr( attribute , ':' ) ) {
    int ind = atoi( attribute + 1 ) - 1;
    if( ind < indVerticesEnd[ nbLevels - 1 ] ) {
      if( tabVertices[ind].getParameters( strchr( attribute , ':' ) + 1 ,
					  dataType ,
					  valDouble , valInt , valBool , valString ,
					  tableOrMatrixSize ,
					  valScalars ) ) {
	//printf( "attribute %s ind vertex %d val %.3f\n" , attribute , ind , *valDouble );
	return true;
      }
    }
  }
  return false;
}

void vc_Subdivision::parse( int *fileDepth , int *p_c ,
			    vc_SceneNode *sceneNode ,
			    int numberOfInitialVertices ,
			    int numberOfInitialTextureCoords ,
			    int numberOfInitialNormals ,
			    int numberOfInitialTangents ,
// 			    int numberOfInitialColors ,
			    int numberOfInitialFaces ) {

  vc_XMLTag  *XMLtag = new vc_XMLTag();

  vc_Vertex  *localTabVertices;
  vc_TextureCoord  *localTabTextureCoords;
  vc_vector3  *localTabNormals;
  vc_vector3  *localTabTangents;
//   GLfloat  *localTabColors;
  vc_Face    *localTabFaces;

  if( nbMaxFaces < numberOfInitialFaces ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: subdivision ID [%s] must contain at least %d (%d) faces!" , id , numberOfInitialFaces , nbMaxFaces ); ReportError( ErrorStr ); throw 50;
  }
  if( nbMaxVertices < numberOfInitialVertices ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: subdivision ID [%s] must contain at least %d (%d) vertices!" , id , numberOfInitialVertices , nbMaxVertices ); ReportError( ErrorStr ); throw 50;
  }
  if( nbMaxTextureCoords < numberOfInitialTextureCoords ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: subdivision ID [%s] must contain at least %d (%d) texture coordinates!" , id , numberOfInitialTextureCoords , nbMaxTextureCoords ); ReportError( ErrorStr ); throw 50;
  }
  if( nbMaxNormals < numberOfInitialNormals ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: subdivision ID [%s] must contain at least %d (%d) normals!" , id , numberOfInitialNormals , nbMaxNormals ); ReportError( ErrorStr ); throw 50;
  }
  if( nbMaxTangents < numberOfInitialTangents ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: subdivision ID [%s] must contain at least %d (%d) tangents!" , id , numberOfInitialTangents , nbMaxTangents ); ReportError( ErrorStr ); throw 50;
  }
//   if( nbMaxColors < numberOfInitialColors ) {
//     delete XMLtag;
//     sprintf( ErrorStr , "Error: subdivision ID [%s] must contain at least %d (%d) colors!" , id , numberOfInitialColors , nbMaxColors ); ReportError( ErrorStr ); throw 50;
//   }

  localTabVertices = new vc_Vertex[ numberOfInitialVertices ];
  localTabTextureCoords = new vc_TextureCoord[ numberOfInitialTextureCoords ];
  localTabNormals = new vc_vector3[ numberOfInitialNormals ];
  localTabTangents = new vc_vector3[ numberOfInitialTangents ];
//   localTabColors = new GLfloat[ numberOfInitialColors * 4 ];
  localTabFaces = new vc_Face[ numberOfInitialFaces ];
//   if( !localTabVertices || !localTabTextureCoords || !localTabNormals || !localTabColors || !localTabFaces ) {
  if( !localTabVertices || !localTabTextureCoords || !localTabNormals || !localTabTangents || !localTabFaces ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }

  for( int ind = 0 ; ind < numberOfInitialVertices ; ind++ ) {
    localTabVertices[ind].init();
  }
  for( int ind = 0 ; ind < numberOfInitialTextureCoords ; ind++ ) {
    localTabTextureCoords[ind].u = 0;
    localTabTextureCoords[ind].v = 0;
  }
  for( int ind = 0 ; ind < numberOfInitialNormals ; ind++ ) {
    localTabNormals[ind].x = 0;
    localTabNormals[ind].y = 0;
    localTabNormals[ind].z = 0;
  }
  for( int ind = 0 ; ind < numberOfInitialTangents ; ind++ ) {
    localTabTangents[ind].x = 0;
    localTabTangents[ind].y = 0;
    localTabTangents[ind].z = 0;
  }
//   for( int indColor = 0 ; indColor < numberOfInitialColors ; indColor++ ) {
//     GLfloat * color = &localTabColors[ indColor * 4 ];
//     for( int ind = 0 ; ind < 4 ; ind++ ) {
//       color[ ind ] = 1.0;
//     }
//   }
  for( int ind = 0 ; ind < numberOfInitialFaces ; ind++ ) {
    localTabFaces[ind].init();
  }

  // printf("subdivision begin\n");

  while( true ) {
    // new tag XML : subdivision component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    // subdivision closing tag
    if( strcmp( XMLtag->tag , "subdivision" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      // printf("subdivision updateBasePolygon\n");
      updateBasePolygon( localTabVertices ,
			 localTabTextureCoords ,
			 localTabNormals ,
			 localTabTangents ,
// 			 localTabColors ,
			 localTabFaces ,
			 numberOfInitialVertices ,
			 numberOfInitialTextureCoords ,
			 numberOfInitialNormals ,
			 numberOfInitialTangents ,
// 			 numberOfInitialColors ,
			 numberOfInitialFaces );
      // printf("subdivision end\n");
      delete [] localTabVertices;
      localTabVertices = NULL;

      delete [] localTabTextureCoords;
      localTabTextureCoords = NULL;
      
      delete [] localTabNormals;
      localTabNormals = NULL;
      
      delete [] localTabTangents;
      localTabTangents = NULL;
      
//       delete [] localTabColors;
      delete [] localTabFaces;
      localTabFaces = NULL;
      
      delete XMLtag;
      XMLtag = NULL;

      return;
    }

    // subdivision FDPs
    else if( strcmp( XMLtag->tag , "fdp" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      int nbKeypoints = 0;

      nbKeypoints = XMLtag->getAttributeIntValue( "size" , false );
      if( nbKeypoints != keypointSize ) {
	sprintf( ErrorStr , "Error: keypoint size in fdp element does not correspond to the keypoint attribute value (%d) in subdivision [%s]!" , keypointSize , id ); ReportError( ErrorStr ); throw 220;
      }

      keypointTransfs = new vc_SceneNode* [ keypointSize ];
      keypointPrecedingTransfs = new vc_Transformation* [ keypointSize ];
      keypointLoc = new vc_point3[ keypointSize ];
      for( int ind = 0 ; ind < keypointSize ; ind++ ) {
	// printf( "keypointPrecedingTransfs %d\n" ,ind);
	keypointTransfs[ ind ] = NULL;
	keypointPrecedingTransfs[ ind ] = new vc_Transformation();
	keypointLoc[ ind ].update(0,0,0);
      }

      // parsing the coordinates of the initial vertices
      ParseFDP( fileDepth , p_c , localTabVertices , numberOfInitialVertices ,
		keypointTransfs , keypointLoc , keypointSize );
    }

    // subdivision FDP weights
    else if( strcmp( XMLtag->tag , "tabvertexfdp" ) == 0 
	     && (XMLtag->tagType == OpeningTag) ) {
      keypoint_mask_channel1 = NULL;
      keypoint_mask_channel2 = NULL; 
      keypoint_mask_channel1_ID = NULL;
      keypoint_mask_channel2_ID = NULL; 
      indirectionTableKeypointIndices_1 = NULL;
      indirectionTableKeypointIndices_2 = NULL;

      if( XMLtag->hasAttribute( "size" ) ) {
	nbKeypointWeightedVertices = XMLtag->getAttributeIntValue( "size" , false );
      }
      else {
	sprintf( ErrorStr , "Error: missing number of weighted vertices in subdivision [%s]!" , id ); ReportError( ErrorStr ); throw 220;
      }
      if( keypointSize <= 0 ) {
	sprintf( ErrorStr , "Error: <fdp> keypoint element should be placed before <tabvertexfdp> in subdivision [%s]!" , id ); ReportError( ErrorStr ); throw 220;
      }

      if( XMLtag->hasAttribute( "keypoint_mask_channel1" ) ) {
	StrCpy( &keypoint_mask_channel1_ID , 
		XMLtag->getAttributeStringValue( "keypoint_mask_channel1" ) );
	keypoint_mask_channel1 
	  = sceneNode->getTableFromName( keypoint_mask_channel1_ID );
      }
      if( XMLtag->hasAttribute( "keypoint_mask_channel2" ) ) {
	StrCpy( &keypoint_mask_channel2_ID , 
		XMLtag->getAttributeStringValue( "keypoint_mask_channel2" ) );
	keypoint_mask_channel2 
	  = sceneNode->getTableFromName( keypoint_mask_channel2_ID );
      }

      if( XMLtag->hasAttribute( "weight_normalization" ) ) {
	XMLtag->BooleanFieldValue( "weight_normalization" ,
				   &weight_normalization );
      }

      if( XMLtag->hasAttribute( "normal_normalization" ) ) {
	XMLtag->BooleanFieldValue( "normal_normalization" ,
				   &normal_normalization );
      }

      if( XMLtag->hasAttribute( "tangent_normalization" ) ) {
	XMLtag->BooleanFieldValue( "tangent_normalization" ,
				   &tangent_normalization );
      }

      // parsing the coordinates of the initial vertices
      ParseFDPWeights( fileDepth , p_c , localTabVertices ,
		       numberOfInitialVertices , 
		       keypointTransfs ,
		       keypoint_mask_channel1 ,
		       keypoint_mask_channel2 ,
		       weight_normalization ,
		       &indirectionTableKeypointIndices_1 ,
		       &indirectionTableKeypointIndices_2 ,
		       &nbKeypointWeightedVertices ,
		       keypointSize , 
		       &keypointSize_1 , 
		       &keypointSize_2 );
      // case of null weights
      if( nbKeypointWeightedVertices <= 0 ) {
	keypointSize = 0;
	keypointSize_1 = 0;
	keypointSize_2 = 0;
	if( keypointTransfs ) {
	  delete [] keypointTransfs;
	  keypointTransfs = NULL;
	}
	if( keypointLoc ) {
	  delete [] keypointLoc;
	  keypointLoc = NULL;
	}
      }
    }

    // subdivision Bones
    else if( strcmp( XMLtag->tag , "tabboneref" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      int nbBones = 0;

      nbBones = XMLtag->getAttributeIntValue( "size" , false );
      if( nbBones != boneSize ) {
	sprintf( ErrorStr , "Error: bone size in fdp element does not correspond to the bone attribute value (%d) in subdivision [%s]!" , boneSize , id ); ReportError( ErrorStr ); throw 220;
      }

      boneNodes = new vc_SceneNode* [ boneSize ];
      bonePrecedingMatrices = new vc_matrix44[ boneSize ];
      boneIndVertex = new int[ boneSize ];
      for( int ind = 0 ; ind < boneSize ; ind++ ) {
	boneNodes[ ind ] = NULL;
	bonePrecedingMatrices[ ind ].LoadIdentity();
	boneIndVertex[ ind ] = -1;
      }

      // parsing the references to the bone nodes
      ParseBones( fileDepth , p_c , numberOfInitialVertices ,
		  boneNodes , boneIndVertex , boneSize );
    }

    // subdivision FDP weights
    else if( strcmp( XMLtag->tag , "tabvertexbone" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      if( XMLtag->hasAttribute( "size" ) ) {
	nbBoneWeightedVertices = XMLtag->getAttributeIntValue( "size" , false );
      }
      else {
	sprintf( ErrorStr , "Error: missing number of weighted vertices in subdivision [%s]!" , id ); ReportError( ErrorStr ); throw 220;
      }
      if( boneSize <= 0 ) {
	sprintf( ErrorStr , "Error: <fdp> bone element should be placed before <tabvertexfdp> in subdivision [%s]!" , id ); ReportError( ErrorStr ); throw 220;
      }

      // parsing the coordinates of the initial vertices
      ParseBoneWeights( fileDepth , p_c , localTabVertices ,
			numberOfInitialVertices , &nbBoneWeightedVertices ,
			boneSize );
      // case of null weights
      if( nbBoneWeightedVertices <= 0 ) {
	boneSize = 0;
	if( boneNodes ) {
	  delete [] boneNodes;
	  boneNodes = NULL;
	}
	if( boneIndVertex ) {
	  delete [] boneIndVertex;
	  boneIndVertex = NULL;
	}
      }
    }

    // subdivision initial vertices
    else if( strcmp( XMLtag->tag , "tabvertex" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabvertex\n");

      // parsing the coordinates of the initial vertices
      ParseTabVertex( fileDepth , p_c , localTabVertices ,
		      numberOfInitialVertices );
    }

    // subdivision initial texture coordinates
    else if( strcmp( XMLtag->tag , "tabtextureCoord" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabtextureCoord\n");

      // parsing the coordinates of the initial texture coordinates
      ParseTabTextureCoord( fileDepth , p_c , localTabTextureCoords ,
			    numberOfInitialTextureCoords );
    }

    // subdivision initial normals
    else if( strcmp( XMLtag->tag , "tabnormal" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabnormal\n");

      // parsing the coordinates of the initial texture coordinates
      ParseTabNormal( fileDepth , p_c , localTabNormals ,
		      numberOfInitialNormals , normal_normalization );
    }

    // subdivision initial tangents
    else if( strcmp( XMLtag->tag , "tabtangent" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // parsing the coordinates of the initial texture coordinates
      ParseTabTangent( fileDepth , p_c , localTabTangents ,
		       numberOfInitialTangents , tangent_normalization );
    }

    // DEPRECATED: subdivision initial colors
    else if( strcmp( XMLtag->tag , "tabcolor" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      fprintf( stderr , "Deprecated: <tabcolor> tag for meshes deprecated, use material properties or textures instead!\n" );
      // parsing the coordinates of the initial texture coordinates
      // ParseTabColor( fileDepth , p_c , localTabColors ,
      // 		     numberOfInitialColors );
      ParseTabColor( fileDepth , p_c , NULL , 0 );
    }

    // subdivision initial faces
    else if( strcmp( XMLtag->tag , "tabface" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabface\n");

      // parsing the indices of the vertices in the faces
      ParseTabFace( fileDepth , p_c , localTabFaces ,
		    numberOfInitialFaces ,
		    numberOfInitialVertices ,
		    numberOfInitialTextureCoords ,
		    numberOfInitialNormals ,
		    numberOfInitialTangents 
// 		    , numberOfInitialColors 
		    );
    }

    // color
    else if( ( strcmp( XMLtag->tag , "color" ) == 0
	       || strcmp( XMLtag->tag , "material" ) == 0
	       || strcmp( XMLtag->tag , "materialRaytrace" ) == 0
	       || strcmp( XMLtag->tag , "shader" ) == 0
	       || strcmp( XMLtag->tag , "texture" ) == 0
	       || strcmp( XMLtag->tag , "video" ) == 0  )
	     && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag)) ) {
      material->parse( fileDepth , p_c , id , XMLtag , this );
    }

    else {
      sprintf( ErrorStr , "Error: unknown subdivision component tag [%s] in subdivision ID [%s]!" , XMLtag->tag , id ); ReportError( ErrorStr ); throw 50;
      delete XMLtag;
      XMLtag = NULL;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void vc_Subdivision::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
//   fprintf( file , "<subdivision id=\"%s\" levels=\"%d\" shadow_level=\"%d\" level0_vertices=\"%d\" level0_texturecoords=\"%d\" level0_normals=\"%d\" level0_colors=\"%d\" level0_faces=\"%d\" max_vertices=\"%d\" max_degree=\"%d\" max_texturecoords=\"%d\" max_normals=\"%d\" max_colors=\"%d\" max_faces=\"%d\" subdivision_algorithm=\"%s\" coefLoop1=\"%.5f\" coefLoop2=\"%.5f\" coefMB1=\"%.5f\" coefMB2=\"%.5f\" coefMB3=\"%.5f\" autoscale=\"" , id , nbLevels , shadowLevel , indVerticesEnd[ 0 ] , indTextureCoordsEnd[ 0 ] , indNormalsEnd[ 0 ] , indColorsEnd[ 0 ] , indFacesEnd[ 0 ] , nbMaxVertices , max_degree , nbMaxTextureCoords , nbMaxNormals , nbMaxColors , nbMaxFaces , SubdivisionAlgorithmString[subdivisionAlgorithm] , coefLoop1 , coefLoop2 , coefMB1 , coefMB2 , coefMB3 );
  fprintf( file , "<subdivision id=\"%s\" levels=\"%d\" shadow_level=\"%d\" level0_vertices=\"%d\" level0_texturecoords=\"%d\" level0_normals=\"%d\" level0_tangents=\"%d\" level0_faces=\"%d\" max_vertices=\"%d\" max_degree=\"%d\" max_texturecoords=\"%d\" max_normals=\"%d\" max_tangents=\"%d\" max_faces=\"%d\" subdivision_algorithm=\"%s\" subdivision_primitive=\"%s\" coefLoop1=\"%.5f\" coefLoop2=\"%.5f\" coefMB1=\"%.5f\" coefMB2=\"%.5f\" coefMB3=\"%.5f\" autoscale=\"%s\" autocenter=\"%s\" normal_normalization=\"%s\" tangent_normalization=\"%s\" gpu_keypoint_animation=\"%s\" flat_shading=\"%s\" reversed_normals=\"%s\">\n" , id , nbLevels , shadowLevel , indVerticesEnd[ 0 ] , indTextureCoordsEnd[ 0 ] , indNormalsEnd[ 0 ] , indTangentsEnd[ 0 ] , indFacesEnd[ 0 ] , nbMaxVertices , max_degree , nbMaxTextureCoords , nbMaxNormals ,  nbMaxTangents , nbMaxFaces , SubdivisionAlgorithmString[subdivisionAlgorithm] , RenderingPrimitiveString[subdivisionRenderingPrimitive] , coefLoop1 , coefLoop2 , coefMB1 , coefMB2 , coefMB3 , BooleanString[BooleanToInt(autoscale)] , BooleanString[BooleanToInt(autocenter)] ,   BooleanString[BooleanToInt(normal_normalization)] , BooleanString[BooleanToInt(tangent_normalization)] , BooleanString[BooleanToInt(gpu_KPanim)] , BooleanString[BooleanToInt(flatShading)] , BooleanString[BooleanToInt(reversedNormals)] );

  if( isLong ) {
    material->print( file , depth + 1 );

    indent( file , depth + 1 );
    fprintf( file , "<tabvertex size=\"%d\">\n" ,  indVerticesEnd[ 0 ] );
    for( int indVertex = 0 ; indVertex < indVerticesEnd[ 0 ] ; indVertex++ ) {
      tabVertices[indVertex].print( file , depth + 2 , indVertex );
    }
    indent( file , depth + 1 );
    fprintf( file , "</tabvertex>\n" );

    indent( file , depth + 1 );
    fprintf( file , "<tabtextureCoord size=\"%d\">\n" ,  indTextureCoordsEnd[ 0 ] );
    for( int indVertex = 0 ; indVertex < indTextureCoordsEnd[ 0 ] ; indVertex++ ) {
      tabTextureCoords[indVertex].print( file , depth + 2 , indVertex );
    }
    indent( file , depth + 1 );
    fprintf( file , "</tabtextureCoord>\n" );

    indent( file , depth + 1 );
    fprintf( file , "<tabnormal size=\"%d\">\n" ,  indNormalsEnd[ 0 ] );
    for( int indNormal = 0 ; indNormal < indNormalsEnd[ 0 ] ; indNormal++ ) {
      indent( file , depth + 2 );
      fprintf( file , "<normal index=\"%d\" x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , indNormal + 1 , tabNormals[indNormal].x , tabNormals[indNormal].y , tabNormals[indNormal].z );
    }
    indent( file , depth + 1 );
    fprintf( file , "</tabnormal>\n" );

    indent( file , depth + 1 );
    fprintf( file , "<tabtangent size=\"%d\">\n" ,  indTangentsEnd[ 0 ] );
    for( int indTangent = 0 ; indTangent < indTangentsEnd[ 0 ] ; indTangent++ ) {
      indent( file , depth + 2 );
      fprintf( file , "<tangent index=\"%d\" x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , indTangent + 1 , tabTangents[indTangent].x , tabTangents[indTangent].y , tabTangents[indTangent].z );
    }
    indent( file , depth + 1 );
    fprintf( file , "</tabtangent>\n" );

//     indent( file , depth + 1 );
//     fprintf( file , "<tabcolor size=\"%d\">\n" ,  indColorsEnd[ 0 ] );
//     for( int indColor = 0 ; indColor < indColorsEnd[ 0 ] ; indColor++ ) {
//       GLfloat * color = &tabColors[ indColor * 4 ];
//       indent( file , depth + 2 );
//       fprintf( file , "<color index=\"%d\" r=\"%.5f\" g=\"%.5f\" b=\"%.5f\" a=\"%.5f\"/>\n" ,
// 	       indColor + 1 , color[0] , color[1] , color[2] , color[3] );
//     }
//     indent( file , depth + 1 );
//     fprintf( file , "</tabcolor>\n" );

    indent( file , depth + 1 );
    fprintf( file , "<tabface size=\"%d\">\n" ,  indFacesEnd[ 0 ] );
    for( int indFace = 0 ; indFace < indFacesEnd[ 0 ] ; indFace++ ) {
      tabFaces[indFace].print( file , depth + 2 , indFace );
    }
    indent( file , depth + 1 );
    fprintf( file , "</tabface>\n" );

    if( keypointSize > 0 ) {
      indent( file , depth + 1 );
      fprintf( file , "<fdp size=\"%d\">\n" ,  keypointSize );
      for( int indKeypoint = 0 ; indKeypoint < keypointSize ; indKeypoint++ ) {
	indent( file , depth + 2 );
	fprintf( file , "<keypoint index=\"%d\" x=\"%.3f\" y=\"%.3f\" z=\"%.3f\" xlink:href=\"%s\">\n" , indKeypoint + 1 , keypointLoc[ indKeypoint ].x , keypointLoc[ indKeypoint ].y , keypointLoc[ indKeypoint ].z , keypointTransfs[ indKeypoint ]->id );
      }
      indent( file , depth + 1 );
      fprintf( file , "</fdp>\n" );

      indent( file , depth + 1 );
      fprintf( file , "<tabvertexfdp size=\"%d\"" ,  nbKeypointWeightedVertices );
      if( keypoint_mask_channel1_ID ) {
	fprintf( file , " keypoint_mask_channel1=\"%s\"" , 
		 keypoint_mask_channel1_ID );
      } 
      if( keypoint_mask_channel2_ID ) {
	fprintf( file , " keypoint_mask_channel2=\"%s\"" , 
		 keypoint_mask_channel2_ID );
      } 
      fprintf( file , ">\n" );

      for( int indVertex = 0 ; indVertex < indVerticesEnd[ 0 ] ; indVertex++ ) {
	if( tabVertices[indVertex].keypointWeightingSize > 0 ) {
	  indent( file , depth + 2 );
	  fprintf( file , "<vertex index=\"%d\" size=\"%d\">" ,
		   indVertex + 1  , tabVertices[indVertex].keypointWeightingSize );
	  for( int indWeight = 0 ;
	       indWeight < tabVertices[indVertex].keypointWeightingSize ;
	       indWeight++ ) {
	    fprintf( file , "%d %.5f %.5f %.5f " ,
		     tabVertices[indVertex].
		     tabKeypointWeighting[indWeight].indKeypoint + 1  ,
		     tabVertices[indVertex].
		     tabKeypointWeighting[indWeight].weights[0] ,
		     tabVertices[indVertex].
		     tabKeypointWeighting[indWeight].weights[1] ,
		     tabVertices[indVertex].
		     tabKeypointWeighting[indWeight].weights[2] );
	  }
	  fprintf( file , "</vertex>\n" );
	}
      }
      indent( file , depth + 1 );
      fprintf( file , "</tabvertexfdp>\n" );
    }

    if( boneSize > 0 ) {
      indent( file , depth + 1 );
      fprintf( file , "<tabboneref size=\"%d\">\n" ,  boneSize );
      for( int indBone = 0 ; indBone < boneSize ; indBone++ ) {
	indent( file , depth + 2 );
	fprintf( file , "<boneref index=\"%d\" vertex=\"%d\" xlink:href=\"#%s\">\n" , indBone + 1 , boneIndVertex[ indBone ] + 1 , boneNodes[ indBone ]->id );
      }
      indent( file , depth + 1 );
      fprintf( file , "</tabboneref>\n" );

      indent( file , depth + 1 );
      fprintf( file , "<tabvertexbone size=\"%d\">\n" ,  nbBoneWeightedVertices );
      for( int indVertex = 0 ; indVertex < indVerticesEnd[ 0 ] ; indVertex++ ) {
	if( tabVertices[indVertex].boneWeightingSize > 0 ) {
	  indent( file , depth + 2 );
	  fprintf( file , "<vertex index=\"%d\" size=\"%d\">" ,
		   indVertex + 1  , tabVertices[indVertex].boneWeightingSize );
	  for( int indWeight = 0 ;
	       indWeight < tabVertices[indVertex].boneWeightingSize ;
	       indWeight++ ) {
	    fprintf( file , "%d %.5f" ,
		     tabVertices[indVertex].
		     tabBoneWeighting[indWeight].indBone + 1  ,
		     tabVertices[indVertex].
		     tabBoneWeighting[indWeight].weight );
	  }
	  fprintf( file , "</vertex>\n" );
	}
      }
      indent( file , depth + 1 );
      fprintf( file , "</tabvertexbone>\n" );
    }

    // just in case, how to print the list of top level faces and vertices
    // currently unused
    if( false ) {
      // top-level vertices
      for (int i = indVerticesIni[ nbLevels - 1 ] ;
	   i < indVerticesEnd[ nbLevels - 1 ] ; i++) {
	tabVertices[i].print( file , depth ,  i + 1 );
      }
      // top-level faces
      for (int i = indFacesIni[ nbLevels - 1 ] ;
	   i < indFacesEnd[ nbLevels - 1 ] ; i++) {
	tabFaces[i].print( file , depth ,  i + 1 );
      }
    }
  }

  indent( file , depth );
  fprintf( file , "</subdivision>\n" );
}

void vc_Subdivision::averageNormal( int indVertex , int indNormal ,
				    vc_vector3 *averageNormal ,
				    int level ,
				    int * tabIndConnectedVertices ,
				    int * tabIndConnectedVerticesCopy ,
				    int * tabConnectedFaces ,
				    int * tabConnectedFacesCopy ,
				    int * nbConnectedVertices ,
				    int * nbConnectedFaces ,
				    vc_vector3 * normals
				    ) {


  *nbConnectedVertices = 0;
  *nbConnectedFaces = 0;

  // predefined normal
  if( indNormal != -1  ) {
    averageNormal->update( tabNormals[indNormal].x ,
			   tabNormals[indNormal].y ,
			   tabNormals[indNormal].z );
    averageNormal->normalize();

    // will be used for tangent average computation
    collect_connected_vertices( indVertex ,
				tabIndConnectedVertices ,
				tabIndConnectedVerticesCopy ,
				nbConnectedVertices ,
				tabConnectedFaces ,
				tabConnectedFacesCopy ,
				nbConnectedFaces ,
				max_degree , level );
    return;
  }

  // printf( "average computation\n" );
  collect_connected_vertices( indVertex ,
			      tabIndConnectedVertices ,
			      tabIndConnectedVerticesCopy ,
			      nbConnectedVertices ,
			      tabConnectedFaces ,
			      tabConnectedFacesCopy ,
			      nbConnectedFaces ,
			      max_degree , level );

  // for all the connected faces
  for (int c = 0 ; c < *nbConnectedFaces ; c++) {
    tabFaces[tabConnectedFaces[ c ]].normal( &(normals[c]) , 
					     tabVertices );
//     printf( "  indFace %d (%d) normal (%.2f,%.2f,%.2f)\n" , c , 
// 	    tabConnectedFaces[ c ] ,
// 	    normals[c].x , normals[c].y , normals[c].z );
  }

  averageNormal->averageVectors( normals , *nbConnectedFaces );
  averageNormal->normalize();

//   printf( "  av normal (%.2f,%.2f,%.2f)\n" ,
// 	  averageNormal->x ,
// 	  averageNormal->y ,
// 	  averageNormal->z );

}


// should ALWAYS come after averageNormal 
// so that tabIndConnectedVertices and tabConnectedFaces
// are already computed
void vc_Subdivision::averageTangent( 
		     int indVertex ,
		     vc_vector3 *averageTangent ,
		     vc_vector3 *normal ,
		     int *tabIndConnectedFaces ,
		     int nbConnectedFaces ,
		     vc_vector3 *tangents ) {

//   printf( "average computation\n" );

  // for all the connected faces
  for (int c = 0 ; c < nbConnectedFaces ; c++) {
    tabFaces[tabIndConnectedFaces[ c ]].tangent( &(tangents[c]) , 
						 normal , 
						 indVertex ,
						 tabVertices , 
						 tabTextureCoords );
//     printf( "  indFace %d (%d) tangent (%.2f,%.2f,%.2f)\n" , c , 
// 	    tabIndConnectedFaces[ c ] ,
// 	    tangents[c].x , tangents[c].y , tangents[c].z );
  }

  averageTangent->averageVectors( tangents , nbConnectedFaces );
  averageTangent->normalize();

//   printf( "  av tangent (%.2f,%.2f,%.2f)\n" ,
// 	  averageTangent->x ,
// 	  averageTangent->y ,
// 	  averageTangent->z );

}

void vc_Subdivision::uniqueNormal( int indFace , 
				   int indNormal ,
				   vc_vector3 *uniqueNormal ) {
  // predefined normal
  if( indNormal != -1  ) {
    uniqueNormal->update( tabNormals[indNormal].x ,
			  tabNormals[indNormal].y ,
			  tabNormals[indNormal].z );
    uniqueNormal->normalize();
//     printf( "Precomputed normal %d (%.2f,%.2f,%.2f)\n" , indNormal ,
// 	    uniqueNormal->x , uniqueNormal->y , uniqueNormal->z );
    return;
  }

  tabFaces[indFace].normal( uniqueNormal , tabVertices );
//   printf( "Computed normal (%.2f,%.2f,%.2f)\n" , 
// 	  uniqueNormal->x , uniqueNormal->y , uniqueNormal->z );
}

void vc_Subdivision::uniqueTangent( int indFace , 
				    int indTangent ,
				    vc_vector3 *normal ,
				    vc_vector3 *uniqueTangent ) {
  // predefined tangent
  if( indTangent != -1  ) {
    uniqueTangent->update( tabTangents[indTangent].x ,
			   tabTangents[indTangent].y ,
			   tabTangents[indTangent].z );
    uniqueTangent->normalize();
//     printf( "Precomputed tangent %d (%.2f,%.2f,%.2f)\n" , indTangent ,
// 	    uniqueTangent->x , uniqueTangent->y , uniqueTangent->z );
    return;
  }

  tabFaces[indFace].tangent( uniqueTangent , 
			     normal ,
			     -1 ,
			     tabVertices , 
			     tabTextureCoords );
//   printf( "Computed tangent (%.2f,%.2f,%.2f)\n" , 
// 	  uniqueTangent->x , uniqueTangent->y , uniqueTangent->z );
}

#ifdef VC_HAVE_CG
void vc_Subdivision::setVertexShaderParams( vc_VertexShader *vertShader , 
					    int indVertex ) {
  //printf( "vertex %d %d\n" , indVertex , tabVertices[indVertex].keypointWeightingSize  );
  float values_1[ 4 ] = {-1.0,-1.0,-1.0,-1.0};
  float values_2[ 4 ] = {-1.0,-1.0,-1.0,-1.0};
  for( int ind = 0 ; 
       ind < tabVertices[indVertex].keypointWeightingSize
	 && ind < 4 ;
       ind++ ) {
    values_1[ ind ] 
      = (float)tabVertices[indVertex].tabKeypointWeighting[ind].indKeypoint_1;
    values_2[ ind ] 
      = (float)tabVertices[indVertex].tabKeypointWeighting[ind].indKeypoint_2;
  }
  // printf( "Parameter index ch1 %d %d %d %d ch2 %d %d %d %d\n" , (int)(values_1[0]+1) , (int)(values_1[1]+1) , (int)(values_1[2]+1) , (int)(values_1[3]+1) , (int)(values_2[0]+1) , (int)(values_2[1]+1) , (int)(values_2[2]+1) , (int)(values_2[3]+1) );
  // cgGLSetParameter4fv( vertShader->indicesParam_1, values_1 );
  glMultiTexCoord4fvARB(GL_TEXTURE2_ARB, values_1 ); 
  if( vertShader->indicesParam_2 ) {
    // cgGLSetParameter4fv( vertShader->indicesParam_2, values_2 );
    glMultiTexCoord4fvARB(GL_TEXTURE3_ARB, values_2  ); 
  }

  float values[ 4 ] = {0,0,0,0};
  for( int ind = 0 ; 
       ind < tabVertices[indVertex].keypointWeightingSize
	 && ind < 4 ;
       ind++ ) {
    values[ ind ] 
      = (float)tabVertices[indVertex].tabKeypointWeighting[ind].weights[0];
  }
  // cgGLSetParameter4fv( vertShader->weightsParam, values );
  glMultiTexCoord4fvARB(GL_TEXTURE1_ARB, values ); 

   // printf( "Parameter weights %.5f %.5f %.5f %.5f\n" , values[0] , values[1] , values[2] , values[3]  );

  // extra weights and indices for vertex shaders 
  // with 5to8 weight and indices parameters
  //           float4 weights_5to8  : TEXCOORD4,
  //           float4 indices_1_5to8: TEXCOORD5,
  //           float4 indices_2_5to8: TEXCOORD6,

  if( vertShader->weightsParam_5to8 
      && vertShader->indicesParam_1_5to8 ) {
    float values_1_5to8[ 4 ] = {-1.0,-1.0,-1.0,-1.0};
    float values_2_5to8[ 4 ] = {-1.0,-1.0,-1.0,-1.0};

    for( int ind = 4 ; 
	 ind < tabVertices[indVertex].keypointWeightingSize
	   && ind < 8 ;
	 ind++ ) {
      values_1_5to8[ ind - 4 ] 
	= (float)tabVertices[indVertex].tabKeypointWeighting[ind].indKeypoint_1;
      values_2_5to8[ ind - 4 ] 
	= (float)tabVertices[indVertex].tabKeypointWeighting[ind].indKeypoint_2;
    }
    //     printf( "Parameter index ch1 %d %d %d %d ch2 %d %d %d %d\n" , (int)(values_1[0]+1) , (int)(values_1[1]+1) , (int)(values_1[2]+1) , (int)(values_1[3]+1) , (int)(values_2[0]+1) , (int)(values_2[1]+1) , (int)(values_2[2]+1) , (int)(values_2[3]+1) );
    //     printf( "Parameter index ch1_5to8 %d %d %d %d ch2_5to8 %d %d %d %d\n" , (int)(values_1_5to8[0]+1) , (int)(values_1_5to8[1]+1) , (int)(values_1_5to8[2]+1) , (int)(values_1_5to8[3]+1) , (int)(values_2_5to8[0]+1) , (int)(values_2_5to8[1]+1) , (int)(values_2_5to8[2]+1) , (int)(values_2_5to8[3]+1) );
    // cgGLSetParameter4fv( vertShader->indicesParam_1_5to8, values_1_5to8 );
    glMultiTexCoord4fvARB(GL_TEXTURE5_ARB, values_1_5to8 ); 
    if( vertShader->indicesParam_2_5to8 ) {
      // cgGLSetParameter4fv( vertShader->indicesParam_2_5to8, values_2_5to8 );
      glMultiTexCoord4fvARB(GL_TEXTURE6_ARB, values_2_5to8 ); 
    }
	
    float values_5to8[ 4 ] = {0,0,0,0};
    for( int ind = 4 ; 
	 ind < tabVertices[indVertex].keypointWeightingSize
	   && ind < 8 ;
	 ind++ ) {
      values_5to8[ ind - 4 ] 
	= (float)tabVertices[indVertex].tabKeypointWeighting[ind].weights[0];
    }
    // cgGLSetParameter4fv( vertShader->weightsParam_5to8, values_5to8 );
    glMultiTexCoord4fvARB(GL_TEXTURE4_ARB, values_5to8 ); 
	
    //     printf( "Parameter weights %.5f %.5f %.5f %.5f\n" , values[0] , values[1] , values[2] , values[3]  );
    //     printf( "Parameter weights_5to8 %.5f %.5f %.5f %.5f\n\n" , values_5to8[0] , values_5to8[1] , values_5to8[2] , values_5to8[3]  );
  }
}
#endif

void vc_Subdivision::displayMesh( int * tabIndConnectedVertices ,
				  int * tabIndConnectedVerticesCopy ,
				  int * tabConnectedFaces ,
				  int * tabConnectedFacesCopy ,
				  vc_vector3 * normals ,
				  vc_vector3 * tangents , 
				  bool use_VBO ) {
  vc_vector3 normal;
#ifdef NORMAL_LUMISSON
  vc_vector3 normal_lumisson;
#endif

  vc_vector3 tangent;
#ifdef VC_HAVE_CG
  vc_Shader *shader = GetMaterialShader();
  vc_VertexShader *vertShader = NULL;
  if( shader ) {
    vertShader = shader->vertexShader[ CurrentIndWindow ];
    //printf( " vertShader %d\n" , vertShader );
  }
#endif

  // current pointers that are incrementally addressed
  float *pVerticesBufferCour = pVerticesBuffer;
  float *pTexCoordsBufferCour = pTexCoordsBuffer;
  float *pNormalsBufferCour = pNormalsBuffer;

  int nbConnectedVertices;
  int nbConnectedFaces;

  // points should only be displayed once
  if(  !use_VBO && (subdivisionRenderingPrimitive == VC_POINT_SPRITE
		    || subdivisionRenderingPrimitive == VC_POINT) ) {
    for( int indVertex = 0 ; indVertex < nbMaxVertices ; indVertex++ ) {
      tabVertices[ indVertex ].modified = false;
    }
  }

  // primitive call
  if( !use_VBO && subdivisionRenderingPrimitive == VC_TRIANGLE ) {
    glBegin(GL_TRIANGLES);
  }
  else if( !use_VBO && ( subdivisionRenderingPrimitive == VC_POINT
			 || subdivisionRenderingPrimitive == VC_POINT_SPRITE ) ) {
    if(  subdivisionRenderingPrimitive == VC_POINT_SPRITE ) {
      // printf( " Point sprite\n" );
      glEnable(GL_POINT_SPRITE);
      glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
      glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    }
    glBegin(GL_POINTS);
  }

  /////////////////////////////////////////////
  // Loops over the faces
  /////////////////////////////////////////////
  for( int indFace = indFacesIni[ nbLevels - 1 ] ; 
       indFace < indFacesIni[ nbLevels - 1 ] + bufferFaceNumber ; indFace++ ) {

    if( !use_VBO ) {
      if( subdivisionRenderingPrimitive == VC_LINE ) {
	glBegin(GL_LINE_LOOP);
      }
      // else if ( subdivisionRenderingPrimitive == VC_POINT
      // 		|| subdivisionRenderingPrimitive == VC_POINT_SPRITE ) {
      // 	glBegin(GL_POINTS);
      // }
    }

    // printf( " vc_Face %d use_VBO %d\n" , indFace , use_VBO );

    /////////////////////////////////////////////
    // vertex #1
    /////////////////////////////////////////////
    if( flatShading ) {
      uniqueNormal( indFace , 
		    tabFaces[indFace].indNormal1 ,
		    &normal );
      uniqueTangent( indFace , 
		     tabFaces[indFace].indTangent1 ,
		     &normal ,
		     &tangent );
      // printf( " vc_Face %d use_VBO %d\n" , indFace , use_VBO );
    }
    else {
      // NORMAL
      if( tabFaces[indFace].indNormal1 != -1  ) {
	normal.update( tabNormals[tabFaces[indFace].indNormal1].x ,
		       tabNormals[tabFaces[indFace].indNormal1].y ,
		       tabNormals[tabFaces[indFace].indNormal1].z );
      }
      else {
	averageNormal( tabFaces[indFace].indVertex1 , 
		       tabFaces[indFace].indNormal1 ,
		       &normal , nbLevels - 1 ,
		       tabIndConnectedVertices , 
		       tabIndConnectedVerticesCopy ,
		       tabConnectedFaces , 
		       tabConnectedFacesCopy , 
		       &nbConnectedVertices ,
		       &nbConnectedFaces ,
		       normals);
      }
      // TANGENT
      if( tabFaces[indFace].indTangent1 != -1  ) {
	tangent.update( tabTangents[tabFaces[indFace].indTangent1].x ,
			tabTangents[tabFaces[indFace].indTangent1].y ,
			tabTangents[tabFaces[indFace].indTangent1].z );
	// printf( "tangent %d %.2f %.2f %.2f\n" , indFace , tangent.x , tangent.y , tangent.z );
      }
      else {
	tabFaces[indFace].tangent( &tangent , 
				   &normal , 
				   tabFaces[indFace].indVertex1 ,
				   tabVertices , 
				   tabTextureCoords );
      }
      // 	averageTangent( tabFaces[indFace].indVertex1 , 
      // 			&tangent , &normal ,
      // 			tabConnectedFaces , 
      // 			nbConnectedFaces , 
      // 			tangents );
      //       if( normal.y != 0 || normal.z != 0 ) {
      // 	tangent.x = 0;
      // 	tangent.y = normal.z;
      // 	tangent.z = -normal.y;
      //       }
      //       else {
      // 	tangent.x = 0;
      // 	tangent.y = 0;
      // 	tangent.z = 1;
      //       }
      
	// printf( "normal %d %.2f %.2f %.2f\n" , nbConnectedFaces , normal.x , normal.y , normal.z );
	// printf( "tangent %d %.2f %.2f %.2f\n" , nbConnectedFaces , tangent.x , tangent.y , tangent.z );
    }
    if( reversedNormals ) {
      // printf( "normal inversion face %d id %s\n" , indFace , id );
      normal *= -1;
    }
#ifdef NORMAL_LUMISSON
    normal_lumisson = normal;
#endif

    //printf( " Coord %d\n" , tabFaces[indFace].indTextureCoord1 );

    if( material->hasTexture
	&& tabFaces[indFace].indTextureCoord1 >= 0 ) {
//       printf(  "Texture[%.5f,%.5f]\n" ,
// 	       tabTextureCoords[tabFaces[indFace].indTextureCoord1].u
// 	       * material->materialTexture->textureTile_s ,
// 	       tabTextureCoords[tabFaces[indFace].indTextureCoord1].v
// 	       * material->materialTexture->textureTile_t );
      if( use_VBO ) {
	*pTexCoordsBufferCour =
	  tabTextureCoords[tabFaces[indFace].indTextureCoord1].u
	  * material->materialTexture->textureTile_s;
	pTexCoordsBufferCour++;
	*pTexCoordsBufferCour =
	  tabTextureCoords[tabFaces[indFace].indTextureCoord1].v
	  * material->materialTexture->textureTile_t;
	pTexCoordsBufferCour++;
      }
      else {
	glTexCoord2f( tabTextureCoords[tabFaces[indFace].indTextureCoord1].u
		      * material->materialTexture->textureTile_s ,
		      tabTextureCoords[tabFaces[indFace].indTextureCoord1].v
		      * material->materialTexture->textureTile_t );
      }
    }

#ifdef VC_HAVE_CG
    if( gpu_KPanim && vertShader 
	&& shader->shaderType == GENERALIZED_SHADER ) {
      setVertexShaderParams( vertShader , tabFaces[indFace].indVertex1 );
    }

    //  tangent
    if( vertShader 
	&& ( shader->shaderType == BUMP_SHADER 
	     || shader->shaderType == GENERALIZED_SHADER ) ) {
      // cgGLSetParameter3f(vertShader->tangentParam, 
      // tangent.x , tangent.y , tangent.z );
      glMultiTexCoord3fARB(GL_TEXTURE7_ARB, tangent.x , tangent.y , tangent.z ); 

      // printf( "tangentParam indFace %d parma %d nb connected faces %d %.2f %.2f %.2f\n" , indFace , vertShader->tangentParam , nbConnectedFaces , tangent.x , tangent.y , tangent.z );
    }
#endif

    // #ifdef VC_HAVE_CG
    //   if( shader ) {
    //     if( shader->shaderType == BUMP_SHADER
    // 	|| shader->shaderType == GENERALIZED_SHADER ) {
    //       if( tabFaces[indFace].indColor1 >= 0 ) {
    // 	// printf( "p1 %.2f %.2f %.2f\n" , p1.x, p1.y, p1.z );
    // 	cgGLSetParameter3fv(vertShader->diffuseParam,
    // 			    &tabColors[tabFaces[indFace].indColor1 * 4] );
    // 	cgGLSetParameter3fv(vertShader->specularParam,
    // 			    white_opaque );
    //       }
    //     }
    //   }
    // #endif

#ifdef NORMAL_LUMISSON
    if( use_VBO ) {
      *pNormalsBufferCour = normal_lumisson.x;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal_lumisson.y;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal_lumisson.z;
      pNormalsBufferCour++;
    }
    else {
      glNormal3f( normal_lumisson.x , normal_lumisson.y , normal_lumisson.z );
    }
#else
    if( use_VBO ) {
      *pNormalsBufferCour = normal.x;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal.y;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal.z;
      pNormalsBufferCour++;
    }
    else {
      glNormal3f( normal.x , normal.y , normal.z );
    }
#endif
    //printf( "normal vertex %d normal %d [%.2f,%.2f,%.2f]\n" , tabFaces[indFace].indVertex1 , tabFaces[indFace].indNormal1  , normal.x , normal.y , normal.z );

    //   if( tabFaces[indFace].indColor1 >= 0 ) {
    //     glColor4fv( &tabColors[tabFaces[indFace].indColor1 * 4] );
    //     //     printf( "Color Vertex1[%.5f,%.5f,%.5f,%.5f]\n" ,
    //     // 	    tabColors[tabFaces[indFace].indColor1 * 4 + 0] ,
    //     // 	    tabColors[tabFaces[indFace].indColor1 * 4 + 1] ,
    //     // 	    tabColors[tabFaces[indFace].indColor1 * 4 + 2] ,
    //     // 	    tabColors[tabFaces[indFace].indColor1 * 4 + 3] );
    //   }

    //fprintf( fileLog , "/%d " , tabFaces[indFace].indVertex1 );
    if( use_VBO ) {
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex1].x;
      pVerticesBufferCour++;
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex1].y;
      pVerticesBufferCour++;
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex1].z;
      pVerticesBufferCour++;
    }
    else {
      int indCurrentVertex = tabFaces[indFace].indVertex1;
      // points should only be displayed once
      if( !( (subdivisionRenderingPrimitive == VC_POINT_SPRITE
	      || subdivisionRenderingPrimitive == VC_POINT ) 
	     && (bool)tabVertices[ indCurrentVertex ].modified == true ) ) {
	tabVertices[ indCurrentVertex ].modified = true;
	glVertex3f( tabVertices[ indCurrentVertex ].x ,
		    tabVertices[ indCurrentVertex ].y ,
		    tabVertices[ indCurrentVertex ].z );
      }
    }
    //   fprintf( fileLog , " Vertex1 %d [%.5f,%.5f,%.5f]\n" , tabFaces[indFace].indVertex1 ,
    // 	  tabVertices[tabFaces[indFace].indVertex1].x ,
    // 	  tabVertices[tabFaces[indFace].indVertex1].y ,
    // 	  tabVertices[tabFaces[indFace].indVertex1].z );



    /////////////////////////////////////////////
    // vertex #2
    /////////////////////////////////////////////
    if( !flatShading ) {
      // NORMAL
      if( tabFaces[indFace].indNormal2 != -1  ) {
	normal.update( tabNormals[tabFaces[indFace].indNormal2].x ,
		       tabNormals[tabFaces[indFace].indNormal2].y ,
		       tabNormals[tabFaces[indFace].indNormal2].z );
      }
      else {
	averageNormal( tabFaces[indFace].indVertex2 , 
		       tabFaces[indFace].indNormal2 ,
		       &normal , nbLevels - 1 ,
		       tabIndConnectedVertices , 
		       tabIndConnectedVerticesCopy ,
		       tabConnectedFaces , 
		       tabConnectedFacesCopy , 
		       &nbConnectedVertices ,
		       &nbConnectedFaces ,
		       normals );
      }
      // TANGENT
      if( tabFaces[indFace].indTangent2 != -1  ) {
	tangent.update( tabTangents[tabFaces[indFace].indTangent2].x ,
			tabTangents[tabFaces[indFace].indTangent2].y ,
			tabTangents[tabFaces[indFace].indTangent2].z );
	// printf( "tangent %d %.2f %.2f %.2f\n" , indFace , tangent.x , tangent.y , tangent.z );
      }
      else {
	tabFaces[indFace].tangent( &tangent , 
				   &normal , 
				   tabFaces[indFace].indVertex2 ,
				   tabVertices , 
				   tabTextureCoords );
      }
      // 	averageTangent( tabFaces[indFace].indVertex2 , 
      // 			&tangent , &normal ,
      // 			tabConnectedFaces , 
      // 			nbConnectedFaces , 
      // 			tangents);
      //       if( normal.y != 0 || normal.z != 0 ) {
      // 	tangent.x = 0;
      // 	tangent.y = normal.z;
      // 	tangent.z = -normal.y;
      //       }
      //       else {
      // 	tangent.x = 0;
      // 	tangent.y = 0;
      // 	tangent.z = 1;
      //       }
      
      // printf( "normal %d %.2f %.2f %.2f\n" , nbConnectedFaces , normal.x , normal.y , normal.z );
      // printf( "tangent %d %.2f %.2f %.2f\n" , nbConnectedFaces , tangent.x , tangent.y , tangent.z );
      if( reversedNormals ) {
	// printf( "normal 2 inversion face %d id %s\n" , indFace , id );
	normal *= -1;
      }
    }

    if( material->hasTexture
	&& tabFaces[indFace].indTextureCoord2 >= 0 ) {
      //     printf(  "Texture[%.5f,%.5f]\n" ,
      // 	     tabTextureCoords[tabFaces[indFace].indTextureCoord2].u
      // 	     * material->textureTile_s ,
      // 	     tabTextureCoords[tabFaces[indFace].indTextureCoord2].v
      //     		  * material->textureTile_t );
      if( use_VBO ) {
	*pTexCoordsBufferCour =
	  tabTextureCoords[tabFaces[indFace].indTextureCoord2].u
	  * material->materialTexture->textureTile_s;
	pTexCoordsBufferCour++;
	*pTexCoordsBufferCour =
	  tabTextureCoords[tabFaces[indFace].indTextureCoord2].v
	  * material->materialTexture->textureTile_t;
	pTexCoordsBufferCour++;
      }
      else {
	glTexCoord2f( tabTextureCoords[tabFaces[indFace].indTextureCoord2].u
		      * material->materialTexture->textureTile_s ,
		      tabTextureCoords[tabFaces[indFace].indTextureCoord2].v
		      * material->materialTexture->textureTile_t );
      }
    }

#ifdef VC_HAVE_CG
    if( gpu_KPanim && vertShader 
	&& shader->shaderType == GENERALIZED_SHADER ) {
      setVertexShaderParams( vertShader , tabFaces[indFace].indVertex2 );
    }

    //  tangent
    if( vertShader 
	&& ( shader->shaderType == BUMP_SHADER 
	     || shader->shaderType == GENERALIZED_SHADER ) ) {
//       cgGLSetParameter3f(vertShader->tangentParam, 
// 			 tangent.x , tangent.y , tangent.z );
      glMultiTexCoord3fARB(GL_TEXTURE7_ARB, tangent.x , tangent.y , tangent.z );      // printf( "tangentParam indFace %d parma %d nb connected faces %d %.2f %.2f %.2f\n" , indFace , vertShader->tangentParam , nbConnectedFaces , tangent.x , tangent.y , tangent.z );
    }
#endif

    // #ifdef VC_HAVE_CG
    //   if( shader ) {
    //     if( shader->shaderType == BUMP_SHADER
    // 	|| shader->shaderType == GENERALIZED_SHADER ) {
    //       if( tabFaces[indFace].indColor2 >= 0 ) {
    // 	// printf( "p2 %.2f %.2f %.2f\n" , p2.x, p2.y, p2.z );
    // 	cgGLSetParameter3fv(vertShader->diffuseParam,
    // 			    &tabColors[tabFaces[indFace].indColor2 * 4] );
    // 	cgGLSetParameter3fv(vertShader->specularParam,
    // 			    white_opaque );
    //       }
    //     }
    //   }
    // #endif

#ifdef NORMAL_LUMISSON
    if( use_VBO ) {
      *pNormalsBufferCour = normal_lumisson.x;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal_lumisson.y;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal_lumisson.z;
      pNormalsBufferCour++;
    }
    else {
      glNormal3f( normal_lumisson.x , normal_lumisson.y , normal_lumisson.z );
    }
#else
    if( use_VBO ) {
      *pNormalsBufferCour = normal.x;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal.y;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal.z;
      pNormalsBufferCour++;
    }
    else {
      glNormal3f( normal.x , normal.y , normal.z );
    }
#endif
    //printf( "normal vertex %d normal %d [%.2f,%.2f,%.2f]\n" , tabFaces[indFace].indVertex2 , tabFaces[indFace].indNormal2  , normal.x , normal.y , normal.z );

    //   if( tabFaces[indFace].indColor2 >= 0 ) {
    //     glColor4fv( &tabColors[tabFaces[indFace].indColor2 * 4] );
    //     //     printf( "Color Vertex2[%.5f,%.5f,%.5f,%.5f]\n" ,
    //     // 	    tabColors[tabFaces[indFace].indColor2 * 4 + 0] ,
    //     // 	    tabColors[tabFaces[indFace].indColor2 * 4 + 2] ,
    //     // 	    tabColors[tabFaces[indFace].indColor2 * 4 + 2] ,
    //     // 	    tabColors[tabFaces[indFace].indColor2 * 4 + 3] );
    //   }

    //fprintf( fileLog , "/%d " , tabFaces[indFace].indVertex2 );
    if( use_VBO ) {
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex2].x;
      pVerticesBufferCour++;
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex2].y;
      pVerticesBufferCour++;
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex2].z;
      pVerticesBufferCour++;
    }
    else {
      int indCurrentVertex = tabFaces[indFace].indVertex2;
      // points should only be displayed once
      if( !( (subdivisionRenderingPrimitive == VC_POINT_SPRITE
	      || subdivisionRenderingPrimitive == VC_POINT ) 
	     && (bool)tabVertices[ indCurrentVertex ].modified == true ) ) {
	tabVertices[ indCurrentVertex ].modified = true;
	glVertex3f( tabVertices[ indCurrentVertex ].x ,
		    tabVertices[ indCurrentVertex ].y ,
		    tabVertices[ indCurrentVertex ].z );
      }
    }
    //   fprintf( fileLog , " Vertex2 %d [%.5f,%.5f,%.5f]\n" , tabFaces[indFace].indVertex2 ,
    // 	  tabVertices[tabFaces[indFace].indVertex2].x ,
    // 	  tabVertices[tabFaces[indFace].indVertex2].y ,
    // 	  tabVertices[tabFaces[indFace].indVertex2].z );



    /////////////////////////////////////////////
    // vertex #3
    /////////////////////////////////////////////
    if( !flatShading ) {
      // NORMAL
      if( tabFaces[indFace].indNormal3 != -1  ) {
	normal.update( tabNormals[tabFaces[indFace].indNormal3].x ,
		       tabNormals[tabFaces[indFace].indNormal3].y ,
		       tabNormals[tabFaces[indFace].indNormal3].z );
      }
      else {
	averageNormal( tabFaces[indFace].indVertex3 , 
		       tabFaces[indFace].indNormal3 ,
		       &normal , nbLevels - 1 ,
		       tabIndConnectedVertices , 
		       tabIndConnectedVerticesCopy ,
		       tabConnectedFaces , 
		       tabConnectedFacesCopy , 
		       &nbConnectedVertices ,
		       &nbConnectedFaces ,
		       normals );
      }
      // TANGENT
      if( tabFaces[indFace].indTangent3 != -1  ) {
	tangent.update( tabTangents[tabFaces[indFace].indTangent3].x ,
			tabTangents[tabFaces[indFace].indTangent3].y ,
			tabTangents[tabFaces[indFace].indTangent3].z );
	// printf( "tangent %d %.2f %.2f %.2f\n" , indFace , tangent.x , tangent.y , tangent.z );
      }
      else {
	tabFaces[indFace].tangent( &tangent , 
				   &normal , 
				   tabFaces[indFace].indVertex2 ,
				   tabVertices , 
				   tabTextureCoords );
      }

      // 	averageTangent( tabFaces[indFace].indVertex3 , 
      // 			&tangent , &normal ,
      // 			tabConnectedFaces , 
      // 			nbConnectedFaces , 
      // 			tangents );
      //       if( normal.y != 0 || normal.z != 0 ) {
      // 	tangent.x = 0;
      // 	tangent.y = normal.z;
      // 	tangent.z = -normal.y;
      //       }
      //       else {
      // 	tangent.x = 0;
      // 	tangent.y = 0;
      // 	tangent.z = 1;
      //       }
      
      // printf( "normal %d %.2f %.2f %.2f\n" , nbConnectedFaces , normal.x , normal.y , normal.z );
      // printf( "tangent %d %.2f %.2f %.2f\n" , nbConnectedFaces , tangent.x , tangent.y , tangent.z );
      if( reversedNormals ) {
	// printf( "normal 3 inversion face %d id %s\n" , indFace , id );
	normal *= -1;
      }
    }

    if( material->hasTexture
	&& tabFaces[indFace].indTextureCoord3 >= 0 ) {
      //     printf(  "Texture[%.5f,%.5f]\n" ,
      // 	     tabTextureCoords[tabFaces[indFace].indTextureCoord3].u
      // 	     * material->textureTile_s ,
      // 	     tabTextureCoords[tabFaces[indFace].indTextureCoord3].v
      //     		  * material->textureTile_t );
      if( use_VBO ) {
	*pTexCoordsBufferCour =
	  tabTextureCoords[tabFaces[indFace].indTextureCoord3].u
	  * material->materialTexture->textureTile_s;
	pTexCoordsBufferCour++;
	*pTexCoordsBufferCour =
	  tabTextureCoords[tabFaces[indFace].indTextureCoord3].v
	  * material->materialTexture->textureTile_t;
	pTexCoordsBufferCour++;
      }
      else {
	glTexCoord2f( tabTextureCoords[tabFaces[indFace].indTextureCoord3].u
		      * material->materialTexture->textureTile_s ,
		      tabTextureCoords[tabFaces[indFace].indTextureCoord3].v
		      * material->materialTexture->textureTile_t );
      }
    }

#ifdef VC_HAVE_CG
    if( gpu_KPanim && vertShader 
	&& shader->shaderType == GENERALIZED_SHADER ) {
      setVertexShaderParams( vertShader , tabFaces[indFace].indVertex3 );
    }

    //  tangent
    if( vertShader 
	&& ( shader->shaderType == BUMP_SHADER 
	     || shader->shaderType == GENERALIZED_SHADER ) ) {
//       cgGLSetParameter3f(vertShader->tangentParam, 
// 			 tangent.x , tangent.y , tangent.z );
      glMultiTexCoord3fARB(GL_TEXTURE7_ARB, tangent.x , tangent.y , tangent.z );      // printf( "tangentParam indFace %d parma %d nb connected faces %d %.2f %.2f %.2f\n" , indFace , vertShader->tangentParam , nbConnectedFaces , tangent.x , tangent.y , tangent.z );
    }
#endif

    // #ifdef VC_HAVE_CG
    //   if( shader ) {
    //     if( shader->shaderType == BUMP_SHADER
    // 	|| shader->shaderType == GENERALIZED_SHADER ) {
    //       if( tabFaces[indFace].indColor3 >= 0 ) {
    // 	// printf( "p3 %.2f %.2f %.2f\n" , p3.x, p3.y, p3.z );
    // 	cgGLSetParameter3fv(vertShader->diffuseParam,
    // 			    &tabColors[tabFaces[indFace].indColor3 * 4] );
    // 	cgGLSetParameter3fv(vertShader->specularParam,
    // 			    white_opaque );
    //       }
    //     }
    //   }
    // #endif

#ifdef NORMAL_LUMISSON
    if( use_VBO ) {
      *pNormalsBufferCour = normal_lumisson.x;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal_lumisson.y;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal_lumisson.z;
      pNormalsBufferCour++;
    }
    else {
      glNormal3f( normal_lumisson.x , normal_lumisson.y , normal_lumisson.z );
    }
#else
    if( use_VBO ) {
      *pNormalsBufferCour = normal.x;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal.y;
      pNormalsBufferCour++;
      *pNormalsBufferCour = normal.z;
      pNormalsBufferCour++;
    }
    else {
      glNormal3f( normal.x , normal.y , normal.z );
    }
#endif
    //printf( "normal vertex %d normal %d [%.2f,%.2f,%.2f]\n" , tabFaces[indFace].indVertex3 , tabFaces[indFace].indNormal3  , normal.x , normal.y , normal.z );

    //   if( tabFaces[indFace].indColor3 >= 0 ) {
    //     glColor4fv( &tabColors[tabFaces[indFace].indColor3 * 4] );
    //     //     printf( "Color Vertex3[%.5f,%.5f,%.5f,%.5f]\n" ,
    //     // 	    tabColors[tabFaces[indFace].indColor3 * 4 + 0] ,
    //     // 	    tabColors[tabFaces[indFace].indColor3 * 4 + 3] ,
    //     // 	    tabColors[tabFaces[indFace].indColor3 * 4 + 2] ,
    //     // 	    tabColors[tabFaces[indFace].indColor3 * 4 + 3] );
    //   }

    //fprintf( fileLog , "/%d " , tabFaces[indFace].indVertex3 );
    if( use_VBO ) {
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex3].x;
      pVerticesBufferCour++;
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex3].y;
      pVerticesBufferCour++;
      *pVerticesBufferCour = tabVertices[tabFaces[indFace].indVertex3].z;
      pVerticesBufferCour++;
    }
    else {
      int indCurrentVertex = tabFaces[indFace].indVertex3;
      // points should only be displayed once
      if( !( (subdivisionRenderingPrimitive == VC_POINT_SPRITE
	      || subdivisionRenderingPrimitive == VC_POINT ) 
	     && (bool)tabVertices[ indCurrentVertex ].modified == true ) ) {
	tabVertices[ indCurrentVertex ].modified = true;
	glVertex3f( tabVertices[ indCurrentVertex ].x ,
		    tabVertices[ indCurrentVertex ].y ,
		    tabVertices[ indCurrentVertex ].z );
      }
    }

    if( !use_VBO ) {
      if( subdivisionRenderingPrimitive == VC_LINE ) {
	glEnd();
      }
      // else if ( subdivisionRenderingPrimitive == VC_POINT
      // 		|| subdivisionRenderingPrimitive == VC_POINT_SPRITE ) {
      // 	glEnd();
      // }
    }

    //   fprintf( fileLog , " Vertex3 %d [%.5f,%.5f,%.5f]\n" , tabFaces[indFace].indVertex3 ,
    // 	  tabVertices[tabFaces[indFace].indVertex3].x ,
    // 	  tabVertices[tabFaces[indFace].indVertex3].y ,
    // 	  tabVertices[tabFaces[indFace].indVertex3].z );
  }
  // printf( " pVerticesBuffer %d\n" , (int)(pVerticesBufferCour - pVerticesBuffer) );
  // printf( " pNormalsBuffer %d\n" , (int)(pNormalsBufferCour - pNormalsBuffer) );
  // printf( " pTexCoordsBuffer %d\n" , (int)(pTexCoordsBufferCour - pTexCoordsBuffer) );

  if( !use_VBO && subdivisionRenderingPrimitive == VC_TRIANGLE ) {
    glEnd();
  }
  else if( !use_VBO && ( subdivisionRenderingPrimitive == VC_POINT
			 || subdivisionRenderingPrimitive == VC_POINT_SPRITE ) ) {
    if(  subdivisionRenderingPrimitive == VC_POINT_SPRITE ) {
      glDisable(GL_POINT_SPRITE);
      glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    }
    glEnd();
  }
}

void vc_Subdivision::displayMeshPOVray( int indFace ,
					int * tabIndConnectedVertices ,
					int * tabIndConnectedVerticesCopy ,
					int * tabConnectedFaces ,
					int * tabConnectedFacesCopy ,
					vc_vector3 * normals ) {
  vc_vector3 normal;
  int nbConnectedVertices;
  int nbConnectedFaces;

  // dessin des faces pleines
  fprintf( filePOVray , "    smooth_triangle {\n     \n" );
  fprintf( filePOVray , "       < %.5f, %.5f, %.5f>, " ,
	   tabVertices[tabFaces[indFace].indVertex1].x ,
	   tabVertices[tabFaces[indFace].indVertex1].y ,
	   tabVertices[tabFaces[indFace].indVertex1].z );
  averageNormal( tabFaces[indFace].indVertex1 , tabFaces[indFace].indNormal1 ,
		 &normal , nbLevels - 1 , 
		 tabIndConnectedVertices , 
		 tabIndConnectedVerticesCopy ,
		 tabConnectedFaces , 
		 tabConnectedFacesCopy , 
		 &nbConnectedVertices ,
		 &nbConnectedFaces ,
		 normals );
  fprintf( filePOVray , "< %.5f, %.5f, %.5f>, \n" ,
	   normal.x , normal.y , normal.z );
  fprintf( filePOVray , "       < %.5f, %.5f, %.5f>, " ,
	   tabVertices[tabFaces[indFace].indVertex2].x ,
	   tabVertices[tabFaces[indFace].indVertex2].y ,
	   tabVertices[tabFaces[indFace].indVertex2].z );
  averageNormal( tabFaces[indFace].indVertex2 ,tabFaces[indFace].indNormal2 ,
		 &normal , nbLevels - 1 , tabIndConnectedVertices , 
		 tabIndConnectedVerticesCopy ,
		 tabConnectedFaces , 
		 tabConnectedFacesCopy , 
		 &nbConnectedVertices ,
		 &nbConnectedFaces ,
		 normals );
  fprintf( filePOVray , "< %.5f, %.5f, %.5f>, \n" ,
	   normal.x , normal.y , normal.z );
  fprintf( filePOVray , "       < %.5f, %.5f, %.5f>, " ,
	   tabVertices[tabFaces[indFace].indVertex3].x ,
	   tabVertices[tabFaces[indFace].indVertex3].y ,
	   tabVertices[tabFaces[indFace].indVertex3].z );
  averageNormal( tabFaces[indFace].indVertex3 , tabFaces[indFace].indNormal3 ,
		 &normal , nbLevels - 1 , tabIndConnectedVertices , 
		 tabIndConnectedVerticesCopy ,
		 tabConnectedFaces , 
		 tabConnectedFacesCopy , 
		 &nbConnectedVertices ,
		 &nbConnectedFaces ,
		 normals );
  fprintf( filePOVray , "< %.5f, %.5f, %.5f> \n" ,
	   normal.x , normal.y , normal.z );
  fprintf( filePOVray , "    }\n" );
}

void vc_Subdivision::updateBoundingBoxAndAutoformat( void ) {
#ifndef VC_LUMISSON
  initSubdivisionMesh();
  updateMesh();
#endif
  updateBoundingBox();

  scale = 1.0;
  if( autoscale ) {
    // boundingBox.print( stdout , 1 );
    float max = fabs(boundingBox.p.x - boundingBox.P.x);
    float max2 =  fabs(boundingBox.p.y - boundingBox.P.y);
    if( max2 > max ) {
      max = max2;
    }
    max2 =  fabs(boundingBox.p.z - boundingBox.P.z);
    if( max2 > max ) {
      max = max2;
    }
    if( max > 0.0 ) {
      scale = 1.0F / max;
    }
    // printf( "    scale %.5f <1.0/%.10f>\n", scale,max);
  }
  tx = 0.0; ty = 0.0; tz = 0.0;
  if( autocenter ) {
    tx = -(boundingBox.p.x + boundingBox.P.x)/2.0F;
    ty = -(boundingBox.p.y + boundingBox.P.y)/2.0F;
    tz = -(boundingBox.p.z + boundingBox.P.z)/2.0F;
    //printf( "    tx y z  %.5f %.5f %.5f\n", tx,ty,tz);
  }
}

void vc_Subdivision::display( DrawingMode mode , 
			      bool *invalidDL ,
			      bool isDisabled , 
			      bool use_VBO , 
			      bool externalMaterial ) {

  int * tabIndConnectedVertices = new int[ max_degree ];
  int * tabIndConnectedVerticesCopy = new int[ max_degree ];
  int * tabConnectedFaces = new int[ max_degree ];
  int * tabConnectedFacesCopy = new int[ max_degree ];
  vc_vector3 * normals = new vc_vector3[ max_degree ];
  vc_vector3 * tangents = new vc_vector3[ max_degree ];

  if( !tabIndConnectedVertices || !tabIndConnectedVerticesCopy 
      || !tabConnectedFaces || !tabConnectedFacesCopy || !normals 
      || !tangents 
      ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }

  // Renderman rendering
  if( mode == _Renderman ) {
    // TODO
  }
  // POVray rendering
  else if( mode == _POVray ) {
    fprintf( filePOVray , "  mesh {\n    " );
    for (int i = indFacesIni[ nbLevels - 1 ] ;
	 i < indFacesEnd[ nbLevels - 1 ] ; i++) {
      displayMeshPOVray( i , tabIndConnectedVertices ,
			 tabIndConnectedVerticesCopy ,
			 tabConnectedFaces ,
			 tabConnectedFacesCopy ,
			 normals );
    }
    if( autoscale ) {
      fprintf( filePOVray , "    scale <%.5f,%.5f,%.5f>\n", scale,scale,scale);
    }
    if( autocenter ) {
      fprintf( filePOVray , "    translate <%.5f,%.5f,%.5f>\n", tx,ty,tz );
    }
  }
  // interactive rendering
  else {
    glPushMatrix();

    // printf(  "Mesh ID %s Use VBO %d\n", id , use_VBO );
      if( use_VBO 
	  && ( ( indFacesEnd[ nbLevels - 1 ] - indFacesIni[ nbLevels - 1 ] )
	       != bufferFaceNumber ) ) {
	// Allocate Vertex Data
	if( pVerticesBuffer )
	  delete [] pVerticesBuffer; 

	// Allocate Tex Coord Data
	if( pTexCoordsBuffer )  {
	  delete [] pTexCoordsBuffer; 
	  pTexCoordsBuffer = NULL;
	}
	
	// Allocate Normals Data
	if( pNormalsBuffer )  {
	  delete [] pNormalsBuffer; 
	  pNormalsBuffer = NULL;
	}
	
	// Allocate Tangents Data
	if( pTangentsBuffer )  {
	  delete [] pTangentsBuffer; 
	  pTangentsBuffer = NULL;
	}
	
	bufferFaceNumber 
	  = indFacesEnd[ nbLevels - 1 ] - indFacesIni[ nbLevels - 1 ];
	
	// Allocate Vertex Data
	// nb faces * 3 vertices * 3 coordinates
	pVerticesBuffer = new float[ bufferFaceNumber * 3 * 3 ]; 

	// Allocate Tex Coord Data
	// nb faces * 3 vertices * 2 coordinates 
	pTexCoordsBuffer = new float[ bufferFaceNumber * 3 * 2 ]; 

	// Allocate Normal Data
	// nb faces * 3 normals * 3 coordinates
	pNormalsBuffer = new float[ bufferFaceNumber * 3 * 3 ]; 

	// Allocate Tangent Data
	// nb faces * 3 tangents * 3 coordinates
	pTangentsBuffer = new float[ bufferFaceNumber * 3 * 3 ]; 

	// printf( " Allocate %d\n" , bufferFaceNumber * 3 * 3 );
      }

      // OpenGL mode
      if( !use_VBO) {
	bufferFaceNumber 
	  = indFacesEnd[ nbLevels - 1 ] - indFacesIni[ nbLevels - 1 ];

	// material rendering must not be performed at
	// during display for VBO rendering, because
	// it is made during renderVBO
	// printf( "Draw material / subdivision %s\n" , id );
	if( !externalMaterial ) {
	  material->display( mode , invalidDL , isDisabled );
	}
      }

      // printf( "Render subdivision\n" );
      //  rendering all the faces
      displayMesh( tabIndConnectedVertices ,
		   tabIndConnectedVerticesCopy ,
		   tabConnectedFaces ,
		   tabConnectedFacesCopy ,
		   normals , 
		   tangents , 
		   use_VBO );

      if( !use_VBO ) {
	if( !externalMaterial ) {
	  material->close( mode );
	}
      }
    glPopMatrix();
  }

  delete [] tabIndConnectedVertices;
  tabIndConnectedVertices = NULL;

  delete [] tabIndConnectedVerticesCopy;
  tabIndConnectedVerticesCopy = NULL;

  delete [] tabConnectedFaces;
  tabConnectedFaces = NULL;

  delete [] tabConnectedFacesCopy;
  tabConnectedFacesCopy = NULL;

  delete [] normals;
  normals = NULL;

  delete [] tangents;
  tangents = NULL;
}

void vc_Subdivision::loadVBO( int verticesVBOIndex , 
			      int normalsVBOIndex , 
			      int texCoordsVBOIndex ) {
#ifdef VC_HAVE_VBO
  // * 3 because bufferFaceNumber is the number of triangular faces
  // * 2 for textures and * 3 for normal and vertices

  // Binds The Vertex Buffer
  MyglBindBufferARB( GL_ARRAY_BUFFER_ARB, verticesVBOIndex );
  // Load The Data
  MyglBufferDataARB( GL_ARRAY_BUFFER_ARB, bufferFaceNumber*3*3*sizeof(float), 
		     pVerticesBuffer, GL_DYNAMIC_DRAW_ARB );

  // Binds The Normal Buffer
  MyglBindBufferARB( GL_ARRAY_BUFFER_ARB, normalsVBOIndex );
  // Load The Data
  MyglBufferDataARB( GL_ARRAY_BUFFER_ARB, bufferFaceNumber*3*3*sizeof(float), 
		     pNormalsBuffer, GL_DYNAMIC_DRAW_ARB );

  // Binds The TexCoord Buffer
  MyglBindBufferARB( GL_ARRAY_BUFFER_ARB, texCoordsVBOIndex );
  // Load The Data
  // 2 coordinates
  MyglBufferDataARB( GL_ARRAY_BUFFER_ARB, bufferFaceNumber*3*2*sizeof(float), 
		     pTexCoordsBuffer, GL_DYNAMIC_DRAW_ARB );
#else
  // Binds The Vertex Buffer
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, verticesVBOIndex );
  // Load The Data
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, bufferFaceNumber*3*3*sizeof(float), 
		   pVerticesBuffer, GL_DYNAMIC_DRAW_ARB );

  // Binds The Normal Buffer
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, normalsVBOIndex );
  // Load The Data
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, bufferFaceNumber*3*3*sizeof(float), 
		   pNormalsBuffer, GL_DYNAMIC_DRAW_ARB );

  // Binds The TexCoord Buffer
  glBindBufferARB( GL_ARRAY_BUFFER_ARB, texCoordsVBOIndex );
  // Load The Data
  // 2 coordinates
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, bufferFaceNumber*3*2*sizeof(float), 
		   pTexCoordsBuffer, GL_DYNAMIC_DRAW_ARB );
#endif
}

void vc_Subdivision::renderVBO( DrawingMode mode , 
				bool *invalidDL , 
				bool isDisabled ,
				int verticesVBOIndex , 
				int normalsVBOIndex , 
				int texCoordsVBOIndex , 
				bool externalMaterial ) {
  // Enable Pointers
  glEnableClientState( GL_VERTEX_ARRAY );
  // Enable Vertex Arrays
  glEnableClientState( GL_NORMAL_ARRAY ); 
  // Enable Normal Arrays
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  // Enable Texture Coord Arrays

#ifdef VC_HAVE_VBO
  MyglBindBufferARB( GL_ARRAY_BUFFER, verticesVBOIndex );
  glVertexPointer( 3, GL_FLOAT, 0, (char *) NULL );
  // Set The Vertex Pointer To The Vertex Buffer

  MyglBindBufferARB( GL_ARRAY_BUFFER, normalsVBOIndex );
  glNormalPointer( GL_FLOAT, 0, (char *) NULL );
  // Set The Normal Pointer To The Normal Buffer

  MyglBindBufferARB( GL_ARRAY_BUFFER, texCoordsVBOIndex );
  // 2 coordinates
  glTexCoordPointer( 2, GL_FLOAT, 0, (char *) NULL );
  // Set The TexCoord Pointer To The TexCoord Buffer
#else
  glBindBuffer( GL_ARRAY_BUFFER, verticesVBOIndex );
  glVertexPointer( 3, GL_FLOAT, 0, (char *) NULL );
  // Set The Vertex Pointer To The Vertex Buffer

  glBindBuffer( GL_ARRAY_BUFFER, normalsVBOIndex );
  glNormalPointer( GL_FLOAT, 0, (char *) NULL );
  // Set The Normal Pointer To The Normal Buffer

  glBindBuffer( GL_ARRAY_BUFFER, texCoordsVBOIndex );
  // 2 coordinates 
  glTexCoordPointer( 2, GL_FLOAT, 0, (char *) NULL );
  // Set The TexCoord Pointer To The TexCoord Buffer
#endif

  // printf("render material %s\n",id);
  if( !externalMaterial ) {
    material->display( mode , invalidDL , isDisabled );
  }

  // Render
  if( subdivisionRenderingPrimitive == VC_TRIANGLE ) {
    glDrawArrays( GL_TRIANGLES, 0, bufferFaceNumber * 3 );
  }
  else if( subdivisionRenderingPrimitive == VC_LINE ) {
    glDrawArrays( GL_LINE_LOOP, 0, bufferFaceNumber * 3 );
  }
  else { // ( subdivisionRenderingPrimitive == VC_POINT 
         //   || subdivisionRenderingPrimitive == VC_POINT_SPRITE ) 
    glDrawArrays( GL_POINTS, 0, bufferFaceNumber * 3 );
  }
  // Draw All Of The Triangles At Once
  
  if( !externalMaterial ) {
    material->close( mode );
  }

  // Disable Pointers
  glDisableClientState( GL_VERTEX_ARRAY );
  // Disable Vertex Arrays
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  // Disable Texture Coord Arrays
  glDisableClientState( GL_NORMAL_ARRAY );
  // Disable Normal Arrays
}

void vc_Subdivision::interpolate( vc_Subdivision *n1 , vc_Subdivision *n2 ) {
  coefLoop1 += generateRandomFloat( -n2->coefLoop1 , n2->coefLoop1 );
  coefLoop2 += generateRandomFloat( -n2->coefLoop2 , n2->coefLoop2 );
  coefMB1 += generateRandomFloat( -n2->coefMB1 , n2->coefMB1 );
  coefMB2 += generateRandomFloat( -n2->coefMB2 , n2->coefMB2 );
  coefMB3 += generateRandomFloat( -n2->coefMB3 , n2->coefMB3 );
  //nbLevels += generateRandomInt( -n2->nbLevels , n2->nbLevels );
  if( nbLevels < 0 ) {
    nbLevels = 0;
  }
  // printf(  "random stepwise motion Loop1, Loop2, MB1, MB2, MB3, nbLevels %.4f %.4f %.4f %.4f %.4f %d\n",coefLoop1, coefLoop2, coefMB1, coefMB2, coefMB3, nbLevels);


  for( int indPoint = 0 ; indPoint < indVerticesEnd[ 0 ] ; indPoint++ ) {
    tabVertices[indPoint].interpolate( &(n1->tabVertices[indPoint]) , &(n2->tabVertices[indPoint]) );
  }

  for( int indPoint = 0 ; indPoint < indNormalsEnd[ 0 ] ; indPoint++ ) {
    tabNormals[indPoint].interpolate( &(n1->tabNormals[indPoint]) , &(n2->tabNormals[indPoint]) );
    tabTangents[indPoint].interpolate( &(n1->tabTangents[indPoint]) , &(n2->tabTangents[indPoint]) );
  }

//   for( int indPoint = 0 ; indPoint < indColorsEnd[ 0 ] ; indPoint++ ) {
//     GLfloat * tabCol =  &tabColors[indPoint * 4];
//     GLfloat * tabCol2 = &(n2->tabColors[indPoint * 4]);
//     for( int ind = 0 ; ind < 4 ; ind++ ) {
//       tabCol[ind] += generateRandomFloat( -tabCol2[ind] , tabCol2[ind] );
//     }
//   }
}

void vc_Subdivision::interpolate( vc_Subdivision **tabSubdivision ,
				  float *weights1 ,
				  float *weights2 ,
				  int numberOfComponents ,
				  float coef ) {

  // printf(  "n-ary interpolation %.4f (%s)\n",coef,id);
  // tabSubdivision[0]->print(stdout,true,1);
  // tabSubdivision[1]->print(stdout,true,1);

  float coef2 = 1.0F - coef;
  float K1;
  float K2;
  K1 = 0.0; K2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    K1 += weights1[ind] * tabSubdivision[ind]->coefLoop1;
    K2 += weights2[ind] * tabSubdivision[ind]->coefLoop1;
  }
  coefLoop1 = K1 * coef + K2 * coef2;
  K1 = 0.0; K2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    K1 += weights1[ind] * tabSubdivision[ind]->coefLoop2;
    K2 += weights2[ind] * tabSubdivision[ind]->coefLoop2;
  }
  coefLoop2 = K1 * coef + K2 * coef2;
  K1 = 0.0; K2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    K1 += weights1[ind] * tabSubdivision[ind]->coefMB1;
    K2 += weights2[ind] * tabSubdivision[ind]->coefMB1;
  }
  coefMB1 = K1 * coef + K2 * coef2;
  K1 = 0.0; K2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    K1 += weights1[ind] * tabSubdivision[ind]->coefMB2;
    K2 += weights2[ind] * tabSubdivision[ind]->coefMB2;
  }
  coefMB2 = K1 * coef + K2 * coef2;
  K1 = 0.0; K2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    K1 += weights1[ind] * tabSubdivision[ind]->coefMB3;
    K2 += weights2[ind] * tabSubdivision[ind]->coefMB3;
  }
  coefMB3 = K1 * coef + K2 * coef2;

  K1 = 0.0; K2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    K1 += weights1[ind] * tabSubdivision[ind]->nbLevels;
    K2 += weights2[ind] * tabSubdivision[ind]->nbLevels;
  }
  nbLevels = (int)my_round(K1 * coef + K2 * coef2);
  //printf( "nbLevels %.2f %.2f %d\n" , K1 * coef , K2 * coef2 , nbLevels );

//   for( int indColor = 0 ; indColor < indColorsEnd[ 0 ] ; indColor++ ) {
//     for( int ind = 0 ; ind < 4 ; ind++ ) {
//       K1 = 0.0; K2 = 0.0;
//       for( int indComp = 0 ; indComp < numberOfComponents ; indComp++ ) {
// 	K1 += weights1[indComp]
// 	  * tabSubdivision[indComp]->tabColors[indColor * 4 + ind];
// 	K2 += weights2[indComp]
// 	  * tabSubdivision[indComp]->tabColors[indColor * 4 + ind];
//       }
//       tabColors[indColor * 4 + ind] = K1 * coef + K2 * coef2;
//     }
//   }

  //printf(  "Coefs coef coefLoop1, coefLoop2, coefMB1, coefMB2, coefMB3 %.2f %.2f %.2f %.2f %.2f %.2f \n",coef,coefLoop1, coefLoop2, coefMB1, coefMB2, coefMB3);

  vc_Vertex **local_tabVertices = new vc_Vertex*[ numberOfComponents ];
  if( !local_tabVertices ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int indPoint = 0 ; indPoint < indVerticesEnd[ 0 ] ; indPoint++ ) {
    for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
      local_tabVertices[ind] = &(tabSubdivision[ind]->tabVertices[indPoint]);
    }
    tabVertices[indPoint].interpolate( local_tabVertices ,
				       weights1 , weights2 ,
				       numberOfComponents , coef );
  }
  delete [] local_tabVertices;
  local_tabVertices = NULL;

  vc_vector3 **local_tabNormals = new vc_vector3*[ numberOfComponents ];
  if( !local_tabNormals ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int indPoint = 0 ; indPoint < indNormalsEnd[ 0 ] ; indPoint++ ) {
    for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
      local_tabNormals[ind] = &(tabSubdivision[ind]->tabNormals[indPoint]);
    }
    tabNormals[indPoint].interpolate( local_tabNormals ,
				      weights1 , weights2 ,
				      numberOfComponents , coef );
  }
  delete [] local_tabNormals;
  local_tabNormals = NULL;

  vc_vector3 **local_tabTangents = new vc_vector3*[ numberOfComponents ];
  if( !local_tabTangents ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int indPoint = 0 ; indPoint < indNormalsEnd[ 0 ] ; indPoint++ ) {
    for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
      local_tabTangents[ind] = &(tabSubdivision[ind]->tabTangents[indPoint]);
    }
    tabTangents[indPoint].interpolate( local_tabTangents ,
				       weights1 , weights2 ,
				       numberOfComponents , coef );
  }
  delete [] local_tabTangents;
  local_tabTangents = NULL;
}

int vc_Subdivision::findsReplacementVertex(  int indInitialVertex , int level ) {
  //printf("indVertex %d %d %d\n", level,indVerticesIni[ level ],indVerticesEnd[ level ]);
  for (int i = indVerticesIni[ level ] ; i < indVerticesEnd[ level ] ; i++) {
    // vertex 1, 2 or 3 belongs to face #i
    //     if( tabVertices[i].vertexType == REPLACEMENT_VERTEX ) {
    //       printf("-> %d\n",tabVertices[i].indReplacedVertex);
    //     }
    if( tabVertices[i].vertexType == REPLACEMENT_VERTEX
	&& tabVertices[i].indReplacedVertex == indInitialVertex ) {
      return i;
    }
  }
  return -1;
}

int vc_Subdivision::findsSplitVertex( int indInitialVertex ,
				      int indFinalVertex ,
				      int level ) {
  for (int i = indVerticesIni[ level ] ; i < indVerticesEnd[ level ] ; i++) {
    // vertex 1, 2 or 3 belongs to face #i
    if( tabVertices[i].vertexType == SPLITTING_VERTEX
	&&
	((tabVertices[i].indSegmentInitialVertex == indInitialVertex
	  && tabVertices[i].indSegmentFinalVertex == indFinalVertex)
	 || (tabVertices[i].indSegmentInitialVertex == indFinalVertex
	     && tabVertices[i].indSegmentFinalVertex == indInitialVertex)) ) {
      return i;
    }
  }
  return -1;
}

int vc_Subdivision::findsAdjacentFace( int nbConnectedFaces ,
				       int *tabConnectedFaces ,
				       int indVertex ,
				       int *tabIndConnectedVertices ,
				       int *nbConnectedVertices ) {
  // finds the face that is adjacent to the preceding one
  int indexOfConnectedFace = -1;
  for (int f = 1 ; f < nbConnectedFaces ; f++) {
    int i = tabConnectedFaces[f];

    // tabConnectedFaces[f] is set to -1 when face is used
    if( i >= 0 ) {
      // preceding connected vertex is vertex 1 of  face #i
      if( tabFaces[i].indVertex1
	  == tabIndConnectedVertices[*nbConnectedVertices - 1] ) {
	if( tabFaces[i].indVertex2 == indVertex ) {
	  tabIndConnectedVertices[*nbConnectedVertices]
	    = tabFaces[i].indVertex3;
	  (*nbConnectedVertices)++;
	  if( (*nbConnectedVertices) >= max_degree ) {
	    sprintf( ErrorStr , "Error: Parameter value max_degree should be increased (%d) subdivision ID [%s]!" , max_degree , id ); ReportError( ErrorStr ); throw 403;	    
	  }
	}
	else if( tabFaces[i].indVertex3 == indVertex ) {
	  tabIndConnectedVertices[*nbConnectedVertices]
	    = tabFaces[i].indVertex2;
	  (*nbConnectedVertices)++;
	  if( (*nbConnectedVertices) >= max_degree ) {
	    sprintf( ErrorStr , "Error: Parameter value max_degree should be increased (%d) subdivision ID [%s]!" , max_degree , id ); ReportError( ErrorStr ); throw 403;	    
	  }
	}
	else {
	  sprintf( ErrorStr , "Error: Expected connected vertex not found in subdivision (1) ID [%s]!" , id ); ReportError( ErrorStr ); throw 403;
	}
	// 	    printf("%d / " ,
	// 		   tabIndConnectedVertices[*nbConnectedVertices - 1] );
	tabConnectedFaces[f] = -1;
	indexOfConnectedFace = f;
	break;
      }

      // preceding connected vertex is vertex 2 of  face #i
      if( tabFaces[i].indVertex2
	  == tabIndConnectedVertices[*nbConnectedVertices - 1] ) {
	if( tabFaces[i].indVertex1 == indVertex ) {
	  tabIndConnectedVertices[*nbConnectedVertices]
	    = tabFaces[i].indVertex3;
	  (*nbConnectedVertices)++;
	  if( (*nbConnectedVertices) >= max_degree ) {
	    sprintf( ErrorStr , "Error: Parameter value max_degree should be increased (%d) subdivision ID [%s]!" , max_degree , id ); ReportError( ErrorStr ); throw 403;	    
	  }
	}
	else if( tabFaces[i].indVertex3 == indVertex ) {
	  tabIndConnectedVertices[*nbConnectedVertices]
	    = tabFaces[i].indVertex1;
	  (*nbConnectedVertices)++;
	  if( (*nbConnectedVertices) >= max_degree ) {
	    sprintf( ErrorStr , "Error: Parameter value max_degree should be increased (%d) subdivision ID [%s]!" , max_degree , id ); ReportError( ErrorStr ); throw 403;	    
	  }
	}
	else {
	  sprintf( ErrorStr , "Error: Expected connected vertex not found in subdivision (2) ID [%s]!" , id ); ReportError( ErrorStr ); throw 399;
	}
	// 	    printf("%d / " ,
	// 		   tabIndConnectedVertices[*nbConnectedVertices - 1] );
	tabConnectedFaces[f] = -1;
	indexOfConnectedFace = f;
	break;
      }

      // preceding connected vertex is vertex 3 of  face #i
      if( tabFaces[i].indVertex3
	  == tabIndConnectedVertices[*nbConnectedVertices - 1] ) {
	if( tabFaces[i].indVertex1 == indVertex ) {
	  tabIndConnectedVertices[*nbConnectedVertices]
	    = tabFaces[i].indVertex2;
	  (*nbConnectedVertices)++;
	  if( (*nbConnectedVertices) >= max_degree ) {
	    sprintf( ErrorStr , "Error: Parameter value max_degree should be increased (%d) subdivision ID [%s]!" , max_degree , id ); ReportError( ErrorStr ); throw 403;	    
	  }
	}
	else if( tabFaces[i].indVertex2 == indVertex ) {
	  tabIndConnectedVertices[*nbConnectedVertices]
	    = tabFaces[i].indVertex1;
	  (*nbConnectedVertices)++;
	  if( (*nbConnectedVertices) >= max_degree ) {
	    sprintf( ErrorStr , "Error: Parameter value max_degree should be increased (%d) subdivision ID [%s]!" , max_degree , id ); ReportError( ErrorStr ); throw 403;	    
	  }
	}
	else {
	  sprintf( ErrorStr , "Error: Expected connected vertex not found in subdivision (3) ID [%s]!" , id ); ReportError( ErrorStr ); throw 400;
	}
	// 	    printf("%d / " ,
	// 		   tabIndConnectedVertices[*nbConnectedVertices - 1] );
	tabConnectedFaces[f] = -1;
	indexOfConnectedFace = f;
	break;
      }

    }

  }
  // finds the face that is adjacent to the preceding one

  return indexOfConnectedFace;
}

void vc_Subdivision::collect_connected_vertices( int indVertex ,
						 int *tabIndConnectedVertices ,
						 int *tabIndConnectedVerticesCopy ,
						 int *nbConnectedVertices ,
						 int *tabConnectedFaces ,
						 int *tabConnectedFacesCopy ,
						 int *nbConnectedFaces ,
						 int nbMaxConnectedVertices ,
						 int level ) {
  // first collects the indices of the connected faces
  for (int i = indFacesIni[ level ] ; i < indFacesEnd[ level ] ; i++) {

    // vertex 1, 2 or 3 belongs to face #i
    // printf( "face %d vertices %d %d %d\n" , i , tabFaces[i].indVertex1 , tabFaces[i].indVertex2 , tabFaces[i].indVertex3 );
    if( tabFaces[i].indVertex1 == indVertex
	|| tabFaces[i].indVertex2 == indVertex
	|| tabFaces[i].indVertex3 == indVertex ) {

      if( *nbConnectedFaces < max_degree - 1 ) {
	tabConnectedFaces[*nbConnectedFaces] = i;
	tabConnectedFacesCopy[*nbConnectedFaces] = i;
	(*nbConnectedFaces)++;
	// printf( "face %d nbConnectedFaces %d max_degree %d\n" , i , *nbConnectedFaces , max_degree );
      }
      else {
	sprintf( ErrorStr , "Error: Insufficient parameter max_degree [%d] in subdivision ID [%s] (vertex %d level %d)!" , max_degree , id , indVertex , level ); ReportError( ErrorStr ); throw 395;
      }
    }
  }

  if( *nbConnectedFaces <= 0 ) {
    sprintf( ErrorStr , "Error: Isolated vertex #%d level %d in subdivision ID [%s]!" , indVertex , level , id ); ReportError( ErrorStr ); throw 396;
  }

  if( nbMaxConnectedVertices < 3 ) {
    sprintf( ErrorStr , "Error: Minimal arity for each vertex should be > 3 in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 397;
  }

  // second collects the indices of the connected vertices
  int ind = 0;
  if( tabFaces[tabConnectedFaces[0]].indVertex1 == indVertex ) {
    // avoids degenerated triangles
    if( tabFaces[tabConnectedFaces[0]].indVertex2 != indVertex ) {
      tabIndConnectedVertices[ind++]
	= tabFaces[tabConnectedFaces[0]].indVertex2;
    }
    if( tabFaces[tabConnectedFaces[0]].indVertex3 != indVertex ) {
      tabIndConnectedVertices[ind++]
	= tabFaces[tabConnectedFaces[0]].indVertex3;
    }
  }
  else if( tabFaces[tabConnectedFaces[0]].indVertex2 == indVertex ) {
    if( tabFaces[tabConnectedFaces[0]].indVertex3 != indVertex ) {
      tabIndConnectedVertices[ind++]
	= tabFaces[tabConnectedFaces[0]].indVertex3;
    }
    if( tabFaces[tabConnectedFaces[0]].indVertex1 != indVertex ) {
      tabIndConnectedVertices[ind++]
	= tabFaces[tabConnectedFaces[0]].indVertex1;
    }
  }
  else if( tabFaces[tabConnectedFaces[0]].indVertex3 == indVertex ) {
    if( tabFaces[tabConnectedFaces[0]].indVertex1 != indVertex ) {
      tabIndConnectedVertices[ind++]
	= tabFaces[tabConnectedFaces[0]].indVertex1;
    }
    if( tabFaces[tabConnectedFaces[0]].indVertex2 != indVertex ) {
      tabIndConnectedVertices[ind++]
	= tabFaces[tabConnectedFaces[0]].indVertex2;
    }
  }
  else {
    sprintf( ErrorStr , "Error: Expected vertex not found in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 398;
  }
  *nbConnectedVertices = ind;
  if( (*nbConnectedVertices) >= max_degree ) {
    sprintf( ErrorStr , "Error: Parameter value max_degree should be increased (%d) subdivision ID [%s]!" , max_degree , id ); ReportError( ErrorStr ); throw 403;	    
  }
  tabConnectedFacesCopy[0] =  -1;

  //     printf("vertex %d : %d / %d / " ,
  // 	   indVertex ,
  // 	   tabIndConnectedVertices[0] ,
  // 	   tabIndConnectedVertices[1] );

  // for all the connected faces except the first one
  for (int connectedFace = 1 ; connectedFace < *nbConnectedFaces ;
       connectedFace++) {
    // tries to find an adjacent face
    int indexOfConnectedFace = findsAdjacentFace(  *nbConnectedFaces ,
						   tabConnectedFacesCopy ,
						   indVertex ,
						   tabIndConnectedVertices ,
						   nbConnectedVertices );
    if( indexOfConnectedFace == - 1 ) {
      // the central vertex is a border vertex
      // the order of the faces is reversed in order
      // to find a face that is adjacent to the first one
      for( int ind = 0 ; ind < *nbConnectedVertices ; ind ++ ) {
	tabIndConnectedVerticesCopy[ *nbConnectedVertices - 1 - ind ]
	  = tabIndConnectedVertices[ind];
      }
      for( int ind = 0 ; ind < *nbConnectedVertices ; ind ++ ) {
	tabIndConnectedVertices[ind] = tabIndConnectedVerticesCopy[ind];
      }

      // tries to find an adjacent face with a reversed order
      // for adjacent vertices
      indexOfConnectedFace = findsAdjacentFace(  *nbConnectedFaces ,
						 tabConnectedFacesCopy ,
						 indVertex ,
						 tabIndConnectedVertices ,
						 nbConnectedVertices );
      //       if( indexOfConnectedFace == - 1 ) {
      // 	printf( "%d connected faces and %d connected vertices: " ,
      // 		*nbConnectedFaces , *nbConnectedVertices );
      // 	for( int ind = 0 ; ind < *nbConnectedVertices ; ind ++ ) {
      // 	  printf( "%d " , tabIndConnectedVertices[ind] );
      // 	}
      // 	printf( "\n" );
      // 	sprintf( ErrorStr , "Error: adjacent face not found in subdivision ID [%s] vertex %d!" , id , indVertex ); ReportError( ErrorStr );// throw 400;
      //       }
    }
  }

  // if the central vertex is not an edge vertex, first and last
  // connected vertices are the same
  // decrements the number of connected vertices to return
  // each connected vertex only once
  if( (*nbConnectedVertices) >= max_degree ) {
    sprintf( ErrorStr , "Error: Parameter value max_degree should be increased (%d) subdivision ID [%s]!" , max_degree , id ); ReportError( ErrorStr ); throw 403;	    
  }
  if( tabIndConnectedVertices[0]
      == tabIndConnectedVertices[*nbConnectedVertices - 1] ) {
    (*nbConnectedVertices)--;
  }
  // for all the connected faces except the first one
  //     printf("\n");
}

void vc_Subdivision::subdividesAFace(  int indInitialFace , int *indNewFace ,
				       int *indNewVertex , int *indNewNormal ,
				       int *indNewTangent ,
// 				       int *indNewColor ,
				       int *indNewTextureCoord ,
				       int level ) {

  //printf("findsReplacementVertex %d\n",tabFaces[indInitialFace].indVertex1);
  int indReplacement1 =
    findsReplacementVertex( tabFaces[indInitialFace].indVertex1 , level );
  int indNormalReplacement1 = tabFaces[indInitialFace].indNormal1;
  int indTangentReplacement1 = tabFaces[indInitialFace].indTangent1;
//   int indColorReplacement1 = tabFaces[indInitialFace].indColor1;
  int indTextureCoordReplacement1 = tabFaces[indInitialFace].indTextureCoord1;
  if( indReplacement1 < 0 ) {
    tabVertices[*indNewVertex]
      = tabVertices[tabFaces[indInitialFace].indVertex1];
    tabVertices[*indNewVertex].vertexType = REPLACEMENT_VERTEX;
    tabVertices[*indNewVertex].indReplacedVertex
      = tabFaces[indInitialFace].indVertex1;
    indReplacement1 = *indNewVertex;
    (*indNewVertex)++;
    indVerticesEnd[ level ] = *indNewVertex;
    if( *indNewVertex >= nbMaxVertices ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_vertices [%d] in subdivision ID [%s]!" , nbMaxVertices , id ); ReportError( ErrorStr ); throw 384;
    }
  }

  int indReplacement2 =
    findsReplacementVertex( tabFaces[indInitialFace].indVertex2 , level );
  int indNormalReplacement2 = tabFaces[indInitialFace].indNormal2;
  int indTangentReplacement2 = tabFaces[indInitialFace].indTangent2;
//   int indColorReplacement2 = tabFaces[indInitialFace].indColor2;
  int indTextureCoordReplacement2 = tabFaces[indInitialFace].indTextureCoord2;
  if( indReplacement2 < 0 ) {
    tabVertices[*indNewVertex]
      = tabVertices[tabFaces[indInitialFace].indVertex2];
    tabVertices[*indNewVertex].vertexType = REPLACEMENT_VERTEX;
    tabVertices[*indNewVertex].indReplacedVertex
      = tabFaces[indInitialFace].indVertex2;
    indReplacement2 = *indNewVertex;
    (*indNewVertex)++;
    indVerticesEnd[ level ] = *indNewVertex;
    if( *indNewVertex >= nbMaxVertices ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_vertices [%d] in subdivision ID [%s]!" , nbMaxVertices , id ); ReportError( ErrorStr ); throw 385;
    }
  }

  int indReplacement3 =
    findsReplacementVertex( tabFaces[indInitialFace].indVertex3 , level );
  int indNormalReplacement3 = tabFaces[indInitialFace].indNormal3;
  int indTangentReplacement3 = tabFaces[indInitialFace].indTangent3;
//   int indColorReplacement3 = tabFaces[indInitialFace].indColor3;
  int indTextureCoordReplacement3 = tabFaces[indInitialFace].indTextureCoord3;
  if( indReplacement3 < 0 ) {
    tabVertices[*indNewVertex]
      = tabVertices[tabFaces[indInitialFace].indVertex3];
    tabVertices[*indNewVertex].vertexType = REPLACEMENT_VERTEX;
    tabVertices[*indNewVertex].indReplacedVertex
      = tabFaces[indInitialFace].indVertex3;
    indReplacement3 = *indNewVertex;
    (*indNewVertex)++;
    indVerticesEnd[ level ] = *indNewVertex;
    if( *indNewVertex >= nbMaxVertices ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_vertices [%d] in subdivision ID [%s]!" , nbMaxVertices , id ); ReportError( ErrorStr ); throw 386;
    }
  }

  int indSplit1 =
    findsSplitVertex( tabFaces[indInitialFace].indVertex1 ,
		      tabFaces[indInitialFace].indVertex2 , level );
  int indNormalSplit1 = -1;
  int indTangentSplit1 = -1;
//   int indColorSplit1 = -1;
  int indTextureCoordSplit1 = -1;
  if( indSplit1 < 0 ) {
    // creates a new vertex that is the middle point of the
    // 2 initial vertices
    tabVertices[*indNewVertex].middlePoint(
					   tabVertices[tabFaces[indInitialFace].indVertex1] ,
					   tabVertices[tabFaces[indInitialFace].indVertex2] );
    tabVertices[*indNewVertex].vertexType = SPLITTING_VERTEX;
    tabVertices[*indNewVertex].indSegmentInitialVertex
      = tabFaces[indInitialFace].indVertex1;
    tabVertices[*indNewVertex].indSegmentFinalVertex
      = tabFaces[indInitialFace].indVertex2;
    indSplit1 = *indNewVertex;
    (*indNewVertex)++;
    indVerticesEnd[ level ] = *indNewVertex;
    if( *indNewVertex >= nbMaxVertices ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_vertices [%d] for splitting in subdivision ID [%s]!" , nbMaxVertices , id ); ReportError( ErrorStr ); throw 387;
    }
    // creates a new normal that is the average of the
    // normals of the 2 initial vertices
    tabNormals[*indNewNormal].averageNormalizedVector(
		      tabNormals[tabFaces[indInitialFace].indNormal1] ,
		      tabNormals[tabFaces[indInitialFace].indNormal2] );
    tabTangents[*indNewNormal].averageNormalizedVector(
		      tabTangents[tabFaces[indInitialFace].indNormal1] ,
		      tabTangents[tabFaces[indInitialFace].indNormal2] );
    indNormalSplit1 = *indNewNormal;
    (*indNewNormal)++;
    indNormalsEnd[ level ] = *indNewNormal;
    tabVertices[indSplit1].indNewNormal  = indNormalSplit1;
    if( *indNewNormal >= nbMaxNormals ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_normals [%d] for splitting  in subdivision ID [%s]!" , nbMaxNormals , id ); ReportError( ErrorStr ); throw 387;
    }
    indTangentSplit1 = *indNewTangent;
    (*indNewTangent)++;
    indTangentsEnd[ level ] = *indNewTangent;
    tabVertices[indSplit1].indNewTangent  = indTangentSplit1;
    if( *indNewTangent >= nbMaxTangents ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_tangents [%d] for splitting  in subdivision ID [%s]!" , nbMaxTangents , id ); ReportError( ErrorStr ); throw 387;
    }
//     if( tabFaces[indInitialFace].indColor1 >= 0
// 	&& tabFaces[indInitialFace].indColor2 >= 0  ) {
//       // creates a new color that is the average of the
//       // colors of the 2 initial vertices
//       for( int indComp = 0 ; indComp < 4 ; indComp++ ) {
// 	tabColors[*indNewColor * 4 + indComp] =
// 	  0.5 * tabColors[tabFaces[indInitialFace].indColor1 * 4]
// 	  + 0.5  * tabColors[tabFaces[indInitialFace].indColor2 * 4];
//       }

//       indColorSplit1 = *indNewColor;
//       (*indNewColor)++;
//       indColorsEnd[ level ] = *indNewColor;
//       tabVertices[indSplit1].indNewColor  = indColorSplit1;
//       if( *indNewColor >= nbMaxColors ) {
// 	sprintf( ErrorStr , "Error: Insufficient parameter max_colors [%d] in subdivision ID [%s]!" , nbMaxColors , id ); ReportError( ErrorStr ); throw 387;
//       }
//     }
    if( tabFaces[indInitialFace].indTextureCoord1 >= 0
	&& tabFaces[indInitialFace].indTextureCoord2 >= 0  ) {
      // creates a new texture coordinate that is the average of the
      // texture coordinates of the 2 initial vertices
      tabTextureCoords[*indNewTextureCoord] =
	tabTextureCoords[tabFaces[indInitialFace].indTextureCoord1];
      tabTextureCoords[*indNewTextureCoord] *= 0.5;
      vc_TextureCoord tmpVal
	= tabTextureCoords[tabFaces[indInitialFace].indTextureCoord2];
      tmpVal *= 0.5;
      tabTextureCoords[*indNewTextureCoord] += tmpVal;

      indTextureCoordSplit1 = *indNewTextureCoord;
      (*indNewTextureCoord)++;
      indTextureCoordsEnd[ level ] = *indNewTextureCoord;
      tabVertices[indSplit1].indNewTextureCoord  = indTextureCoordSplit1;
      if( *indNewTextureCoord >= nbMaxTextureCoords ) {
	sprintf( ErrorStr , "Error: Insufficient parameter max_texturecoords [%d] in subdivision ID [%s]!" , nbMaxTextureCoords , id ); ReportError( ErrorStr ); throw 387;
      }
    }
  }
  else {
    indNormalSplit1 = tabVertices[indSplit1].indNewNormal;
    indTangentSplit1 = tabVertices[indSplit1].indNewTangent;
//     indColorSplit1 = tabVertices[indSplit1].indNewColor;
    indTextureCoordSplit1 = tabVertices[indSplit1].indNewTextureCoord;
  }

  int indSplit2 =
    findsSplitVertex( tabFaces[indInitialFace].indVertex1 ,
		      tabFaces[indInitialFace].indVertex3 , level );
  int indNormalSplit2 = -1;
  int indTangentSplit2 = -1;
//   int indColorSplit2 = -1;
  int indTextureCoordSplit2 = -1;
  if( indSplit2 < 0 ) {
    // creates a new vertex that is the middle point of the
    // 2 initial vertices
    tabVertices[*indNewVertex].middlePoint(
					   tabVertices[tabFaces[indInitialFace].indVertex1] ,
					   tabVertices[tabFaces[indInitialFace].indVertex3] );
    tabVertices[*indNewVertex].vertexType = SPLITTING_VERTEX;
    tabVertices[*indNewVertex].indSegmentInitialVertex
      = tabFaces[indInitialFace].indVertex1;
    tabVertices[*indNewVertex].indSegmentFinalVertex
      = tabFaces[indInitialFace].indVertex3;
    indSplit2 = *indNewVertex;
    (*indNewVertex)++;
    indVerticesEnd[ level ] = *indNewVertex;
    if( *indNewVertex >= nbMaxVertices ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_vertices [%d] in subdivision ID [%s]!" , nbMaxVertices , id ); ReportError( ErrorStr ); throw 388;
    }
    // creates a new normal that is the average of the
    // normals of the 2 initial vertices
    tabNormals[*indNewNormal].averageNormalizedVector(
		      tabNormals[tabFaces[indInitialFace].indNormal1] ,
		      tabNormals[tabFaces[indInitialFace].indNormal3] );
    tabTangents[*indNewNormal].averageNormalizedVector(
		      tabTangents[tabFaces[indInitialFace].indNormal1] ,
		      tabTangents[tabFaces[indInitialFace].indNormal3] );
    indNormalSplit2 = *indNewNormal;
    (*indNewNormal)++;
    indNormalsEnd[ level ] = *indNewNormal;
    tabVertices[indSplit2].indNewNormal  = indNormalSplit2;
    if( *indNewNormal >= nbMaxNormals ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_normals [%d] for splitting  in subdivision ID [%s]!" , nbMaxNormals , id ); ReportError( ErrorStr ); throw 387;
    }
    indTangentSplit2 = *indNewTangent;
    (*indNewTangent)++;
    indTangentsEnd[ level ] = *indNewTangent;
    tabVertices[indSplit2].indNewTangent  = indTangentSplit2;
    if( *indNewTangent >= nbMaxTangents ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_tangents [%d] for splitting  in subdivision ID [%s]!" , nbMaxTangents , id ); ReportError( ErrorStr ); throw 387;
    }
//     if( tabFaces[indInitialFace].indColor1 >= 0
// 	&& tabFaces[indInitialFace].indColor3 >= 0  ) {
//       // creates a new color that is the average of the
//       // colors of the 2 initial vertices
//       for( int indComp = 0 ; indComp < 4 ; indComp++ ) {
// 	tabColors[*indNewColor * 4 + indComp] =
// 	  0.5 * tabColors[tabFaces[indInitialFace].indColor1 * 4]
// 	  + 0.5  * tabColors[tabFaces[indInitialFace].indColor3 * 4];
//       }

//       indColorSplit2 = *indNewColor;
//       (*indNewColor)++;
//       indColorsEnd[ level ] = *indNewColor;
//       tabVertices[indSplit2].indNewColor  = indColorSplit2;
//       if( *indNewColor >= nbMaxColors ) {
// 	sprintf( ErrorStr , "Error: Insufficient parameter max_colors [%d] in subdivision ID [%s]!" , nbMaxColors , id ); ReportError( ErrorStr ); throw 387;
//       }
//     }
    if( tabFaces[indInitialFace].indTextureCoord1 >= 0
	&& tabFaces[indInitialFace].indTextureCoord3 >= 0  ) {
      // creates a new texture coordinate that is the average of the
      // texture coordinates of the 2 initial vertices
      tabTextureCoords[*indNewTextureCoord] =
	tabTextureCoords[tabFaces[indInitialFace].indTextureCoord1];
      tabTextureCoords[*indNewTextureCoord] *= 0.5;
      vc_TextureCoord tmpVal
	= tabTextureCoords[tabFaces[indInitialFace].indTextureCoord3];
      tmpVal *= 0.5;
      tabTextureCoords[*indNewTextureCoord] += tmpVal;

      indTextureCoordSplit2 = *indNewTextureCoord;
      (*indNewTextureCoord)++;
      indTextureCoordsEnd[ level ] = *indNewTextureCoord;
      tabVertices[indSplit2].indNewTextureCoord  = indTextureCoordSplit2;
      if( *indNewTextureCoord >= nbMaxTextureCoords ) {
	sprintf( ErrorStr , "Error: Insufficient parameter max_texturecoords [%d] in subdivision ID [%s]!" , nbMaxTextureCoords , id ); ReportError( ErrorStr ); throw 387;
      }
    }
  }
  else {
    indNormalSplit2 = tabVertices[indSplit2].indNewNormal;
    indTangentSplit2 = tabVertices[indSplit2].indNewTangent;
//     indColorSplit2 = tabVertices[indSplit2].indNewColor;
    indTextureCoordSplit2 = tabVertices[indSplit2].indNewTextureCoord;
  }

  int indSplit3 =
    findsSplitVertex( tabFaces[indInitialFace].indVertex2 ,
		      tabFaces[indInitialFace].indVertex3 , level );
  int indNormalSplit3 = -1;
  int indTangentSplit3 = -1;
//   int indColorSplit3 = -1;
  int indTextureCoordSplit3 = -1;
  if( indSplit3 < 0 ) {
    // creates a new vertex that is the middle point of the
    // 2 initial vertices
    tabVertices[*indNewVertex].middlePoint(
		   tabVertices[tabFaces[indInitialFace].indVertex2] ,
		   tabVertices[tabFaces[indInitialFace].indVertex3] );
    tabTangents[*indNewNormal].averageNormalizedVector(
		      tabTangents[tabFaces[indInitialFace].indNormal2] ,
		      tabTangents[tabFaces[indInitialFace].indNormal3] );
    tabVertices[*indNewVertex].vertexType = SPLITTING_VERTEX;
    tabVertices[*indNewVertex].indSegmentInitialVertex
      = tabFaces[indInitialFace].indVertex2;
    tabVertices[*indNewVertex].indSegmentFinalVertex
      = tabFaces[indInitialFace].indVertex3;
    indSplit3 = *indNewVertex;
    (*indNewVertex)++;
    indVerticesEnd[ level ] = *indNewVertex;
    tabVertices[indSplit3].indNewNormal  = indNormalSplit3;
    if( *indNewVertex >= nbMaxVertices ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_vertices [%d] in subdivision ID [%s]!" , nbMaxVertices , id ); ReportError( ErrorStr ); throw 389;
    }
    // creates a new normal that is the average of the
    // normals of the 2 initial vertices
    tabNormals[*indNewNormal].averageNormalizedVector(
		      tabNormals[tabFaces[indInitialFace].indNormal2] ,
		      tabNormals[tabFaces[indInitialFace].indNormal3] );
    tabTangents[*indNewTangent].averageNormalizedVector(
		      tabTangents[tabFaces[indInitialFace].indTangent2] ,
		      tabTangents[tabFaces[indInitialFace].indTangent3] );
    indNormalSplit3 = *indNewNormal;
    (*indNewNormal)++;
    indNormalsEnd[ level ] = *indNewNormal;
    tabVertices[indSplit3].indNewNormal  = indNormalSplit3;
    if( *indNewNormal >= nbMaxNormals ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_normals [%d] for splitting  in subdivision ID [%s]!" , nbMaxNormals , id ); ReportError( ErrorStr ); throw 387;
    }
    indTangentSplit3 = *indNewTangent;
    (*indNewTangent)++;
    indTangentsEnd[ level ] = *indNewTangent;
    tabVertices[indSplit3].indNewTangent  = indTangentSplit3;
    if( *indNewTangent >= nbMaxTangents ) {
      sprintf( ErrorStr , "Error: Insufficient parameter max_tangents [%d] for splitting  in subdivision ID [%s]!" , nbMaxTangents , id ); ReportError( ErrorStr ); throw 387;
    }
//     if( tabFaces[indInitialFace].indColor2 >= 0
// 	&& tabFaces[indInitialFace].indColor3 >= 0  ) {
//       // creates a new color that is the average of the
//       // colors of the 2 initial vertices
//       for( int indComp = 0 ; indComp < 4 ; indComp++ ) {
// 	tabColors[*indNewColor * 4 + indComp] =
// 	  0.5 * tabColors[tabFaces[indInitialFace].indColor2 * 4]
// 	  + 0.5  * tabColors[tabFaces[indInitialFace].indColor3 * 4];
//       }

//       indColorSplit3 = *indNewColor;
//       (*indNewColor)++;
//       indColorsEnd[ level ] = *indNewColor;
//       tabVertices[indSplit3].indNewColor  = indColorSplit3;
//       if( *indNewColor >= nbMaxColors ) {
// 	sprintf( ErrorStr , "Error: Insufficient parameter max_colors [%d] in subdivision ID [%s]!" , nbMaxColors , id ); ReportError( ErrorStr ); throw 387;
//       }
//     }
    if( tabFaces[indInitialFace].indTextureCoord2 >= 0
	&& tabFaces[indInitialFace].indTextureCoord3 >= 0  ) {
      // creates a new texture coordinate that is the average of the
      // texture coordinates of the 2 initial vertices
      tabTextureCoords[*indNewTextureCoord] =
	tabTextureCoords[tabFaces[indInitialFace].indTextureCoord2];
      tabTextureCoords[*indNewTextureCoord] *= 0.5;
      vc_TextureCoord tmpVal
	= tabTextureCoords[tabFaces[indInitialFace].indTextureCoord3];
      tmpVal *= 0.5;
      tabTextureCoords[*indNewTextureCoord] += tmpVal;

      indTextureCoordSplit3 = *indNewTextureCoord;
      (*indNewTextureCoord)++;
      indTextureCoordsEnd[ level ] = *indNewTextureCoord;
      tabVertices[indSplit3].indNewTextureCoord  = indTextureCoordSplit3;
      if( *indNewTextureCoord >= nbMaxTextureCoords ) {
	sprintf( ErrorStr , "Error: Insufficient parameter max_texturecoords [%d] in subdivision ID [%s]!" , nbMaxTextureCoords , id ); ReportError( ErrorStr ); throw 387;
      }
    }
  }
  else {
    indNormalSplit3 = tabVertices[indSplit3].indNewNormal;
    indTangentSplit3 = tabVertices[indSplit3].indNewTangent;
//     indColorSplit3 = tabVertices[indSplit3].indNewColor;
    indTextureCoordSplit3 = tabVertices[indSplit3].indNewTextureCoord;
  }

  tabFaces[*indNewFace].indVertex1 = indReplacement1;
  tabFaces[*indNewFace].indVertex2 = indSplit1;
  tabFaces[*indNewFace].indVertex3 = indSplit2;
  tabFaces[*indNewFace].indNormal1 = indNormalReplacement1;
  tabFaces[*indNewFace].indNormal2 = indNormalSplit1;
  tabFaces[*indNewFace].indNormal3 = indNormalSplit2;
  tabFaces[*indNewFace].indTangent1 = indTangentReplacement1;
  tabFaces[*indNewFace].indTangent2 = indTangentSplit1;
  tabFaces[*indNewFace].indTangent3 = indTangentSplit2;
//   tabFaces[*indNewFace].indColor1 = indColorReplacement1;
//   tabFaces[*indNewFace].indColor2 = indColorSplit1;
//   tabFaces[*indNewFace].indColor3 = indColorSplit2;
  tabFaces[*indNewFace].indTextureCoord1 = indTextureCoordReplacement1;
  tabFaces[*indNewFace].indTextureCoord2 = indTextureCoordSplit1;
  tabFaces[*indNewFace].indTextureCoord3 = indTextureCoordSplit2;
  (*indNewFace)++;
  indFacesEnd[ level ] = *indNewFace;
  if( *indNewFace >= nbMaxFaces ) {
    sprintf( ErrorStr , "Error: Insufficient parameter max_faces [%d] in subdivision ID [%s]!" , nbMaxFaces , id ); ReportError( ErrorStr ); throw 390;
  }

  tabFaces[*indNewFace].indVertex1 = indReplacement2;
  tabFaces[*indNewFace].indVertex2 = indSplit3;
  tabFaces[*indNewFace].indVertex3 = indSplit1;
  tabFaces[*indNewFace].indNormal1 = indNormalReplacement2;
  tabFaces[*indNewFace].indNormal2 = indNormalSplit3;
  tabFaces[*indNewFace].indNormal3 = indNormalSplit1;
  tabFaces[*indNewFace].indTangent1 = indTangentReplacement2;
  tabFaces[*indNewFace].indTangent2 = indTangentSplit3;
  tabFaces[*indNewFace].indTangent3 = indTangentSplit1;
//   tabFaces[*indNewFace].indColor1 = indColorReplacement2;
//   tabFaces[*indNewFace].indColor2 = indColorSplit3;
//   tabFaces[*indNewFace].indColor3 = indColorSplit1;
  tabFaces[*indNewFace].indTextureCoord1 = indTextureCoordReplacement2;
  tabFaces[*indNewFace].indTextureCoord2 = indTextureCoordSplit3;
  tabFaces[*indNewFace].indTextureCoord3 = indTextureCoordSplit1;
  (*indNewFace)++;
  indFacesEnd[ level ] = *indNewFace;
  if( *indNewFace >= nbMaxFaces ) {
    sprintf( ErrorStr , "Error: Insufficient parameter max_faces [%d] in subdivision ID [%s]!" , nbMaxFaces , id ); ReportError( ErrorStr ); throw 391;
  }

  tabFaces[*indNewFace].indVertex1 = indReplacement3;
  tabFaces[*indNewFace].indVertex2 = indSplit2;
  tabFaces[*indNewFace].indVertex3 = indSplit3;
  tabFaces[*indNewFace].indNormal1 = indNormalReplacement3;
  tabFaces[*indNewFace].indNormal2 = indNormalSplit2;
  tabFaces[*indNewFace].indNormal3 = indNormalSplit3;
  tabFaces[*indNewFace].indTangent1 = indTangentReplacement3;
  tabFaces[*indNewFace].indTangent2 = indTangentSplit2;
  tabFaces[*indNewFace].indTangent3 = indTangentSplit3;
//   tabFaces[*indNewFace].indColor1 = indColorReplacement3;
//   tabFaces[*indNewFace].indColor2 = indColorSplit2;
//   tabFaces[*indNewFace].indColor3 = indColorSplit3;
  tabFaces[*indNewFace].indTextureCoord1 = indTextureCoordReplacement3;
  tabFaces[*indNewFace].indTextureCoord2 = indTextureCoordSplit2;
  tabFaces[*indNewFace].indTextureCoord3 = indTextureCoordSplit3;
  (*indNewFace)++;
  indFacesEnd[ level ] = *indNewFace;
  if( *indNewFace >= nbMaxFaces ) {
    sprintf( ErrorStr , "Error: Insufficient parameter max_faces [%d] in subdivision ID [%s]!" , nbMaxFaces , id ); ReportError( ErrorStr ); throw 392;
  }

  tabFaces[*indNewFace].indVertex1 = indSplit1;
  tabFaces[*indNewFace].indVertex2 = indSplit3;
  tabFaces[*indNewFace].indVertex3 = indSplit2;
  tabFaces[*indNewFace].indNormal1 = indNormalSplit1;
  tabFaces[*indNewFace].indNormal2 = indNormalSplit3;
  tabFaces[*indNewFace].indNormal3 = indNormalSplit2;
  tabFaces[*indNewFace].indTangent1 = indTangentSplit1;
  tabFaces[*indNewFace].indTangent2 = indTangentSplit3;
  tabFaces[*indNewFace].indTangent3 = indTangentSplit2;
//   tabFaces[*indNewFace].indColor1 = indColorSplit1;
//   tabFaces[*indNewFace].indColor2 = indColorSplit3;
//   tabFaces[*indNewFace].indColor3 = indColorSplit2;
  tabFaces[*indNewFace].indTextureCoord1 = indTextureCoordSplit1;
  tabFaces[*indNewFace].indTextureCoord2 = indTextureCoordSplit3;
  tabFaces[*indNewFace].indTextureCoord3 = indTextureCoordSplit2;
  (*indNewFace)++;
  indFacesEnd[ level ] = *indNewFace;
  if( *indNewFace >= nbMaxFaces ) {
    sprintf( ErrorStr , "Error: Insufficient parameter max_faces [%d] in subdivision ID [%s]!" , nbMaxFaces , id ); ReportError( ErrorStr ); throw 393;
  }
}

void vc_Subdivision::modifiesAVertexLoopAlgo(  int indVertex , int level ,
					       int * tabConnectedFaces ,
					       int * tabConnectedFacesCopy ,
					       int * tabIndConnectedVerticesIni ,
					       int * tabIndConnectedVerticesCopy ) {
  if( tabVertices[ indVertex ].modified ) {
    return;
  }

  if( tabVertices[ indVertex ].vertexType == SPLITTING_VERTEX ) {
    int indIniSegment = tabVertices[ indVertex ].indSegmentInitialVertex;
    int indEndSegment = tabVertices[ indVertex ].indSegmentFinalVertex;

    int nbConnectedVerticesIni = 0;
    int nbConnectedFaces = 0;
    collect_connected_vertices( indIniSegment ,
				tabIndConnectedVerticesIni ,
				tabIndConnectedVerticesCopy ,
				&nbConnectedVerticesIni ,
				tabConnectedFaces ,
				tabConnectedFacesCopy ,
				&nbConnectedFaces ,
				max_degree , level - 1 );

    // the extremities of the segments reciprocally
    // belong to the other extremity's connected vertices
    int indexEnd = -1;
    int nextIndexEnd = -1;
    int precedingIndexEnd = -1;
    //     printf( "Connectivity: " );
    for (int c = 0 ; c < nbConnectedVerticesIni ; c++) {
      //       printf( "%d " , tabIndConnectedVerticesIni[c] );
      if( tabIndConnectedVerticesIni[c] == indEndSegment ) {
	indexEnd = c;
	break;
      }
    }
    //     printf( "\n" );
    if( indexEnd == -1 ) {
      printf( "indexEnd (1) %d/%d : %d-%d \n" ,
	      indexEnd , nbConnectedVerticesIni ,
	      indIniSegment ,
	      tabIndConnectedVerticesIni[indexEnd]  );
      sprintf( ErrorStr , "Error: Segment extremity not found in connectivity in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 401;
    }

    // computes the index of the preceding vertex
    precedingIndexEnd
      = tabIndConnectedVerticesIni[
				   (nbConnectedVerticesIni + indexEnd - 1) % nbConnectedVerticesIni ];
    // computes the index of the next vertex
    nextIndexEnd
      = tabIndConnectedVerticesIni[
				   (indexEnd + 1) % nbConnectedVerticesIni ];

    vc_Vertex v;
    //       printf( "Splitted vertex %d-%d (%d) : %d / %d \n" ,
    // 	      indIniSegment , indEndSegment , indVertex ,
    // 	      precedingIndexEnd ,
    // 	      nextIndexEnd );

    tabVertices[indVertex] = tabVertices[indIniSegment];
    tabVertices[indVertex] *= coefLoop1;
    v = tabVertices[indEndSegment];
    v *= coefLoop1;
    tabVertices[indVertex] += v;
    v = tabVertices[precedingIndexEnd];
    v *= coefLoop2;
    tabVertices[indVertex] += v;
    v = tabVertices[nextIndexEnd];
    v *= coefLoop2;
    tabVertices[indVertex] += v;
  }
  else if( tabVertices[ indVertex ].vertexType == REPLACEMENT_VERTEX ) {
    int indIniVertex = tabVertices[ indVertex ].indReplacedVertex;

    int nbConnectedVertices = 0;
    int nbConnectedFaces = 0;
    collect_connected_vertices( indIniVertex ,
				tabIndConnectedVerticesIni ,
				tabIndConnectedVerticesCopy ,
				&nbConnectedVertices ,
				tabConnectedFaces ,
				tabConnectedFacesCopy ,
				&nbConnectedFaces ,
				max_degree , level - 1 );

    if( nbConnectedVertices <= 0 ) {
      sprintf( ErrorStr , "Error: Replaced vertex has no connected vertices in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 404;
    }

    float coef = (3.0F + 2.0F * (float)cos( 2 * PI / (float)nbConnectedVertices));
    float beta = (1.0F / (float)nbConnectedVertices)
      * (0.625F - ((coef * coef)/64.0F));

    //       printf( "Replaced vertex %d (%d) : " , indIniVertex , indVertex );

    tabVertices[indVertex] = tabVertices[indIniVertex];
    tabVertices[indVertex] *= (1.0 - (0.625 - ((coef * coef)/64.0)));

    // for all the connected vertices
    vc_Vertex v;
    for (int c = 0 ; c < nbConnectedVertices ; c++) {
      // 	printf( "/ %d " , tabIndConnectedVerticesIni[c] );
      v = tabVertices[tabIndConnectedVerticesIni[c]];
      v *= beta;
      tabVertices[indVertex] += v;
    }

    //       printf( "\n" );
  }
  else {
    sprintf( ErrorStr , "Error: Unexpected parameter type in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 402;
  }

  //   StoresMaxima( &tabVertices[indVertex] );
  //   NormalizesColor( &tabVertices[indVertex] );
  tabVertices[ indVertex ].modified = true;
}

void vc_Subdivision::ComputesModifiedButterfly( vc_Vertex *modifiedVertex ,
						int nbVerticesIrregular ,
						int indRegularSegmentExtremity ,
						int indIrregularSegmentExtremity ,
						int indexRegularVertex ,
						int tabIndConnectedVerticesIrreg[] ) {
  vc_Vertex v;
  float weights[5]={0,0,0,0,0};

  if( nbVerticesIrregular == 3 ) {
    weights[0] = 5.0F/12.0F;
    weights[1] = -(1.0F/12.0F);
    weights[2] = -(1.0F/12.0F);
  }
  else if( nbVerticesIrregular == 4 ) {
    weights[0] = 3.0F/8.0F;
    weights[1] = 0.0F;
    weights[2] = -(1.0F/8.0F);
    weights[3] = 0.0F;
  }
  else if( nbVerticesIrregular == 5 ) {
    for( int ind = 0 ; ind < 5 ; ind++ ) {
      weights[ind] = ( .25F + (float)cos( 2.0 * PI * ind / 5.0)
		       + 0.5F * (float)cos( 4.0 * PI * ind / 5.0) ) / 5.0F;
    }
  }

  *modifiedVertex = tabVertices[indRegularSegmentExtremity];
  *modifiedVertex *= weights[0];
  v = tabVertices[indIrregularSegmentExtremity];
  v *= 0.75;
  *modifiedVertex += v;

  for( int ind = 1 ; ind < nbVerticesIrregular ; ind++ ) {
    v = tabVertices[tabIndConnectedVerticesIrreg[
						 (indexRegularVertex + ind) % nbVerticesIrregular ]];
    v *= weights[ind];
    *modifiedVertex += v;
  }
}

void vc_Subdivision::modifiesAVertexModifiedButterflyAlgo(
		  int indVertex ,
		  int level ,
		  int * tabIndConnectedVerticesIni ,
		  int * tabIndConnectedVerticesEnd ,
		  int * tabIndConnectedVerticesCopy ,
		  int * tabConnectedFaces ,
		  int * tabConnectedFacesCopy ) {

  if( tabVertices[ indVertex ].modified ) {
    return;
  }

  if( tabVertices[ indVertex ].vertexType == SPLITTING_VERTEX ) {
    int indIniSegment = tabVertices[ indVertex ].indSegmentInitialVertex;
    int indEndSegment = tabVertices[ indVertex ].indSegmentFinalVertex;

    int nbConnectedVerticesIni = 0;
    int nbConnectedVerticesEnd = 0;
    int nbConnectedFacesIni = 0;
    int nbConnectedFacesEnd = 0;
    collect_connected_vertices( indIniSegment ,
				tabIndConnectedVerticesIni ,
				tabIndConnectedVerticesCopy ,
				&nbConnectedVerticesIni ,
				tabConnectedFaces ,
				tabConnectedFacesCopy ,
				&nbConnectedFacesIni ,
				max_degree , level - 1 );

    collect_connected_vertices( indEndSegment ,
				tabIndConnectedVerticesEnd ,
				tabIndConnectedVerticesCopy ,
				&nbConnectedVerticesEnd ,
				tabConnectedFaces ,
				tabConnectedFacesCopy ,
				&nbConnectedFacesEnd ,
				max_degree , level - 1 );

    // the extremities of the segments reciprocally
    // belong to the other extremity's connected vertices
    int indexEnd = -1;
    for (int c = 0 ; c < nbConnectedVerticesIni ; c++) {
      if( tabIndConnectedVerticesIni[c] == indEndSegment ) {
	indexEnd = c;
	break;
      }
    }
    if( indexEnd == -1 ) {
      printf( "indexEnd (2) %d-%d, %d connected: " ,
	      indIniSegment ,
	      indEndSegment , nbConnectedVerticesIni );
      for (int c = 0 ; c < nbConnectedVerticesIni ; c++) {
	printf( "%d " , tabIndConnectedVerticesIni[c] );
      }
      printf( "\n" );
      sprintf( ErrorStr , "Error: Segment extremity not found in connectivity in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 405;
    }

    int indexIni = -1;
    for (int c = 0 ; c < nbConnectedVerticesEnd ; c++) {
      if( tabIndConnectedVerticesEnd[c] == indIniSegment ) {
	indexIni = c;
	break;
      }
    }
    if( indexIni == -1 ) {
      printf( "indexIni (3) %d-%d, %d connected: " ,
	      indIniSegment ,
	      indEndSegment  , nbConnectedVerticesEnd );
      for (int c = 0 ; c < nbConnectedVerticesEnd ; c++) {
	printf( "%d " , tabIndConnectedVerticesEnd[c] );
      }
      printf( "\n" );
      sprintf( ErrorStr , "Error: Segment extremity not found in connectivity in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 406;
    }

    vc_Vertex v;
    //       printf( "Splitted vertex %d-%d (%d) \n" ,
    // 	      indIniSegment , indIniSegment , indVertex );

    if( nbConnectedVerticesIni == 6
	&& nbConnectedVerticesEnd == 6 ) {

      tabVertices[indVertex] = tabVertices[indIniSegment];
      tabVertices[indVertex] *= coefMB1;
      v = tabVertices[indEndSegment];
      v *= coefMB1;
      tabVertices[indVertex] += v;

      v = tabVertices[tabIndConnectedVerticesIni[
						 (nbConnectedVerticesIni + indexEnd - 1) % nbConnectedVerticesIni ]];
      v *= coefMB2;
      tabVertices[indVertex] += v;
      v = tabVertices[tabIndConnectedVerticesIni[
						 (indexEnd + 1) % nbConnectedVerticesIni ]];
      v *= coefMB2;
      tabVertices[indVertex] += v;

      v = tabVertices[tabIndConnectedVerticesIni[
						 (nbConnectedVerticesIni + indexEnd - 2) % nbConnectedVerticesIni ]];
      v *= coefMB3;
      tabVertices[indVertex] += v;
      v = tabVertices[tabIndConnectedVerticesIni[
						 (indexEnd + 2) % nbConnectedVerticesIni ]];
      v *= coefMB3;
      tabVertices[indVertex] += v;

      v = tabVertices[tabIndConnectedVerticesEnd[
						 (nbConnectedVerticesEnd + indexIni - 2) % nbConnectedVerticesEnd ]];
      v *= coefMB3;
      tabVertices[indVertex] += v;
      v = tabVertices[tabIndConnectedVerticesEnd[
						 (indexIni + 2) % nbConnectedVerticesEnd ]];
      v *= coefMB3;
      tabVertices[indVertex] += v;
    }
    else if( nbConnectedVerticesIni == 6
	     && nbConnectedVerticesEnd < 6) {
      ComputesModifiedButterfly( &tabVertices[indVertex] ,
				 nbConnectedVerticesEnd ,
				 indIniSegment ,
				 indEndSegment ,
				 indexIni ,
				 tabIndConnectedVerticesEnd );
    }
    else if( nbConnectedVerticesIni < 6
	     && nbConnectedVerticesEnd == 6) {
      ComputesModifiedButterfly( &tabVertices[indVertex] ,
				 nbConnectedVerticesIni ,
				 indEndSegment ,
				 indIniSegment ,
				 indexEnd ,
				 tabIndConnectedVerticesIni );
    }
    else if( nbConnectedVerticesIni < 6
	     && nbConnectedVerticesEnd < 6) {
      vc_Vertex v1, v2;
      ComputesModifiedButterfly( &v1 ,
				 nbConnectedVerticesEnd ,
				 indIniSegment ,
				 indEndSegment ,
				 indexIni ,
				 tabIndConnectedVerticesEnd );
      ComputesModifiedButterfly( &v2 ,
				 nbConnectedVerticesIni ,
				 indEndSegment ,
				 indIniSegment ,
				 indexEnd ,
				 tabIndConnectedVerticesIni );
      tabVertices[indVertex].middlePoint( v1 , v2 );
    }
  }
  else if( tabVertices[ indVertex ].vertexType == REPLACEMENT_VERTEX ) {
  }
  else {
    sprintf( ErrorStr , "Error: Unexpected parameter type in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 407;
  }

  //   StoresMaxima( &tabVertices[indVertex] );
  //   NormalizesColor( &tabVertices[indVertex] );
  tabVertices[ indVertex ].modified = true;
}

void vc_Subdivision::subdividesAndModifies( int level ,
					    int * tabConnectedFaces ,
					    int * tabConnectedFacesCopy ,
					    int * tabIndConnectedVerticesIni ,
					    int * tabIndConnectedVerticesEnd ,
					    int * tabIndConnectedVerticesCopy ) {
  int indNewFace = indFacesEnd[ level - 1 ];
  int indNewTextureCoord = indTextureCoordsEnd[ level - 1 ];
  int indNewNormal = indNormalsEnd[ level - 1 ];
  int indNewTangent = indTangentsEnd[ level - 1 ];
//   int indNewColor = indColorsEnd[ level - 1 ];
  int indNewVertex = indVerticesEnd[ level - 1 ];

  indFacesIni[ level ] = indNewFace;
  indFacesEnd[ level ] = indNewFace;
  indTextureCoordsIni[ level ] = indNewTextureCoord;
  indTextureCoordsEnd[ level ] = indNewTextureCoord;
  indNormalsIni[ level ] = indNewNormal;
  indNormalsEnd[ level ] = indNewNormal;
  indTangentsIni[ level ] = indNewTangent;
  indTangentsEnd[ level ] = indNewTangent;
//   indColorsIni[ level ] = indNewColor;
//   indColorsEnd[ level ] = indNewColor;
  indVerticesIni[ level ] = indNewVertex;
  indVerticesEnd[ level ] = indNewVertex;

  for (int i = indFacesIni[ level - 1 ] ; i < indFacesEnd[ level - 1 ] ; i++) {
    subdividesAFace(  i , &indNewFace , &indNewVertex ,
		      &indNewNormal  , 
		      &indNewTangent  , 
// 		      &indNewColor , 
		      &indNewTextureCoord ,
		      level );
  }
  if( subdivisionAlgorithm == LOOP ) {
    for (int i = indVerticesIni[ level ] ; i < indVerticesEnd[ level ] ; i++) {
      modifiesAVertexLoopAlgo(  i , level ,
				tabConnectedFaces ,
				tabConnectedFacesCopy ,
				tabIndConnectedVerticesIni ,
				tabIndConnectedVerticesCopy );
    }
  }
  else if( subdivisionAlgorithm == MODIFIED_BUTTERFLY ) {
    for (int i = indVerticesIni[ level ] ; i < indVerticesEnd[ level ] ; i++) {
      modifiesAVertexModifiedButterflyAlgo(  i , level ,
					     tabIndConnectedVerticesIni ,
					     tabIndConnectedVerticesEnd ,
					     tabIndConnectedVerticesCopy ,
					     tabConnectedFaces ,
					     tabConnectedFacesCopy );
    }
  }
  else {
    sprintf( ErrorStr , "Error: unknown subdivision algorithm!" ); ReportError( ErrorStr ); throw 385;
  }

  indFacesEnd[ level ] = indNewFace;
  indTextureCoordsEnd[ level ] = indNewTextureCoord;
  indNormalsEnd[ level ] = indNewNormal;
  indTangentsEnd[ level ] = indNewTangent;
//   indColorsEnd[ level ] = indNewColor;
  indVerticesEnd[ level ] = indNewVertex;
}

bool vc_Subdivision::updateKeypoints( void ) {
  // memorizes preceding transformation in case sthing changes
  bool updated = false;
  for( int ind = 0 ; ind < keypointSize ; ind++ ) {
    vc_Transformation * curTransf 
      = ((vc_Transformation *)(keypointTransfs[ind]->sceneObject));
    if( keypointPrecedingTransfs[ind]->aVector.x != curTransf->aVector.x
	|| keypointPrecedingTransfs[ind]->aVector.y != curTransf->aVector.y
	|| keypointPrecedingTransfs[ind]->aVector.z != curTransf->aVector.z ) {
      keypointPrecedingTransfs[ind]->aVector.x = curTransf->aVector.x;
      keypointPrecedingTransfs[ind]->aVector.y = curTransf->aVector.y;
      keypointPrecedingTransfs[ind]->aVector.z = curTransf->aVector.z;
      updated = true; 
    }
  }

  // the current animation is different from the preceding one
  if( updated ) {
    // for each weighted vertex: modify the coordinates as a weighted
    // sum of translation transformations
    for( int indVertex = 0 ; indVertex < indVerticesEnd[ 0 ] ; indVertex++ ) {
      if( tabVertices[indVertex].keypointWeightingSize > 0 ) {
	// remember the vertex
	vc_Vertex * vert = &(tabVertices[indVertex]);
	// copy initial coordinates from x_ini, y_ini, z_ini
	vert->x = vert->x_ini;
	vert->y = vert->y_ini;
	vert->z = vert->z_ini;

	// apply a weighted sum of translations
	// 	  vc_Vertex sum;//SM 16/12/2005
	// 	  sum.x = 0;
	// 	  sum.y = 0;
	// 	  sum.z = 0;
	// weight normalization is performed in ParseFDPWeights
	// 	  vc_Vertex sumweights;
	// 	  sumweights.x = 0;
	// 	  sumweights.y = 0;
	// 	  sumweights.z = 0;
	for( int indWeight = 0 ;
	     indWeight < vert->keypointWeightingSize ;
	     indWeight++ ) {
	  vc_KeypointWeighting * kpw = &(vert->tabKeypointWeighting[indWeight]);
	  vc_Transformation * tr
	    = ((vc_Transformation *)(keypointTransfs[kpw->indKeypoint]->sceneObject));
	  // SM 16/12/2005
	  // 	    sum.x += tr->aVector.x * kpw->weights[0];
	  // 	    sum.y += tr->aVector.y * kpw->weights[1];
	  // 	    sum.z += tr->aVector.z * kpw->weights[2];
	  // 	    sumweights.x += kpw->weights[0];
	  // 	    sumweights.y += kpw->weights[1];
	  // 	    sumweights.z += kpw->weights[2];
	  vert->x += tr->aVector.x * kpw->weights[0];
	  vert->y += tr->aVector.y * kpw->weights[1];
	  vert->z += tr->aVector.z * kpw->weights[2];
						
	  // 	   	    printf( "vector %.2f %.2f %.2f / weights %.2f %.2f %.2f \n" ,
	  // 	   		    tr->aVector.x , tr->aVector.y , tr->aVector.z ,
	  // 	   		    kpw->weights[0] , kpw->weights[1] , kpw->weights[2] );
	}
	// SM 16/12/2005
	// 	  if( sumweights.x != 0 ) vert->x += sum.x / sumweights.x;
	// 	  if( sumweights.y != 0 ) vert->y += sum.y / sumweights.y;
	// 	  if( sumweights.z != 0 ) vert->z += sum.z / sumweights.z;
	// 	if( indVertex == 0 ) {
	// 	  printf( "vert %d before %.2f %.2f %.2f / after %.2f %.2f %.2f \n" ,
	// 		  indVertex , vert->x_ini, vert->y_ini, vert->z_ini, vert->x, vert->y, vert->z );
	// 	}
      }
    }
  }
  return updated;
}

// GPU viseme based update
// viseme based update: a list of tables each contains one viseme
// and the interpolation is made between the current viseme and the
// next one
// this function sets the correct values for the shader parameters
// KPtranslationsIniParam and KPtranslationsEndParam to the current
// viseme and next viseme
// a cyclic update is made in the XML script that sends the 
// - alpha value at each time step
// - textureAnimationParameters  is a boolean parameters that tells
// whether animation is controlled by vertex shader parameters 
// or by a texture if the size limit is reached
void vc_Subdivision::updateKeypoints( int indVisemeSource , 
				      int indVisemeDestination , 
				      char * visemeTableId ,
				      vc_SceneNode *tableNode , 
				      int indChannel ,
				      vc_Shader *subdivisionShader ) {
  // in case of GPU rendering, collects details about the KP texture
  // and updates the texture from the KP transformations
#ifdef VC_HAVE_CG
  vc_VertexShader *vertShader = NULL;
  
  // translation table for KP transaltion copy before passing
  // it as a parameter to the vertex shader (in case of 
  // animation by passing the KP translations through vertex param table)
  float * tabKPtranslations = NULL;

  // gets a pointer to the vertex shader
  if( subdivisionShader ) {
    vertShader = subdivisionShader->vertexShader[ CurrentIndWindow ];
    //printf( " vertShader %d\n" , vertShader );
  }

  // subdivision has a shader parameter with all the necessary parameters
  if( vertShader
      && ((vertShader->KPtranslationsIniParam_1
	   && vertShader->KPtranslationsEndParam_1)
	  || vertShader->KPtextureTranslationParam )
      && gpu_KPanim 
      && subdivisionShader
      && subdivisionShader->shaderType == GENERALIZED_SHADER  ) {
    // int width = VC_CompositionNode->pImageRaw[ textureNo ].sizeX;
    // int height = VC_CompositionNode->pImageRaw[ textureNo ].sizeY;
    if( tableNode ) {
      char    tableId[XMLAttrValLength];
      int     currentKeypointSize = keypointSize;
      // TEMPORARY LIMITATION FOR PARAMETER SIZE
      // CHECK IF NIVIDIA 80XX ACCEPTS LARGER PARAMETER DIMENSIONS
      if( indChannel == 1 ) {
	// printf( "Subdivision %s channel 1 %d KPs\n" , id , keypointSize_1 );
	currentKeypointSize = keypointSize_1;
      }
      else if( indChannel == 2 ) {
	// printf( "Subdivision %s channel 2 %d KPs\n" , id , keypointSize_2 );
	currentKeypointSize = keypointSize_2;
      }

      // initial transformation
      sprintf( tableId , "%s%d" , visemeTableId , indVisemeSource );
      vc_Table *table = tableNode->getTableFromName( tableId );
      
      // printf( "Table %s %s channel %d\n" , tableNode->id , tableId , indChannel );
      // fprintf( fileLog , "Table %s %s channel %d\n" , tableNode->id , tableId , indChannel );

      // table has been found and contains source transformations
      if( table && table->GetSize() >= keypointSize * 3 ) {

	// animation by passing the KP translations through vertex texture 
	if( vertShader->KPtextureTranslationParam ) {
	  // canal 1 KP translations are stored between 0 && keypointSize - 1
	  // canal 2 KP translations are stored between keypointSize - 1
	  //                                            && 2 * keypointSize - 1
	  float * dataFloatVertexAnimationTexture 
	    = VC_CompositionNode->
	              pImageRaw[ vertShader->KPtextureNo ].dataFloat;
	  for( int indKP = (indChannel - 1) * keypointSize ; 
	       indKP < keypointSize ; 
	       indKP++ ) {
	    dataFloatVertexAnimationTexture[ indKP * 4 ] 
	      = table->GetNumericalValue( indKP * 3 );
	    dataFloatVertexAnimationTexture[ indKP * 4 + 1 ] 
	      = table->GetNumericalValue( indKP * 3 + 1 );
	    dataFloatVertexAnimationTexture[ indKP * 4 + 2 ] 
	      = table->GetNumericalValue( indKP * 3 + 2 );
	    dataFloatVertexAnimationTexture[ indKP * 4 + 3 ] = 1.0;
	  }      
 
	  // Transfer to GPU + possible mipmaps
	  VC_CompositionNode->LoadTextureInGPUMemory( 
						     vertShader->KPtextureNo , 
						     1 , 
						     1 , 
						     RAW ,
						     VC_BITMAP_TEXTURE ,
						     false , 
						     false , 
						     float_tex_format ,
						     linear_filter ,
						     linear_filter ,
						     VC_VERTEX_TEXTURE , 
						     GL_TEXTURE_2D ,
						     0 ,
						     -1 );
	}

	// animation by passing the KP translations through vertex table param 
	if( vertShader->KPtranslationsIniParam_1 ) {
	  tabKPtranslations = new float[ currentKeypointSize * 3 ]; 
	  // If we can't allocate memory, quit!
	  if(tabKPtranslations == NULL) {		  
	    sprintf( ErrorStr , "Error: Translation table allocation error!" ); ReportError( ErrorStr ); throw 385;
	  }
	  
	  if( indChannel == 1 ) {
	    int KPsize = 0;
	    memset( tabKPtranslations , 0 , currentKeypointSize * 3 * sizeof (float) );
	    if( indirectionTableKeypointIndices_1 ) {
	      // printf( "\nchannel1 - source: " );
	      for( int indKP = 0 ; indKP < keypointSize ; indKP++ ) {
		int ind = indirectionTableKeypointIndices_1[indKP];
		if( ind >= 0 ) {
		  for( int i = 0 ; i < 3 ; i++ ) {
		    tabKPtranslations[ 3 * ind + i ] 
		      = table->GetNumericalValue( 3 * indKP + i );
		    // if( indKP >= 22 && indKP <= 27 ) {
		      // printf( "(%d/%d/%s) %.2f " , indKP , ind  , keypointTransfs[indKP]->id , tabKPtranslations[ 3 * ind + i ] );
		    // }
		  }
		  KPsize++;
		}
	      }
	      // printf( "\n\n" );
	      if( KPsize != currentKeypointSize ) {
		sprintf( ErrorStr , "Error: Incorrect keypoint transformation size on channel 1 (%d) mask size is (%d)!" , KPsize , currentKeypointSize ); ReportError( ErrorStr ); 
	      }
	    }
	    else {
	      for( int indKP = 0 ; indKP < currentKeypointSize * 3 ; indKP++ ) {
		tabKPtranslations[ indKP ] 
		  = table->GetNumericalValue( indKP );
		// fprintf( fileLog , "%.2f " , tabKPtranslations[ indKP ] );
	      }
	      // fprintf( fileLog , "\n" );
	    }

	    // fprintf( fileLog , "Shader ini param assignation (ch 1)\n" );
	    cgGLSetParameterArray3f( vertShader->KPtranslationsIniParam_1 ,
				     0 , currentKeypointSize , 
				     tabKPtranslations );
	  }
	  else if( indChannel == 2 && vertShader->KPtranslationsIniParam_2 ) {
	    int KPsize = 0;
	    memset( tabKPtranslations , 0 , currentKeypointSize * 3 * sizeof (float) );
	    if( indirectionTableKeypointIndices_2 ) {
	      // printf( "\nchannel2 - source: " );
	      for( int indKP = 0 ; indKP < keypointSize ; indKP++ ) {
		int ind = indirectionTableKeypointIndices_2[indKP];
		if( ind >= 0 ) {
		  for( int i = 0 ; i < 3 ; i++ ) {
		    tabKPtranslations[ 3 * ind + i ] 
		      = table->GetNumericalValue( 3 * indKP + i );
		    // if( indKP >= 22 && indKP <= 27 ) {
		      // printf( "(%d/%d/%s) %.2f " , indKP , ind  , keypointTransfs[indKP]->id , tabKPtranslations[ 3 * ind + i ] );
		    // }
		  }
		  KPsize++;
		}
	      }
	      // printf( "\n\n" );
	      if( KPsize != currentKeypointSize ) {
		sprintf( ErrorStr , "Error: Incorrect initial keypoint transformation size on channel 2 (%d) mask size is (%d)!" , KPsize , currentKeypointSize ); ReportError( ErrorStr ); 
	      }
	    }
	    else {
	      for( int indKP = 0 ; indKP < keypointSize * 3 ; indKP++ ) {
		tabKPtranslations[ indKP ] 
		  = table->GetNumericalValue( indKP );
		// fprintf( fileLog , "%.2f " , tabKPtranslations[ indKP ] );
	      }
	      // fprintf( fileLog , "\n" );
	    }

	    // fprintf( fileLog , "Shader ini param assignation (ch 2)\n" );
	    cgGLSetParameterArray3f( vertShader->KPtranslationsIniParam_2 ,
				     0 , currentKeypointSize , 
				     tabKPtranslations );
	  }
	}
      }
      else {
	sprintf( ErrorStr , "Error: Expected table [%s] on node [%s] of size >= %d!" , tableId , tableNode->id , keypointSize * 3  ); ReportError( ErrorStr ); 
      }
      
      // final transformation
      sprintf( tableId , "%s%d" , visemeTableId , indVisemeDestination );
      table = tableNode->getTableFromName( tableId );

      // printf( "Table %s %s channel %d\n" , tableNode->id , tableId , indChannel );
      // fprintf( fileLog , "Table %s %s channel %d\n" , tableNode->id , tableId , indChannel );

      // table has been found and contains target transformations      
      if( table && table->GetSize() >= keypointSize * 3 ) {

	// animation by passing the KP translations through vertex texture 
	if( vertShader->KPtextureTranslationParam ) {
	  // canal 1 KP translations are stored between 2 * keypointSize - 1
	  //                                            && 2 * keypointSize - 1
	  // canal 2 KP translations are stored between 3 * keypointSize - 1
	  //                                            && 3 * keypointSize - 1
	  float * dataFloatVertexAnimationTexture 
	    = VC_CompositionNode->
	    pImageRaw[ vertShader->KPtextureNo ].dataFloat;
	  for( int indKP = (2 + indChannel - 1) * keypointSize ; 
	       indKP < keypointSize ; 
	       indKP++ ) {
	    dataFloatVertexAnimationTexture[ indKP * 4 ] 
	      = table->GetNumericalValue( indKP * 3 );
	    dataFloatVertexAnimationTexture[ indKP * 4 + 1 ] 
	      = table->GetNumericalValue( indKP * 3 + 1 );
	    dataFloatVertexAnimationTexture[ indKP * 4 + 2 ] 
	      = table->GetNumericalValue( indKP * 3 + 2 );
	    dataFloatVertexAnimationTexture[ indKP * 4 + 3 ] = 1.0;
	  }      
 
	  // Transfer to GPU + possible mipmaps
	  VC_CompositionNode->LoadTextureInGPUMemory( 
						     vertShader->KPtextureNo , 
						     1 ,
						     1 , 
						     RAW ,
						     VC_BITMAP_TEXTURE ,
						     false , 
						     false , 
						     float_tex_format ,
						     linear_filter ,
						     linear_filter ,
						     VC_VERTEX_TEXTURE , 
						     GL_TEXTURE_2D ,
						     0 ,
						      -1 );
	}

	// animation by passing the KP translations through vertex table param 
	if( vertShader->KPtranslationsEndParam_1 ) {
	  if( indChannel == 1 ) {
	    int KPsize = 0;
	    if( indirectionTableKeypointIndices_1 ) {
	      // printf( "\nchannel1 - target: " );
	      for( int indKP = 0 ; indKP < keypointSize ; indKP++ ) {
		int ind = indirectionTableKeypointIndices_1[indKP];
		if( ind >= 0 ) {
		  for( int i = 0 ; i < 3 ; i++ ) {
		    tabKPtranslations[ 3 * ind + i ] 
		      = table->GetNumericalValue( 3 * indKP + i );
		    if( indKP < 2 ) {
		      // printf( "ch1 (%d/%d/%s) %d:%.2f \n" , indKP , ind  , keypointTransfs[indKP]->id , i , tabKPtranslations[ 3 * ind + i ] );
		    }
		    // }
		  }
		  KPsize++;
		}
	      }
	      // printf( "\n\n" );
	      if( KPsize != currentKeypointSize ) {
		sprintf( ErrorStr , "Error: Incorrect final keypoint transformation size on channel 1 (%d) mask size is (%d)!" , KPsize , currentKeypointSize ); ReportError( ErrorStr ); 
	      }
	    }
	    else {
	      for( int indKP = 0 ; indKP < currentKeypointSize * 3 ; indKP++ ) {
		tabKPtranslations[ indKP ] 
		  = table->GetNumericalValue( indKP );
		// fprintf( fileLog , "%.2f " , tabKPtranslations[ indKP ] );
	      }
	      // fprintf( fileLog , "\n" );
	    }
	    // fprintf( fileLog , "Shader end param assignation (ch 1)\n" );
	    cgGLSetParameterArray3f( vertShader->KPtranslationsEndParam_1 ,
				     0 , currentKeypointSize , 
				     tabKPtranslations );
	  }
	  else if( indChannel == 2 && vertShader->KPtranslationsEndParam_2 ) {
	    int KPsize = 0;
	    if( indirectionTableKeypointIndices_2 ) {
	      // printf( "\nchannel2 - target: " );
	      for( int indKP = 0 ; indKP < keypointSize ; indKP++ ) {
		int ind = indirectionTableKeypointIndices_2[indKP];
		if( ind >= 0 ) {
		  for( int i = 0 ; i < 3 ; i++ ) {
		    tabKPtranslations[ 3 * ind + i ] 
		      = table->GetNumericalValue( 3 * indKP + i );
		    if( indKP < 2 ) {
		      // printf( "ch2 (%d/%d/%s) %d:%.2f \n" , indKP , ind  , keypointTransfs[indKP]->id , i , tabKPtranslations[ 3 * ind + i ] );
		    }
		  }
		  KPsize++;
		}
	      }
	      // printf( "\n\n" );
	      if( KPsize != currentKeypointSize ) {
		sprintf( ErrorStr , "Error: Incorrect final keypoint transformation size on channel 2 (%d) mask size is (%d)!" , KPsize , currentKeypointSize ); ReportError( ErrorStr ); 
	      }
	    }
	    else {
	      for( int indKP = 0 ; indKP < keypointSize * 3 ; indKP++ ) {
		tabKPtranslations[ indKP ] 
		  = table->GetNumericalValue( indKP );
		// fprintf( fileLog , "%.2f " , tabKPtranslations[ indKP ] );
	      }
	      // fprintf( fileLog , "\n" );
	    }

	    // fprintf( fileLog , "Shader end param assignation (ch 2)\n" );
	    cgGLSetParameterArray3f( vertShader->KPtranslationsEndParam_2 ,
				     0 , currentKeypointSize , 
				     tabKPtranslations );
	  }
	}
      }
      else {
	sprintf( ErrorStr , "Error: Expected table [%s] on node [%s] of size >= %d!" , tableId , tableNode->id , keypointSize * 3  ); ReportError( ErrorStr ); 
      }

      delete [] tabKPtranslations;
	  tabKPtranslations = NULL;

    }
    else {
      sprintf( ErrorStr , "Error: Expected tables [%s]!" ,visemeTableId ); ReportError( ErrorStr ); 
    }
  }
  // subdivision has not a shader parameter with all the necessary parameters
  else {
    sprintf( ErrorStr , "Error: Missing vertex shader or vertex shader parameters for facial animation in subdivision [%s]!" , id ); ReportError( ErrorStr ); 
  }
#endif
}

void vc_Subdivision::updateKeypoints( float *tabKPtranslations ) {
  // in case of GPU rendering, collects details about the KP texture
  // and updates the texture from the KP transformations
#ifdef VC_HAVE_CG
  vc_Shader *subdivisionShader = GetMaterialShader();
  vc_VertexShader *vertShader = NULL;
  if( subdivisionShader ) {
    vertShader = subdivisionShader->vertexShader[ CurrentIndWindow ];
    //printf( " vertShader %d\n" , vertShader );
  }
  if( vertShader
      && vertShader->KPtranslationsIniParam_1
      && gpu_KPanim 
      && subdivisionShader
      && subdivisionShader->shaderType == GENERALIZED_SHADER  ) {

    // transformations compted from video analysis
    // printf( "\n" );
    cgGLSetParameterArray3f( vertShader->KPtranslationsIniParam_1 ,
			     0 , keypointSize , tabKPtranslations );
  }
#endif
}

bool vc_Subdivision::updateBones( float * subdivisionModelViewMatrix ) {
  bool updated = false;
  // bone controlled mesh transformations
  vc_matrix44 * relativeNodeTransformationMVM = new vc_matrix44[ boneSize ];

  // absolute subdivision transformation
  vc_matrix44 subdModelViewMatrix44( subdivisionModelViewMatrix );

  // invert absolute subdivision transformation
  float subdModelViewMatrixInv [ 16 ];
  invert_matrix( subdivisionModelViewMatrix , subdModelViewMatrixInv );
  vc_matrix44 subdModelViewMatrixInv44( subdModelViewMatrixInv );

  for( int indBone = 0 ; indBone < boneSize ; indBone++ ) {
    ///////////////////////////////////////////////////////////
    // precomputation of bone local transformations

    // bone controlled mesh transformation
    //  subd -1 * boneLocalTransf * subd
    // bone local transf is the composition of the
    // upper bone transformation in their joint coordinate system
    vc_Bone * boneNode = (vc_Bone *)(boneNodes[ indBone ]->sceneObject);
    vc_matrix44 m =  subdModelViewMatrixInv44 *
      boneNode->boneLocalTransformation * subdModelViewMatrix44;

    relativeNodeTransformationMVM[ indBone ] = m;

    if( bonePrecedingMatrices[indBone] != m ) {
      updated = true;
      bonePrecedingMatrices[indBone] = m;
    }
  }

  if( updated ) {
    ///////////////////////////////////////////////////////////
    // vertex displacement as a function of bone relative transformations

    // for each wieghted vertex: modify the coordinates as a weighted
    // sum of translation transformations
    for( int indVertex = 0 ; indVertex < indVerticesEnd[ 0 ] ; indVertex++ ) {
      //       printf( "Indvertex %d size %d\n" , indVertex , tabVertices[indVertex].boneWeightingSize );
      if( tabVertices[indVertex].boneWeightingSize > 0 ) {
	// remember the vertex
	vc_Vertex * vert = &(tabVertices[indVertex]);
	// copy initial coordinates from x_ini, y_ini, z_ini
	vert->x = vert->x_ini;
	vert->y = vert->y_ini;
	vert->z = vert->z_ini;

	// TODO: stocker la transformation courante OpenGL sur chacun
	// des bones Mb et la transformation courante de la
	// subdivision Ms

	// - puis calculer la transformation du bone dans
	// le repere de la subdivision Mbs = Mb * Ms -1

	// - puis appliquer
	// les transformations Mbs * Rb (Rb la rotation du bone) sur
	// chaque vertex et calculer le barycentre des images de
	// chaque vertex par l'ensemble des transformations Mbs

	// Rem: les coefficients du barycentre sont normalis�s pour que
	// leur somme soit �gale � 1

	// apply a weighted sum of local bone transformations
	vc_point3 barycenter( 0 , 0 , 0 );

	for( int indWeight = 0 ;
	     indWeight < vert->boneWeightingSize ;
	     indWeight++ ) {
	  vc_BoneWeighting * bone_w = &(vert->tabBoneWeighting[indWeight]);
	  // 	  vc_Transformation * rotation
	  // 	    = ((vc_Transformation *)(boneNodes[bone_w->indBone]->sceneObject));

	  vc_point3 transformedVertex;
	  vc_point3 vertexPoint( vert->x , vert->y , vert->z );
	  transformedVertex.product( vertexPoint ,
				     relativeNodeTransformationMVM[bone_w->indBone] );

	  transformedVertex *= bone_w->weight;
	  barycenter += transformedVertex;

	  // 	  printf( "vertex %.2f %.2f %.2f / transformedVertex %.2f %.2f %.2f  / weight %.2f\n" ,
	  // 		  vert->x , vert->y , vert->z ,
	  // 		  transformedVertex.x , transformedVertex.y , transformedVertex.z ,
	  // 		  bone_w->weight );
	}
	vert->x = barycenter.x;
	vert->y = barycenter.y;
	vert->z = barycenter.z;
      }
    }
  }
  delete [] relativeNodeTransformationMVM;
  relativeNodeTransformationMVM = NULL;

  return updated;
}

void vc_Subdivision::updateBasePolygon( vc_Vertex *localTabVertices ,
					vc_TextureCoord *localTabTextureCoords ,
					vc_vector3 *localTabNormals ,
					vc_vector3 *localTabTangents ,
// 					GLfloat *localTabColors ,
					vc_Face *localTabFaces ,
					int numberOfInitialVertices ,
					int numberOfInitialTextureCoords ,
					int numberOfInitialNormals ,
					int numberOfInitialTangents ,
// 					int numberOfInitialColors ,
					int numberOfInitialFaces ) {
  // initialization
  for( int indVertex = 0 ; indVertex < nbMaxVertices ; indVertex++ ) {
    tabVertices[ indVertex ].init();
#ifdef VC_CRYSTAL_CAHEN
    tabDistances[ indVertex ] = 0.0;
#endif
  }
  for( int indVertex = 0 ; indVertex < nbMaxTextureCoords ; indVertex++ ) {
    tabTextureCoords[ indVertex ].u = 0.0;
    tabTextureCoords[ indVertex ].v = 0.0;
  }
  for( int indNormal = 0 ; indNormal < nbMaxNormals ; indNormal++ ) {
    tabNormals[ indNormal ].x = 0.0;
    tabNormals[ indNormal ].y = 0.0;
    tabNormals[ indNormal ].z = 0.0;
  }
  for( int indTangent = 0 ; indTangent < nbMaxTangents ; indTangent++ ) {
    tabTangents[ indTangent ].x = 0.0;
    tabTangents[ indTangent ].y = 0.0;
    tabTangents[ indTangent ].z = 0.0;
  }
//   for( int indVertex = 0 ; indVertex < nbMaxColors ; indVertex++ ) {
//     GLfloat * color = &tabColors[ indVertex * 4 ];
//     for( int ind = 0 ; ind < 4 ; ind++ ) {
//       color[ ind ] = 1.0;
//     }
//   }
  for( int indFace = 0 ; indFace < nbMaxFaces ; indFace++ ) {
    tabFaces[ indFace ].init();
  }

  // values of vertices given as parameters are
  // assigned to the vertices and faces at level 0
  for( int ind  = 0 ; ind < numberOfInitialVertices ; ind++ ) {
    tabVertices[ind].x = localTabVertices[ind].x;
    tabVertices[ind].y = localTabVertices[ind].y;
    tabVertices[ind].z = localTabVertices[ind].z;
    tabVertices[ind].x_ini = localTabVertices[ind].x_ini;
    tabVertices[ind].y_ini = localTabVertices[ind].y_ini;
    tabVertices[ind].z_ini = localTabVertices[ind].z_ini;
    tabVertices[ind].vertexType = INITIAL_VERTEX;
    tabVertices[ind].keypointWeightingSize
      = localTabVertices[ind].keypointWeightingSize;
    tabVertices[ind].tabKeypointWeighting
      = localTabVertices[ind].tabKeypointWeighting;
    tabVertices[ind].boneWeightingSize
      = localTabVertices[ind].boneWeightingSize;
    tabVertices[ind].tabBoneWeighting
      = localTabVertices[ind].tabBoneWeighting;
  }
  for( int ind  = 0 ; ind < numberOfInitialTextureCoords ; ind++ ) {
    tabTextureCoords[ind].u = localTabTextureCoords[ind].u;
    tabTextureCoords[ind].v = localTabTextureCoords[ind].v;
  }
  for( int ind  = 0 ; ind < numberOfInitialNormals ; ind++ ) {
    tabNormals[ind].x = localTabNormals[ind].x;
    tabNormals[ind].y = localTabNormals[ind].y;
    tabNormals[ind].z = localTabNormals[ind].z;
  }
  for( int ind  = 0 ; ind < numberOfInitialTangents ; ind++ ) {
    tabTangents[ind].x = localTabTangents[ind].x;
    tabTangents[ind].y = localTabTangents[ind].y;
    tabTangents[ind].z = localTabTangents[ind].z;
  }
//   for( int indVertex  = 0 ; indVertex < numberOfInitialColors * 4 ; indVertex++ ) {
//     tabColors[indVertex] = localTabColors[indVertex];
//   }
  for( int ind  = 0 ; ind < numberOfInitialFaces ; ind++ ) {
    tabFaces[ind] = localTabFaces[ind];
  }

  // the extremal index values for level 0 are assigned
  // to the table that contain these values for each level
  indVerticesIni[ 0 ] = 0;
  indTextureCoordsIni[ 0 ] = 0;
  indNormalsIni[ 0 ] = 0;
  indTangentsIni[ 0 ] = 0;
//   indColorsIni[ 0 ] = 0;
  indFacesIni[ 0 ] = 0;
  indVerticesEnd[ 0 ] = numberOfInitialVertices;
  indTextureCoordsEnd[ 0 ] = numberOfInitialTextureCoords;
  indNormalsEnd[ 0 ] = numberOfInitialNormals;
  indTangentsEnd[ 0 ] = numberOfInitialTangents;
//   indColorsEnd[ 0 ] = numberOfInitialColors;
  indFacesEnd[ 0 ] = numberOfInitialFaces;
  indEdgesIni[ 0 ] = 0;
  indEdgesEnd[ 0 ] = 0;


  int * tabIndConnectedVertices = new int[ max_degree ];
  int * tabIndConnectedVerticesCopy = new int[ max_degree ];
  int * tabConnectedFaces = new int[ max_degree ];
  int * tabConnectedFacesCopy = new int[ max_degree ];
  vc_vector3 * normals = new vc_vector3[ max_degree ];
  vc_vector3 * tangents = new vc_vector3[ max_degree ];
  if( !tabIndConnectedVertices || !tabIndConnectedVerticesCopy || !tabConnectedFaces || !tabConnectedFacesCopy || !normals || !tangents 
      ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }

  // automatic normals: computed as the average of the
  // usrrounding normal vectors
  int indVertexLocal;
  int nbConnectedVertices;
  int nbConnectedFaces;
  for( int ind  = 0 ; ind < numberOfInitialFaces ; ind++ ) {
    // normal not defined in the initial mesh
    // an average normal is associated with this vertex and all its
    // other occurrences in the mesh
    if( tabFaces[ind].indNormal1 == -1 ) {
      if( numberOfInitialNormals < nbMaxNormals 
	  && numberOfInitialTangents < nbMaxTangents ) {
	indVertexLocal = tabFaces[ind].indVertex1;
	vc_vector3 normal;
	vc_vector3 tangent;
	if( flatShading ) {
	  uniqueNormal( ind , -1 , &normal );
	  uniqueTangent( ind , -1 , &normal , &tangent );
	  tabNormals[numberOfInitialNormals].x = normal.x;
	  tabNormals[numberOfInitialNormals].y = normal.y;
	  tabNormals[numberOfInitialNormals].z = normal.z;
	  tabTangents[numberOfInitialTangents].x = tangent.x;
	  tabTangents[numberOfInitialTangents].y = tangent.y;
	  tabTangents[numberOfInitialTangents].z = tangent.z;
	  tabFaces[ind].indNormal1 = numberOfInitialNormals;
	  tabFaces[ind].indTangent1 = numberOfInitialTangents;
	}
	else {
	  averageNormal( tabFaces[ind].indVertex1 , -1 ,
			 &normal ,
			 0 ,
			 tabIndConnectedVertices ,
			 tabIndConnectedVerticesCopy ,
			 tabConnectedFaces ,
			 tabConnectedFacesCopy ,
			 &nbConnectedVertices ,
			 &nbConnectedFaces ,
			 normals );
	  averageTangent( tabFaces[ind].indVertex1 , 
			  &tangent , 
			  &normal ,
			  tabConnectedFaces ,
			  nbConnectedFaces ,
			  tangents );
	  tabNormals[numberOfInitialNormals].x = normal.x;
	  tabNormals[numberOfInitialNormals].y = normal.y;
	  tabNormals[numberOfInitialNormals].z = normal.z;
	  tabTangents[numberOfInitialTangents].x = tangent.x;
	  tabTangents[numberOfInitialTangents].y = tangent.y;
	  tabTangents[numberOfInitialTangents].z = tangent.z;
	  tabFaces[ind].indNormal1 = numberOfInitialNormals;
	  tabFaces[ind].indTangent1 = numberOfInitialTangents;
	}
      }
      else {
	sprintf( ErrorStr , "Error: Insufficient parameter max_normals [%d/%d] or max_tangents [%d/%d] or  for automatic normals and tangents (face %d vertex 1) in subdivision ID [%s]!" , nbMaxNormals , numberOfInitialNormals , nbMaxTangents , numberOfInitialTangents , ind + 1 , id ); ReportError( ErrorStr ); throw 387;
      }
      
      // looking for other occurrences of this vertex (including this face)
      if( !flatShading ) {
	for( int indAux = ind ; indAux < numberOfInitialFaces ; indAux++ ) {
	  if( tabFaces[indAux].indVertex1 == indVertexLocal ) {
	    tabFaces[indAux].indNormal1 = numberOfInitialNormals;
	    tabFaces[indAux].indTangent1 = numberOfInitialTangents;
	  }
	  if( tabFaces[indAux].indVertex2 == indVertexLocal ) {
	    tabFaces[indAux].indNormal2 = numberOfInitialNormals;
	    tabFaces[indAux].indTangent2 = numberOfInitialTangents;
	  }
	  if( tabFaces[indAux].indVertex3 == indVertexLocal ) {
	    tabFaces[indAux].indNormal3 = numberOfInitialNormals;
	    tabFaces[indAux].indTangent3 = numberOfInitialTangents;
	  }
	}
      }
      numberOfInitialNormals++;
      numberOfInitialTangents++;
    }
    if( !flatShading && tabFaces[ind].indNormal2 == -1 ) {
      if( numberOfInitialNormals < nbMaxNormals 
	  && numberOfInitialTangents < nbMaxTangents ) {
	indVertexLocal = tabFaces[ind].indVertex2;
	vc_vector3 normal;
	vc_vector3 tangent;
	averageNormal( tabFaces[ind].indVertex2 , -1 ,
		       &normal ,
		       0 ,
		       tabIndConnectedVertices ,
		       tabIndConnectedVerticesCopy ,
		       tabConnectedFaces ,
		       tabConnectedFacesCopy ,
		       &nbConnectedVertices ,
		       &nbConnectedFaces ,
		       normals );
	averageTangent( tabFaces[ind].indVertex2 , 
			&tangent , &normal ,
			tabConnectedFaces ,
			nbConnectedFaces ,
			tangents );
	tabNormals[numberOfInitialNormals].x = normal.x;
	tabNormals[numberOfInitialNormals].y = normal.y;
	tabNormals[numberOfInitialNormals].z = normal.z;
	tabTangents[numberOfInitialTangents].x = tangent.x;
	tabTangents[numberOfInitialTangents].y = tangent.y;
	tabTangents[numberOfInitialTangents].z = tangent.z;
	tabFaces[ind].indNormal2 = numberOfInitialNormals;
	tabFaces[ind].indTangent2 = numberOfInitialTangents;
      }
      else {
	sprintf( ErrorStr , "Error: Insufficient parameter max_normals [%d/%d] or max_tangents [%d/%d] or  for automatic normals and tangents (face %d vertex 1) in subdivision ID [%s]!" , nbMaxNormals , numberOfInitialNormals , nbMaxTangents , numberOfInitialTangents , ind + 1 , id ); ReportError( ErrorStr ); throw 387;
      }

      // looking for other occurrences of this vertex (including this face)
      for( int indAux = ind ; indAux < numberOfInitialFaces ; indAux++ ) {
	if( tabFaces[indAux].indVertex1 == indVertexLocal ) {
	  tabFaces[indAux].indNormal1 = numberOfInitialNormals;
	  tabFaces[indAux].indTangent1 = numberOfInitialTangents;
	}
	if( tabFaces[indAux].indVertex2 == indVertexLocal ) {
	  tabFaces[indAux].indNormal2 = numberOfInitialNormals;
	  tabFaces[indAux].indTangent2 = numberOfInitialTangents;
	}
	if( tabFaces[indAux].indVertex3 == indVertexLocal ) {
	  tabFaces[indAux].indNormal3 = numberOfInitialNormals;
	  tabFaces[indAux].indTangent3 = numberOfInitialTangents;
	}
      }

      numberOfInitialNormals++;
      numberOfInitialTangents++;
    }
    if( !flatShading && tabFaces[ind].indNormal3 == -1 ) {
      if( numberOfInitialNormals < nbMaxNormals 
	  && numberOfInitialTangents < nbMaxTangents ) {
	indVertexLocal = tabFaces[ind].indVertex3;
	vc_vector3 normal;
	vc_vector3 tangent;
	averageNormal( tabFaces[ind].indVertex3 , -1 ,
		       &normal ,
		       0 ,
		       tabIndConnectedVertices ,
		       tabIndConnectedVerticesCopy ,
		       tabConnectedFaces ,
		       tabConnectedFacesCopy ,
		       &nbConnectedVertices ,
		       &nbConnectedFaces ,
		       normals );
	averageTangent( tabFaces[ind].indVertex3 , 
			&tangent , &normal ,
			tabConnectedFaces ,
			nbConnectedFaces ,
			tangents );
	tabNormals[numberOfInitialNormals].x = normal.x;
	tabNormals[numberOfInitialNormals].y = normal.y;
	tabNormals[numberOfInitialNormals].z = normal.z;
	tabTangents[numberOfInitialTangents].x = tangent.x;
	tabTangents[numberOfInitialTangents].y = tangent.y;
	tabTangents[numberOfInitialTangents].z = tangent.z;
	tabFaces[ind].indNormal3 = numberOfInitialNormals;
	tabFaces[ind].indTangent3 = numberOfInitialTangents;
      }
      else {
	sprintf( ErrorStr , "Error: Insufficient parameter max_normals [%d/%d] or max_tangents [%d/%d] or  for automatic normals and tangents (face %d vertex 1) in subdivision ID [%s]!" , nbMaxNormals , numberOfInitialNormals , nbMaxTangents , numberOfInitialTangents , ind + 1 , id ); ReportError( ErrorStr ); throw 387;
      }

      // looking for other occurrences of this vertex (including this face)
      for( int indAux = ind ; indAux < numberOfInitialFaces ; indAux++ ) {
	if( tabFaces[indAux].indVertex1 == indVertexLocal ) {
	  tabFaces[indAux].indNormal1 = numberOfInitialNormals;
	  tabFaces[indAux].indTangent1 = numberOfInitialTangents;
	}
	if( tabFaces[indAux].indVertex2 == indVertexLocal ) {
	  tabFaces[indAux].indNormal2 = numberOfInitialNormals;
	  tabFaces[indAux].indTangent2 = numberOfInitialTangents;
	}
	if( tabFaces[indAux].indVertex3 == indVertexLocal ) {
	  tabFaces[indAux].indNormal3 = numberOfInitialNormals;
	  tabFaces[indAux].indTangent3 = numberOfInitialTangents;
	}
      }

      numberOfInitialNormals++;
      numberOfInitialTangents++;
    }
  }

  delete [] tabIndConnectedVertices;
  tabIndConnectedVertices = NULL;

  delete [] tabIndConnectedVerticesCopy;
  tabIndConnectedVerticesCopy = NULL;

  delete [] tabConnectedFaces;
  tabConnectedFaces = NULL;

  delete [] tabConnectedFacesCopy;
  tabConnectedFacesCopy = NULL;

  delete [] normals;
  normals = NULL;

  delete [] tangents;
  tangents = NULL;

  // updates the number of initial normals
  indNormalsEnd[ 0 ] = numberOfInitialNormals;
  indTangentsEnd[ 0 ] = numberOfInitialTangents;


  // SHADOW
  computeAverageNormalFace( 0 );
  computeAverageTangentFace( 0 );
  orientFacesVertices( 0 );
  computeTabEdges( 0 );

  // CPU vertex shader
#ifdef VC_LUMISSON
  vertexShader();
#endif

}

#ifdef VC_LUMISSON
void vc_Subdivision::vertexShader( void ) {
  float wave_amplitude = 0.1;
  float period = M_PI * 5;
  float length, sinLength, cosLength;
  float twist_amplitude = sin( CurrentClockTime / 40.0 ) * 4.0;
  float centerx = 0.5;
  float centery = 0.5;

  char update[XMLAttrChLength];
  *update = 0;
  ReferenceNodeAttributeValue( "{$main node:update}" , true , update );

  // printf( "update (%s)\n" , update );
  // before the end of compiling: variable values are undetermined
  // or scene has no node named "main node" with a string var "update"
  if( !(*update) ) {
    return;
  }

  bool erase = (strcmp( update , "erase" ) == 0);

  if( strcmp( update , "manual" ) != 0 ) {
    if( erase ) {
      twist_amplitude = 0;
    }

    //printf( "update (%s) twist_amplitude %.2f\n" , update , twist_amplitude );

    // initialization
    for( int ind  = 0 ; ind < nbMaxVertices ; ind++ ) {
      tabVertices[ ind ].modified = false;
    }
    for( int indFace  = 0 ; indFace < nbMaxFaces ; indFace++ ) {
      int indVertex = 0;
      int indTexture = 0;
      for( int rank  = 0 ; rank < 3 ; rank++ ) {
	if( rank == 0 ) {
	  indVertex = tabFaces[indFace].indVertex1;
	  indTexture = tabFaces[indFace].indTextureCoord1;
	}
	else if( rank == 1 ) {
	  indVertex = tabFaces[indFace].indVertex2;
	  indTexture = tabFaces[indFace].indTextureCoord2;
	}
	else if( rank == 2 ) {
	  indVertex = tabFaces[indFace].indVertex3;
	  indTexture = tabFaces[indFace].indTextureCoord3;
	}

	// texture coordinates are aligned to the underlining grid
	if( !tabVertices[ indVertex ].modified ) {
	  // warp bisinusoidal surface
	  if( !erase ) {
	    float x = tabTextureCoords[indTexture].u - centerx;
	    float y = tabTextureCoords[indTexture].v - centery;
	    if( !tabVertices[ indVertex ].z ) {
	      tabVertices[ indVertex ].z
		= sin( (x * period) + sin( y * period ) ) * wave_amplitude;
	    }
	    length = sqrt(x * x + y * y);
	    float angle = twist_amplitude * length;
	    cosLength = cos( angle );
	    sinLength = sin( angle );
	    tabVertices[ indVertex ].x = cosLength * x - sinLength * y + centerx;
	    tabVertices[ indVertex ].y = sinLength * x + cosLength * y + centery;
	  }
	  // flattens the surface
	  else {
	    tabVertices[ indVertex ].x = tabTextureCoords[indTexture].u;
	    tabVertices[ indVertex ].y = tabTextureCoords[indTexture].v;
	    tabVertices[ indVertex ].z = 0;
	  }
	  tabVertices[ indVertex ].modified = true;
	}
      }
    }
  }

  // TODO + 2 pdts vectoriels et la definition des normales indNormal2 et indNormal3
  for( int ind  = 0 ; ind < nbMaxFaces ; ind++ ) {
    // each face is associated with a normal
    if( tabFaces[ind].indNormal1 != -1 ) {
      if( !erase ) {
	vc_vector3 normal;
	tabFaces[ind].normal( &normal , tabVertices );

	tabNormals[tabFaces[ind].indNormal1].x = normal.x;
	tabNormals[tabFaces[ind].indNormal1].y = normal.y;
	tabNormals[tabFaces[ind].indNormal1].z = normal.z;
      }
      // unique normal along Oz
      else {
	tabNormals[tabFaces[ind].indNormal1].x = 0;
	tabNormals[tabFaces[ind].indNormal1].y = 0;
	tabNormals[tabFaces[ind].indNormal1].z = 1;
      }
    }
    if( tabFaces[ind].indNormal2 != -1 ) {
      if( !erase ) {
	vc_vector3 normal;
	tabFaces[ind].normal( &normal , tabVertices );

	tabNormals[tabFaces[ind].indNormal2].x = normal.x;
	tabNormals[tabFaces[ind].indNormal2].y = normal.y;
	tabNormals[tabFaces[ind].indNormal2].z = normal.z;
      }
      // unique normal along Ox
      else {
	tabNormals[tabFaces[ind].indNormal2].x = 1;
	tabNormals[tabFaces[ind].indNormal2].y = 0;
	tabNormals[tabFaces[ind].indNormal2].z = 0;
      }
    }
    if( tabFaces[ind].indNormal3 != -1 ) {
      if( !erase ) {
	vc_vector3 normal;
	tabFaces[ind].normal( &normal , tabVertices );

	tabNormals[tabFaces[ind].indNormal2].x = normal.x;
	tabNormals[tabFaces[ind].indNormal2].y = normal.y;
	tabNormals[tabFaces[ind].indNormal2].z = normal.z;
      }
      // unique normal along Oy
      else {
	tabNormals[tabFaces[ind].indNormal2].x = 0;
	tabNormals[tabFaces[ind].indNormal2].y = 1;
	tabNormals[tabFaces[ind].indNormal2].z = 0;
      }
    }
  }
}

void *send_network(void *ar);

bool vc_Subdivision::MeshLevelInteraction( vc_point2 aPoint ,
					   vc_matrix44 * modelViewSubdivisionMatrix, 
					   vc_SceneNode * node,
					   GLubyte pixelColor_METISSON[]) {
  int indFace, toto = 1;
  float alpha, beta, gamma, d;

  char mode[XMLAttrChLength];
  *mode = 0;
  ReferenceNodeAttributeValue( "{$main node:mode}" , true , mode );

  float delta = 0.0;
  if( strcmp( mode , "bump" ) == 0 ) {
    delta = -0.001;
  }
  else if( strcmp( mode , "carve" ) == 0 ) {
    delta = 0.001;
  }
  else if( strcmp( mode , "read" ) == 0 ) {
    delta = 0.0;
  }

  //distance from mouse of a mesh
  float distx, disty;

  indFace = TriangleIntersectf( aPoint , CurrentFrustum->nearFrustum ,
				modelViewSubdivisionMatrix ,
				&alpha , &beta , &d , &distx, &disty);

  gamma = 1 - alpha - beta;

  float a1 = alpha;
  float b1 = beta;
  float g1 = gamma;

  alpha = g1;
  beta = a1;
  gamma = b1;
  if (indFace < 0)
    return false;
  return true;
  if (indFace < 0)
    {
      indFace = 0;
      toto = 0;
    }
  // calculate the normal Absolute for the three vertex
  vc_vector3 normalAbsolute[3];
  vc_point3 vertexAbsolute[3];
  for (int i=0; i < 3; i++)
    {
      int indNormal = -1;
      int indVertex = -1;
      switch (i) {
	case 0:
	  indVertex =  tabFaces[indFace].indVertex1;
	  indNormal = tabFaces[indFace].indNormal1;
	  break;
	case 1:
	  indVertex =  tabFaces[indFace].indVertex2;
	  indNormal = tabFaces[indFace].indNormal2;
	  break;
	case 2:
	  indVertex =  tabFaces[indFace].indVertex3;
	  indNormal = tabFaces[indFace].indNormal3;
	  break;
	default:

	  printf("This msg should never appear\n");
	  indNormal = indNormal;
	  }

      vc_vector3 * normal;
      if( indNormal >= 0 ) {
	normal = &tabNormals[ indNormal ];
	normalAbsolute[i].product( *normal , *modelViewSubdivisionMatrix );
      }
      else {
	//return false;
      } 
      vc_Vertex * vertex;
      vc_point3 p;
      if( indVertex >= 0 ) {
	vertex = &(tabVertices[ indVertex ]);
	p.x = vertex->x; p.y = vertex->y; p.z = vertex->z;
	vertexAbsolute[i].product( p , *modelViewSubdivisionMatrix );
      }
      else {
	//return false;
      }
      indNormal = -1;
      indVertex = -1;
    }
  vc_point3 vertexCentre((vertexAbsolute[0].x * alpha + vertexAbsolute[1].x * beta + vertexAbsolute[2].x * gamma),
			 (vertexAbsolute[0].y * alpha + vertexAbsolute[1].y * beta + vertexAbsolute[2].y * gamma),
			 (vertexAbsolute[0].z * alpha + vertexAbsolute[1].z * beta + vertexAbsolute[2].z * gamma));

  vc_vector3 normalCentre((normalAbsolute[0].x * alpha + normalAbsolute[1].x * beta + normalAbsolute[2].x * gamma),
			  (normalAbsolute[0].y * alpha + normalAbsolute[1].y * beta + normalAbsolute[2].y * gamma),
			  (normalAbsolute[0].z * alpha + normalAbsolute[1].z * beta + normalAbsolute[2].z * gamma));
  float cosAngleOfView = fabs(normalCentre.z);


    // vertex location modification along the normal
  vc_vector3 * normal = &(tabNormals[ tabFaces[indFace].indNormal1 ]);
  vc_Vertex * vertex= &(tabVertices[ tabFaces[indFace].indVertex1 ]);
  if( delta ) {
    vc_vector3 extrusion = *normal;
    extrusion *= delta;

    *vertex += extrusion;
    if( tabFaces[indFace].indVertex2 >= 0 ) {
      tabVertices[ tabFaces[indFace].indVertex2 ] += extrusion;
    }
    if( tabFaces[indFace].indVertex3 >= 0 ) {
      tabVertices[ tabFaces[indFace].indVertex3 ] += extrusion;
    }
  }
  struct timeval time;
  gettimeofday(&time, 0);

  float cosAngleLight[VC_CompositionNode->nbRegisteredLights];
  vc_Light *light[VC_CompositionNode->nbRegisteredLights];

  //printf("draws lights\n" );
  if( VC_CompositionNode->tabLights ) {
    for( int indLight = 0 ;
	 indLight < VC_CompositionNode->nbRegisteredLights ;
	 indLight++ ) {
      // reloads the currrent matrix associated
      // with the displayed light
      vc_matrix44 *modelViewLightMatrix
	= VC_CompositionNode->tabLights[ indLight ]->getModelViewMatrix44();
      
      vc_Light *ptLight;
      vc_SceneObject * sobj
	= VC_CompositionNode->tabLights[ indLight ]->sceneObject;
      if( VC_CompositionNode->tabLights[ indLight ]->type == ALight ) {
	// light scene node
	ptLight = (vc_Light *)sobj;
      }
      else if( VC_CompositionNode->tabLights[ indLight ]->type == AInterpolator ) {
	// interpolated light
	ptLight = (vc_Light *)(((vc_NodeInterpolator *)sobj)->curNode->sceneObject);
      }
      else {
	sprintf( ErrorStr , "Error: unexpected light type (%s)!" , VC_CompositionNode->tabLights[ indLight ]->id ); ReportError( ErrorStr ); throw 83;
      }
      
      //ptLight->display( mode , ind , true , true , true );
      light[indLight] = ptLight;
      vc_point3 locationAbsolute;
      locationAbsolute.product( ptLight->location ,
				*modelViewLightMatrix );
      
      vc_vector3 lightVector( vertexCentre , locationAbsolute );
      
      lightVector.normalize();
      cosAngleLight[ indLight ] = fabs( lightVector.prodScal( normalCentre ));
    }
  }


  d = fabs(1/d);
  //il faudrait definir un truc si on a pas envie du lissage
  if (false)
    Lissage(d, light, cosAngleLight, cosAngleOfView);
  else
    {
      vc_info *info = new vc_info(VC_CompositionNode->nbRegisteredLights , cosAngleOfView, d, 0);
      list<float>::iterator current = info->cosAngleLight.begin();
      int i = 0;
      while (current != info->cosAngleLight.end()) {
	  *current = cosAngleLight[i];
	  current++;
	  i++;
	  }
      info->idform = node->id;
      info->posx = (float) CurrentMousePos_x / (float) CurrentWindow->getWindowWidth() ;
      info->posy = (float) (CurrentWindow->getWindowHeight() - CurrentMousePos_y) / (float) CurrentWindow->getWindowHeight() ;
      info->posz = 0;
      if (toto == 1) {
	  info->distx = 1;
	  info->disty = 1;
	}
      else {
	  if (distx < 1 && distx > 0)
	    distx += 1;
	  if (disty < 1 && disty > 0)
	    disty += 1;
	  if (distx > -1 && distx < 0)
	    distx -= 1;
	  if (disty > -1 && disty < 0)
	    disty -= 1;
	  info->distx = 1/distx;
	  info->disty = 1/disty;
	}
      //      char    cmdLine[XMLAttrChLength];
      info->R = pixelColor_METISSON[ 0 ];
      info->G = pixelColor_METISSON[ 1 ];
      info->B = pixelColor_METISSON[ 2 ];

      info->distance = d;
      info->in = toto;
      list_buffer->push_back(info);
      if (!launch) {
	  launch = 1;
	  //	  pthread_create(&thread, NULL, send_network, this);
	  send_network(this);
	}
    }
  return toto;
}

void vc_Subdivision::Lissage(float d, vc_Light **light, float cosAngleLight[], float cosAngleOfView)
{

  struct timeval time;
  gettimeofday(&time, 0);
  const int nb_historic = 5;
  float tmp = time.tv_sec - first + (time.tv_usec / 1000000.);
  printf("%.2f\t%.2f\t#normal\n", tmp,
	 (  cosAngleLight[0] *  300));

  if ((sous_echantillonage == 3))
    {
      sous_echantillonage = 0;
      float  i = time.tv_sec - first + (time.tv_usec / 1000000.);

      //update historic
      int j;
      for (j = 1; j < nb_historic; j++) {
	  for( int indLight = 0 ;
	       indLight < VC_CompositionNode->nbRegisteredLights;
	       indLight++ ) {
	    light[indLight]->cosAngleLightOld[j - 1] = light[indLight]->cosAngleLightOld[j];
	  }
	  cosAngleOfViewOld[j - 1] = cosAngleOfViewOld[j];
	  distanceOld[j - 1] = distanceOld[j];
	  it_value[j - 1] = it_value[j];
	}
      for( int indLight = 0 ;
	   indLight < VC_CompositionNode->nbRegisteredLights ;
	   indLight++ ) {
	light[indLight]->cosAngleLightOld[j - 1] = cosAngleLight[indLight];
      }
      cosAngleLight1Old[j - 1] = cosAngleLight[0];
      cosAngleLight2Old[j - 1] = cosAngleLight[2];
      cosAngleOfViewOld[j - 1] = cosAngleOfView;
      distanceOld[j - 1] = d;
      it_value[j - 1] = i;

      //compute interpolation for 5 points
      float oldTime = 0;
      if (distanceOld[0] > 0) {
	  while (oldTime < 1)
	    {
	      vc_info *info = new vc_info(VC_CompositionNode->nbRegisteredLights , 0, 0, 0);

	      spline1(light,
		     cosAngleOfViewOld,
		     distanceOld,
		     it_value, oldTime, info);
	      oldTime += 0.2;
	      list_buffer->push_back(info);
	    }
	}
      if (!launch) {
	  launch = 1;
	  //pthread_join(thread, NULL);
	  pthread_create(&thread, NULL, send_network, this);
	}
    }
  else
    sous_echantillonage++;
}

void vc_Subdivision::spline1(vc_Light **light,
			    float cosAngleOfViewOld[],
			    float distanceOld [],
			    float it_value[],
			    float t, vc_info *info)
{
  float m[4][4] = { -0.1666666F, 0.5F,      -0.5F,      0.1666666F,
		    0.5F      ,-1.0F,       0.5F,      0.0F,
		    -0.5F      , 0.0F,       0.5F,      0.0F,
		    0.1666666F, 0.6666666F, 0.1666666F,0.0F };
  int j,k ;
  float tt[4],ttt[4];
  tt[0] = t*t*t ;
  tt[1] = t*t ;
  tt[2] = t ;
  tt[3] = 1. ;

  for ( j = 0 ; j < 4 ; j++ )
    {
      ttt[j] = 0. ;
      for ( k = 0 ; k < 4 ; k++ )
	ttt[j] += tt[k] * m[k][j] ;
    }
  for ( j = 0 ; j < 4 ; j++ )
    {
      list<float>::iterator current = info->cosAngleLight.begin();
      int cpt = 0;
      while (current != info->cosAngleLight.end()) {
	  *current += ttt[j] * light[cpt]->cosAngleLightOld[j];
	  current++;
	  cpt++;
	}
      info->cosAngleOfView += ttt[j] * cosAngleOfViewOld[j] ;
      info->distance += ttt[j] * distanceOld[j] ;
      info->time += ttt[j] * it_value[j] ;
    }
}

void vc_Subdivision::spline(float time[],
			    float values[],
			    float t,
			    float *rest, 
			    float *resv){
  float m[4][4] = { -0.1666666F, 0.5F,      -0.5F,      0.1666666F,
		    0.5F      ,-1.0F,       0.5F,      0.0F,
		    -0.5F      , 0.0F,       0.5F,      0.0F,
		    0.1666666F, 0.6666666F, 0.1666666F,0.0F };
  // compteur
  int j,k ;
  // variables temporaires de transition
  // contient les ponderation des points de controle
  float tt[4],ttt[4];
  tt[0] = t*t*t ;
  tt[1] = t*t ;
  tt[2] = t ;
  tt[3] = 1. ;
  
  //calcul des factueurs des points de controle de la spline
  for ( j = 0 ; j < 4 ; j++ )
    {
      ttt[j] = 0. ;
      for ( k = 0 ; k < 4 ; k++ )
	ttt[j] += tt[k] * m[k][j] ;
    }
  //calcul du resultat
  for ( j = 0 ; j < 4 ; j++ )
    {
      *resv += ttt[j] * values[j] ;
      *rest += ttt[j] * time[j] ;
    }
}

void  vc_Subdivision::polint(float time[], float values[], int n, float t, float *res) {
  // compteur
  int i,m = 0;

  //indice dans les tableau C et D le plus proche de x
  int ns = 0;
  //derive locale de la fonction
  float den;

  // temp to stock temporal values
  float dif,dift,ho,hp;


  float w;

  // C et D ca stocke les valeurs interpolees grace au polynome de lagrange determine sur I points de la fonction discrete d'entree
  float c[n];
  float d[n];

  dif=fabs(t-time[0]);


  for (i=0;i<n;i++)
  {
	if ( (dift=fabs(t-time[i])) < dif) {
	  ns=i;
	  dif=dift;
	}
    c[i]=values[i];
    d[i]=values[i];
  }
  *res=values[ns--];

  for (m=0;m<n;m++)
    {
      for (i=0;i<n-m;i++) {
	  ho=time[i]-t;
	  hp=time[i+m]-t;
	  w=c[i+1]-d[i];
	  if ( (den=ho-hp) == 0.0)
	    printf("Erreur dans polint\n");
	  den=w/den;
	  d[i]=hp*den;
	  c[i]=ho*den;
	}

	if (2 * ns < (n-m)) //point le plus proche qui permet une estimation avec le moins d'erreur
	  *res += c[ns + 1];
	else
	  *res += d[ns--];
    }
}



vc_info::vc_info(int nblight,
		 float cosAngleOfView_,
		 float distance_,
		 float time_)
{
  for (int i = 0; i < nblight; i++)
    {
      cosAngleLight.push_back(0);
    }
  cosAngleOfView = cosAngleOfView_;
  distance = distance_;
  time = time_;
}

vc_info::~vc_info() {
}

/*!
 * \brief send element 
 * \param ar				to be added
 *
 * This function takes what is in the list in argument
 * and send each element one by one keeping the
 * time space between values correctly.
 */
void *send_network(void *ar)
{
  //float compteur = 0;
  char cmdLine[XMLAttrChLength];
  static float lastTime = 0;
  vc_info *front;
  vc_Subdivision *arg = (vc_Subdivision *)ar;
  while (!arg->list_buffer->empty())
    {
      front = arg->list_buffer->front();
      for( int ind = 0 ; ind < environment->nb_IP_Clients ; ind++ ) {
	// push udp message
	if (!(!strcmp(environment->IP_Clients[ ind ]->id, "udp_client1")))
	if( environment->IP_Clients[ ind ]->SocketToRemoteServer >= 0
	    && environment->IP_Clients[ ind ]->listen_sound_messages ) {
	  struct timeval time;
	  gettimeofday(&time, 0);
	  float tmp = time.tv_sec - arg->first + (time.tv_usec / 1000000.);

	  if (front->time > lastTime)
	    {
	      if ((lastTime != 0))
		{
		  float spaceTime = front->time - lastTime;
		  //  Si jamais, on a trop de retard sur la courbe, on le rattrape ici,
		  //  en diminuant la valeur reelle du usleep par 0.75
		  //  ce probleme est du au fait qu on ne prend pas en
		  //  compte le temps des instructions
		  usleep((int) (spaceTime * 1000000. * 0.75));
		}
	       lastTime = front->time;
	    }

	  gettimeofday(&time, 0);
	  tmp = time.tv_sec - arg->first + (time.tv_usec / 1000000.);
	  //printf("%.2f\t%.2f\t#spline\n", tmp,  (front->cosAngleLight.front() *  300));
	  //printf("%.2f\t%.2f\t#diff\n",tmp, tmp -  front->time  );
	  //printf("%.2f\t%.2f\t#real\n", front->time, (front->cosAngleLight.front() *  300));
	  /* --------- Pour la lumiere  -------------
	  list<float>::iterator current = front->cosAngleLight.begin();

	  sprintf(cmdLine , "FM ");
	  while (current != front->cosAngleLight.end())
	    {
	      if (strlen(cmdLine) > XMLAttrChLength - 20)
		break;
	      sprintf(cmdLine + strlen(cmdLine), "%.2f ", *current * 150);
	      current++;
	    }

	  sprintf(cmdLine + strlen(cmdLine), "%.2f %.2f" ,
		  front->cosAngleOfView * 100,
		  front->distance * 100);
	  environment->IP_Clients[ ind ]
	    ->storeIP_output_message( cmdLine );
	    }*/
	  /*---------Pour le mobile sonore Thing------------------ */
	  	  sprintf(cmdLine , "form %s position %.2f %.2f %.2f distance %.2f %.2f %.2f inside %.2f inclinaison %.2f RGB %d %d %d",
		  front->idform,
		  front->posx, front->posy, front->posz,
		  front->distx,  front->disty, front->distance,
		  front->in,
		  front->cosAngleOfView,
		  (int) front->R, (int) front->G, (int) front->B
		  );
	  environment->IP_Clients[ ind ]
	    ->storeIP_output_message( cmdLine );

	}
}
      arg->list_buffer->pop_front();
      delete(front);
	  front = NULL;
  }
  arg->launch = 0;
  return NULL;
}

void vc_Composition::spotTriangle( int x, int y ) {
  float xPos = x;
  float yPos = y;

  GetUserAndWindow();
  static int sous_ech = 0;

  // printf( "Point %d %d win#%d\n", x,y ,CurrentIndWindow);
  CurrentWindow->relativeMousePosition( &xPos , &yPos );

  // printf( "Rel point %.2f %.2f\n", xPos,yPos);
  vc_point2 aPoint( xPos , yPos );

  // finds subdivisions
  if (!(sous_ech % 4))
    {
      GLubyte pixelColor_METISSON[3];
      glReadPixels( CurrentMousePos_x ,
		    CurrentWindow->getWindowHeight() - CurrentMousePos_y,
		    1, 1,
		    GL_RGB, GL_UNSIGNED_BYTE, pixelColor_METISSON );
      
      for( int indNode = 0 ; indNode < nbGeometricalObjects ; indNode++ ) {
	vc_SceneNode * node = tabGeometricalObjects[ indNode ];
	// mesh level intersection
	// interpolated meshes should be taken
	// into account in the future
	if( node->type == ASubdivision ) {
	  vc_Subdivision * subd = ((vc_Subdivision *)node->sceneObject);
	  vc_matrix44 * modelViewSubdivisionMatrix = node->getModelViewMatrix44();
	  // -------- Pour le mobile sonore ----------------------------
	  bool isIn = subd->MeshLevelInteraction( aPoint , modelViewSubdivisionMatrix, node, pixelColor_METISSON);
	  if (isIn)
	    {
	    for( int ind = 0 ; ind < environment->nb_IP_Clients ; ind++ ) {
		  // push udp message
	      if ((!strcmp(environment->IP_Clients[ ind ]->id, "udp_client1")))
		if( environment->IP_Clients[ ind ]->SocketToRemoteServer >= 0
		    && environment->IP_Clients[ ind ]->listen_sound_messages ) {
		  char cmdLine[XMLAttrChLength];
		  sprintf(cmdLine , "enter %s", node->id);
		  
		  node->inScenes = true;
		  environment->IP_Clients[ ind ]
		    ->storeIP_output_message( cmdLine );
		}
	    }
	    }


	  
	}
      }
    }
  sous_ech++;
}
#endif

void vc_Subdivision::initSubdivisionMesh( void ) {
  //   printf("update nbLevels %d\n" , nbLevels );
  // the surface is initialized
  // initialization
  for( int indVertex = indVerticesEnd[ 0 ] ; indVertex < nbMaxVertices ; indVertex++ ) {
    tabVertices[ indVertex ].init();
#ifdef VC_CRYSTAL_CAHEN
    tabDistances[ indVertex ] = 0.0;
#endif
  }
  for( int indTextureCoord = indTextureCoordsEnd[ 0 ] ; indTextureCoord < nbMaxTextureCoords ; indTextureCoord++ ) {
    tabTextureCoords[ indTextureCoord ].u = 0.0;
    tabTextureCoords[ indTextureCoord ].v = 0.0;
  }
  for( int indNormal = indNormalsEnd[ 0 ] ; indNormal < nbMaxNormals ; indNormal++ ) {
    tabNormals[ indNormal ].x = 0.0;
    tabNormals[ indNormal ].y = 0.0;
    tabNormals[ indNormal ].z = 0.0;
  }
  for( int indTangent = indTangentsEnd[ 0 ] ; indTangent < nbMaxTangents ; indTangent++ ) {
    tabTangents[ indTangent ].x = 0.0;
    tabTangents[ indTangent ].y = 0.0;
    tabTangents[ indTangent ].z = 0.0;
  }
//   for( int indColor = indColorsEnd[ 0 ] ; indColor < nbMaxColors ; indColor++ ) {
//     GLfloat * color = &tabColors[ indColor * 4 ];
//     for( int ind = 0 ; ind < 4 ; ind++ ) {
//       color[ ind ] = 1.0;
//     }
//   }
  for( int indFace = indFacesEnd[ 0  ] ; indFace < nbMaxFaces ; indFace++ ) {
    tabFaces[ indFace ].init();
  }
}

void vc_Subdivision::updateMesh( void ) {
  //   printf("update nbLevels %d\n" , nbLevels );
  // the surface is initialized
  int * tabConnectedFaces = new int[ max_degree ];
  int * tabConnectedFacesCopy = new int[ max_degree ];
  int * tabIndConnectedVerticesIni = new int[ max_degree ];
  int * tabIndConnectedVerticesEnd = new int[ max_degree ];
  int * tabIndConnectedVerticesCopy = new int[ max_degree ];
  if( !tabConnectedFaces || !tabConnectedFacesCopy || !tabIndConnectedVerticesIni || !tabIndConnectedVerticesEnd || !tabIndConnectedVerticesCopy ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }

  for( int indLevel = 1 ; indLevel < nbLevels ; indLevel++ ) {
    //printf("update %d\n",indLevel);
    subdividesAndModifies( indLevel ,
			   tabConnectedFaces ,
			   tabConnectedFacesCopy ,
			   tabIndConnectedVerticesIni ,
			   tabIndConnectedVerticesEnd ,
			   tabIndConnectedVerticesCopy );
  }
  delete [] tabConnectedFaces;
  tabConnectedFaces = NULL;

  delete [] tabConnectedFacesCopy;
  tabConnectedFacesCopy = NULL;

  delete [] tabIndConnectedVerticesIni;
  tabIndConnectedVerticesIni = NULL;

  delete [] tabIndConnectedVerticesEnd;
  tabIndConnectedVerticesEnd = NULL;

  delete [] tabIndConnectedVerticesCopy;
  tabIndConnectedVerticesCopy = NULL;
}

void vc_Subdivision::operator=(vc_Subdivision& p) {
  this->vc_GeometricalObject::operator=(p);

  if( bufferFaceNumber ) {
    bufferFaceNumber = p.bufferFaceNumber;
    delete [] pVerticesBuffer;
	pVerticesBuffer = NULL;

    delete [] pTexCoordsBuffer;
	pTexCoordsBuffer = NULL;

    delete [] pNormalsBuffer;
	pNormalsBuffer = NULL;

    delete [] pTangentsBuffer;
	pTangentsBuffer = NULL;

  }
  if( bufferFaceNumber ) {
    pVerticesBuffer = new float[ bufferFaceNumber * 3 * 3 ]; 
    if( !pVerticesBuffer ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int ind  = 0 ; ind < bufferFaceNumber * 3 * 3 ; ind++ ) {
      pVerticesBuffer[ind] = p.pVerticesBuffer[ind];
    }
    // Allocate Vertex Data
    // 2 coordinates
    pTexCoordsBuffer = new float[ bufferFaceNumber * 3 * 2 ]; 
    if( !pTexCoordsBuffer ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int ind  = 0 ; ind < bufferFaceNumber * 3 * 2 ; ind++ ) {
      pTexCoordsBuffer[ind] = p.pTexCoordsBuffer[ind];
    }

    // Allocate Normal Data
    pNormalsBuffer = new float[ bufferFaceNumber * 3 * 3 ]; 
    if( !pNormalsBuffer ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int ind  = 0 ; ind < bufferFaceNumber * 3 * 3 ; ind++ ) {
      pNormalsBuffer[ind] = p.pNormalsBuffer[ind];
    }

    // Allocate Tangent Data
    pTangentsBuffer = new float[ bufferFaceNumber * 3 * 3 ]; 
    if( !pTangentsBuffer ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int ind  = 0 ; ind < bufferFaceNumber * 3 * 3 ; ind++ ) {
      pTangentsBuffer[ind] = p.pTangentsBuffer[ind];
    }
  }
  else {
    pVerticesBuffer = NULL;
    pTexCoordsBuffer = NULL;
    pNormalsBuffer = NULL;
    pTangentsBuffer = NULL;
  }

  if (averageNormalFace) {
    delete [] averageNormalFace;
    averageNormalFace = NULL;
  }

  if (averageTangentFace) {
    delete [] averageTangentFace;
    averageTangentFace = NULL;
  }

  if (p.averageNormalFace)
  {
	  nbMaxFaces = p.nbMaxFaces;
	  averageNormalFace = new vc_vector3[nbMaxFaces];
	  for (int ind = 0; ind < nbMaxFaces; ind++)
	  {
		  averageNormalFace[ind] = p.averageNormalFace[ind];
	  }
  }
  //averageNormalFace = p.averageNormalFace;
  if (p.averageTangentFace)
  {
	  nbMaxFaces = p.nbMaxFaces;
	  averageTangentFace = new vc_vector3[nbMaxFaces];
	  for (int ind = 0; ind < nbMaxFaces; ind++)
	  {
		  averageTangentFace[ind] = p.averageTangentFace[ind];
	  }
  }
//  averageTangentFace = p.averageTangentFace;

  nbMaxVertices = p.nbMaxVertices;
  max_degree = p.max_degree;
  nbMaxTextureCoords = p.nbMaxTextureCoords;
  nbMaxNormals = p.nbMaxNormals;
  nbMaxTangents = p.nbMaxTangents;
//   nbMaxColors = p.nbMaxColors;
  //   maxArity = p.maxArity;
  nbMaxFaces = p.nbMaxFaces;
  nbLevels = p.nbLevels;
  shadowLevel = p.shadowLevel;
  flatShading = p.flatShading;
  reversedNormals = p.reversedNormals;

  if( tabVertices ) {
    delete [] tabVertices;
	tabVertices = NULL;
  }
  tabVertices = new vc_Vertex[ nbMaxVertices ];
  if( !tabVertices ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbMaxVertices ; ind++ ) {
    tabVertices[ind] = p.tabVertices[ind];
  }

#ifdef VC_CRYSTAL_CAHEN
  if( tabDistances ) {
    delete [] tabDistances;
	tabDistances = NULL;
  }
  tabDistances = new float[ nbMaxVertices ];
  if( !tabDistances ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbMaxVertices ; ind++ ) {
    tabDistances[ind] = p.tabDistances[ind];
  }
#endif

  if( tabTextureCoords ) {
    delete [] tabTextureCoords;
	tabTextureCoords = NULL;
  }
  tabTextureCoords = new vc_TextureCoord[ nbMaxTextureCoords ];
  if( !tabTextureCoords ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbMaxTextureCoords ; ind++ ) {
    tabTextureCoords[ind] = p.tabTextureCoords[ind];
  }

  if( tabNormals ) {
    delete [] tabNormals;
	tabNormals = NULL;
  }
  tabNormals = new vc_vector3[ nbMaxNormals ];
  if( !tabNormals ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbMaxNormals ; ind++ ) {
    tabNormals[ind] = p.tabNormals[ind];
  }

  if( tabTangents ) {
    delete [] tabTangents;
	tabTangents = NULL;
  }
  tabTangents = new vc_vector3[ nbMaxTangents ];
  if( !tabTangents ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbMaxTangents ; ind++ ) {
    tabTangents[ind] = p.tabTangents[ind];
  }
//   if( tabColors ) {
//     delete [] tabColors;
//   }
//   tabColors = new GLfloat[ nbMaxColors * 4 ];
//   if( !tabColors ) {
//     sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
//   }
//   for( int ind  = 0 ; ind < nbMaxColors * 4 ; ind++ ) {
//     tabColors[ind] = p.tabColors[ind];
//   }

  if( tabFaces ) {
    delete [] tabFaces;
	tabFaces = NULL;
  }
  tabFaces = new vc_Face[ nbMaxFaces ];
  if( !tabFaces ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbMaxFaces ; ind++ ) {
    tabFaces[ind] = p.tabFaces[ind];
  }

  if( indVerticesIni ) {
    delete [] indVerticesIni;
    indVerticesIni = NULL;
  }
  indVerticesIni = new int[ nbLevels ];
  if( !indVerticesIni ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indVerticesIni[ind] = p.indVerticesIni[ind];
  }

  if( indVerticesEnd ) {
    delete [] indVerticesEnd;
    indVerticesEnd = NULL;
  }
  indVerticesEnd = new int[ nbLevels ];
  if( !indVerticesEnd ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indVerticesEnd[ind] = p.indVerticesEnd[ind];
  }

  if( indTextureCoordsIni ) {
    delete [] indTextureCoordsIni;
    indTextureCoordsIni = NULL;
  }
  indTextureCoordsIni = new int[ nbLevels ];
  if( !indTextureCoordsIni ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indTextureCoordsIni[ind] = p.indTextureCoordsIni[ind];
  }

  if( indTextureCoordsEnd ) {
    delete [] indTextureCoordsEnd;
    indTextureCoordsEnd = NULL;
  }
  indTextureCoordsEnd = new int[ nbLevels ];
  if( !indTextureCoordsEnd ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indTextureCoordsEnd[ind] = p.indTextureCoordsEnd[ind];
  }

  if( indNormalsIni ) {
    delete [] indNormalsIni;
    indNormalsIni = NULL;
  }
  indNormalsIni = new int[ nbLevels ];
  if( !indNormalsIni ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indNormalsIni[ind] = p.indNormalsIni[ind];
  }

  if( indNormalsEnd ) {
    delete [] indNormalsEnd;
    indNormalsEnd = NULL;
  }
  indNormalsEnd = new int[ nbLevels ];
  if( !indNormalsEnd ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indNormalsEnd[ind] = p.indNormalsEnd[ind];
  }

  if( indTangentsIni ) {
    delete [] indTangentsIni;
    indTangentsIni = NULL;
  }
  indTangentsIni = new int[ nbLevels ];
  if( !indTangentsIni ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indTangentsIni[ind] = p.indTangentsIni[ind];
  }

  if( indTangentsEnd ) {
    delete [] indTangentsEnd;
    indTangentsEnd = NULL;
  }
  indTangentsEnd = new int[ nbLevels ];
  if( !indTangentsEnd ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indTangentsEnd[ind] = p.indTangentsEnd[ind];
  }

//   if( indColorsIni ) {
//     delete [] indColorsIni;
//   }
//   indColorsIni = new int[ nbLevels ];
//   if( !indColorsIni ) {
//     sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
//   }
//   for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
//     indColorsIni[ind] = p.indColorsIni[ind];
//   }

//   if( indColorsEnd ) {
//     delete [] indColorsEnd;
//   }
//   indColorsEnd = new int[ nbLevels ];
//   if( !indColorsEnd ) {
//     sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
//   }
//   for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
//     indColorsEnd[ind] = p.indColorsEnd[ind];
//   }

  if( indFacesIni ) {
    delete [] indFacesIni;
    indFacesIni = NULL;
  }
  indFacesIni = new int[ nbLevels ];
  if( !indFacesIni ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indFacesIni[ind] = p.indFacesIni[ind];
  }

  if( indFacesEnd ) {
    delete [] indFacesEnd;
    indFacesEnd = NULL;
  }
  indFacesEnd = new int[ nbLevels ];
  if( !indFacesEnd ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for( int ind  = 0 ; ind < nbLevels ; ind++ ) {
    indFacesEnd[ind] = p.indFacesEnd[ind];
  }

  keypointSize = p.keypointSize;
  keypointSize_1 = p.keypointSize_1;
  keypointSize_2 = p.keypointSize_2;
  nbKeypointWeightedVertices = p.nbKeypointWeightedVertices;
  keypointTransfs = p.keypointTransfs;
  keypointPrecedingTransfs = p.keypointPrecedingTransfs;
  keypointLoc = p.keypointLoc;
  keypoint_mask_channel1 = p.keypoint_mask_channel1;
  keypoint_mask_channel2 = p.keypoint_mask_channel2; 
  keypoint_mask_channel1_ID = p.keypoint_mask_channel1_ID;
  keypoint_mask_channel2_ID = p.keypoint_mask_channel2_ID; 
  indirectionTableKeypointIndices_1 = p.indirectionTableKeypointIndices_1;
  indirectionTableKeypointIndices_2 = p.indirectionTableKeypointIndices_2;
  
  subdivisionAlgorithm = p.subdivisionAlgorithm;
  subdivisionRenderingPrimitive = p.subdivisionRenderingPrimitive;
  coefLoop1=p.coefLoop1;
  coefLoop2=p.coefLoop2;
  coefMB1=p.coefMB1;
  coefMB2=p.coefMB2;
  coefMB3=p.coefMB3;

  autoscale = p.autoscale;
  autocenter = p.autocenter;
  normal_normalization = p.normal_normalization;
  tangent_normalization = p.tangent_normalization;
  gpu_KPanim = p.gpu_KPanim;
  scale = p.scale;
  tx = p.tx;
  ty = p.ty;
  tz = p.tz;
}


void vc_Subdivision::computeTabEdges( int level ) {
  //printf("computeTabEdges\n");
  //printf("level: %d\n",level);
  //printf("computeTabEdges\n");
  int indVertex1, indVertex2, indVertex3, edgeKey;
  int nbVertices = indVerticesEnd[level] - indVerticesIni[level];
  map <int,vc_Edge> mapEdges;
  map<int,vc_Edge>::iterator mapIter;

  // key for mapEdges
#define KEY(i,j) (i<j) ? j+nbVertices*i-i-1-(i*(i+1)/2) : i+nbVertices*j-j-1-(j*(j+1)/2);
  //printf("indFacesIni[level]: %d\n",indFacesIni[level]);
  //printf("indFacesEnd[level]: %d\n",indFacesEnd[level]);
  // find connected faces
  for (int indFace = indFacesIni[level] ; indFace<indFacesEnd[level] ; indFace++) {

    indVertex1 = tabFaces[indFace].indVertex1;
    indVertex2 = tabFaces[indFace].indVertex2;
    indVertex3 = tabFaces[indFace].indVertex3;

    // first edge
    edgeKey = KEY(indVertex1,indVertex2);
    mapIter = mapEdges.find(edgeKey);
    if (mapIter!=mapEdges.end())
      (mapEdges[edgeKey].indFace1 == -1) ? mapEdges[edgeKey].indFace1 = indFace : mapEdges[edgeKey].indFace2 = indFace;
    else {
      mapEdges[edgeKey].indVertex1 = indVertex1;
      mapEdges[edgeKey].indVertex2 = indVertex2;
      mapEdges[edgeKey].indFace1 = indFace;
      mapEdges[edgeKey].indFace2 = -1;
    }

    // second edge
    edgeKey = KEY(indVertex2,indVertex3);
    mapIter = mapEdges.find(edgeKey);
    if (mapIter!=mapEdges.end())
      (mapEdges[edgeKey].indFace1 == -1) ? mapEdges[edgeKey].indFace1 = indFace : mapEdges[edgeKey].indFace2 = indFace;
    else {
      mapEdges[edgeKey].indVertex1 = indVertex2;
      mapEdges[edgeKey].indVertex2 = indVertex3;
      mapEdges[edgeKey].indFace1 = indFace;
      mapEdges[edgeKey].indFace2 = -1;
    }


    // third edge
    edgeKey = KEY(indVertex3,indVertex1);
    mapIter = mapEdges.find(edgeKey);
    if (mapIter!=mapEdges.end())
      (mapEdges[edgeKey].indFace1 == -1) ? mapEdges[edgeKey].indFace1 = indFace : mapEdges[edgeKey].indFace2 = indFace;
    else {
      mapEdges[edgeKey].indVertex1 = indVertex3;
      mapEdges[edgeKey].indVertex2 = indVertex1;
      mapEdges[edgeKey].indFace1 = indFace;
      mapEdges[edgeKey].indFace2 = -1;
    }



  }

  indEdgesIni[level] = 0;
  indEdgesEnd[level] = mapEdges.size();
  //printf("level: %d\n",level);
  //printf("indEdgesEnd[level] = mapEdges.size(): %d\n",indEdgesEnd[level]);
  if (tabEdges != NULL)
  {
	  delete [] tabEdges;
	  tabEdges = NULL;
  }

  tabEdges = new vc_Edge[mapEdges.size()];
  if( !tabEdges ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  int indEdge = 0;
  for (mapIter = mapEdges.begin() ; mapIter!=mapEdges.end(); mapIter++) {
    tabEdges[indEdge++] = mapIter->second;
  }

  //  printf("dans computeTabEdges\n");
  //   for (int i = indEdgesIni[ nbLevels-1 ]
  // 	 ;  i<indEdgesEnd[ nbLevels-1 ] ; i++) {

  //       printf("indFace1: %d\n",tabEdges[i].indFace1+1);
  //       printf("indFace2: %d\n",tabEdges[i].indFace2+1);
  //       printf("indVertex1: %d\n",tabEdges[i].indVertex1+1);
  //       printf("indVertex2: %d\n",tabEdges[i].indVertex2+1);
  //       printf("\n");
  //   }

  //printf("nbLevel-1: %d\n",level-1);
  //printf("indEdgesIni: %d\n",indEdgesIni[level-1]);


}

void vc_Subdivision::computeAverageNormalFace( int level ) {

  //printf("computeAverageNormalFace\n");
  if (averageNormalFace != NULL)
  {
	  delete [] averageNormalFace;
	  averageNormalFace = NULL;
  }
  averageNormalFace = new vc_vector3[nbMaxFaces];
  if( !averageNormalFace ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for (int indFace = indFacesIni[level] ; indFace<indFacesEnd[level] ; indFace++) {

    averageNormalFace[indFace].update(0,0,0);
    averageNormalFace[indFace] += tabNormals[tabFaces[indFace].indNormal1];
    averageNormalFace[indFace] += tabNormals[tabFaces[indFace].indNormal2];
    averageNormalFace[indFace] += tabNormals[tabFaces[indFace].indNormal3];
    averageNormalFace[indFace].normalize();

  }

}

void vc_Subdivision::computeAverageTangentFace( int level ) {

  //printf("computeAverageTangentFace\n");
  if (averageTangentFace != NULL)
    delete [] averageTangentFace;
  averageTangentFace = NULL;
  averageTangentFace = new vc_vector3[nbMaxFaces];
  if( !averageTangentFace ) {
    sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
  }
  for (int indFace = indFacesIni[level] ; indFace<indFacesEnd[level] ; indFace++) {

    averageTangentFace[indFace].update(0,0,0);
    averageTangentFace[indFace] += tabTangents[tabFaces[indFace].indTangent1];
    averageTangentFace[indFace] += tabTangents[tabFaces[indFace].indTangent2];
    averageTangentFace[indFace] += tabTangents[tabFaces[indFace].indTangent3];
    averageTangentFace[indFace].normalize();

  }

}

void vc_Subdivision::orientFacesVertices( int level ) {
  //printf("orientFacesVertices\n");
  vc_vector3 averageNormalFaceTmp;
  int indTmp;

  for (int indFace = indFacesIni[level] ; indFace<indFacesEnd[level] ; indFace++) {

    tabFaces[indFace].normal( &averageNormalFaceTmp , tabVertices );

    if (averageNormalFaceTmp.prodScal(averageNormalFace[indFace]) > 0) {
      indTmp = tabFaces[indFace].indVertex3;
      tabFaces[indFace].indVertex3 = tabFaces[indFace].indVertex2;
      tabFaces[indFace].indVertex2 = indTmp;

      indTmp = tabFaces[indFace].indNormal3;
      tabFaces[indFace].indNormal3 = tabFaces[indFace].indNormal2;
      tabFaces[indFace].indNormal2 = indTmp;

//       indTmp = tabFaces[indFace].indColor3;
//       tabFaces[indFace].indColor3 = tabFaces[indFace].indColor2;
//       tabFaces[indFace].indColor2 = indTmp;

      indTmp = tabFaces[indFace].indTextureCoord3;
      tabFaces[indFace].indTextureCoord3 = tabFaces[indFace].indTextureCoord2;
      tabFaces[indFace].indTextureCoord2 = indTmp;
    }
  }
}
