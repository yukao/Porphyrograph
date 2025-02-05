/*! \file pg-svg-clip-art.h
 *
 *
 *     File pg-svg-clip-art.h
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

 /* based on nvpr_init.h - initialize NVPR API */
 /* Copyright NVIDIA Corporation, 2010. */

#ifndef __PG_CLIPARTS_H__
#define __PG_CLIPARTS_H__

#if defined(__glew_h__)
#undef GLAPIENTRYP
#undef GLAPIENTRY
#endif


#ifdef  __cplusplus
extern "C" {
#endif

#ifndef GLAPIENTRY
# ifdef _WIN32
#  if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)  /* Mimic <windef.h> */
#   define GLAPIENTRY __stdcall
#  else
#   define GLAPIENTRY
#  endif
# else
#  define GLAPIENTRY
# endif
#endif

#ifndef GLAPI
# define GLAPI extern
#endif

#ifndef GLAPIENTRYP
# define GLAPIENTRYP GLAPIENTRY *
#endif

	/* NV_path_rendering */
#ifndef GL_NV_path_rendering
#define GL_NV_path_rendering 1
#if defined(GL_GLEXT_PROTOTYPES)
	GLAPI GLuint GLAPIENTRY glGenPathsNV(GLsizei range);
	GLAPI void GLAPIENTRY glDeletePathsNV(GLuint path, GLsizei range);
	GLAPI GLboolean GLAPIENTRY glIsPathNV(GLuint path);
	GLAPI void GLAPIENTRY glPathCommandsNV(GLuint path, GLsizei numCommands, const GLubyte* commands, GLsizei numCoords, GLenum coordType, const GLvoid* coords);
	GLAPI void GLAPIENTRY glPathCoordsNV(GLuint path, GLsizei numCoords, GLenum coordType, const GLvoid* coords);
	GLAPI void GLAPIENTRY glPathSubCommandsNV(GLuint path, GLsizei commandStart, GLsizei commandsToDelete, GLsizei numCommands, const GLubyte* commands, GLsizei numCoords, GLenum coordType, const GLvoid* coords);
	GLAPI void GLAPIENTRY glPathSubCoordsNV(GLuint path, GLsizei coordStart, GLsizei numCoords, GLenum coordType, const GLvoid* coords);
	GLAPI void GLAPIENTRY glPathStringNV(GLuint path, GLenum format, GLsizei length, const GLvoid* pathString);
	GLAPI void GLAPIENTRY glPathGlyphsNV(GLuint firstPathName, GLenum fontTarget, const GLvoid* fontName, GLbitfield fontStyle, GLsizei numGlyphs, GLenum type, const GLvoid* charcodes, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
	GLAPI void GLAPIENTRY glPathGlyphRangeNV(GLuint firstPathName, GLenum fontTarget, const GLvoid* fontName, GLbitfield fontStyle, GLuint firstGlyph, GLsizei numGlyphs, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
	GLAPI void GLAPIENTRY glWeightPathsNV(GLuint resultPath, GLsizei numPaths, const GLuint* paths, const GLfloat* weights);
	GLAPI void GLAPIENTRY glCopyPathNV(GLuint resultPath, GLuint srcPath);
	GLAPI void GLAPIENTRY glInterpolatePathsNV(GLuint resultPath, GLuint pathA, GLuint pathB, GLfloat weight);
	GLAPI void GLAPIENTRY glTransformPathNV(GLuint resultPath, GLuint srcPath, GLenum transformType, const GLfloat* transformValues);
	GLAPI void GLAPIENTRY glTransformPathNV(GLuint resultPath, GLuint srcPath, GLenum transformType, const GLfloat* transformValues);
	GLAPI void GLAPIENTRY glPathParameterivNV(GLuint path, GLenum pname, const GLint* value);
	GLAPI void GLAPIENTRY glPathParameteriNV(GLuint path, GLenum pname, GLint value);
	GLAPI void GLAPIENTRY glPathParameterfvNV(GLuint path, GLenum pname, const GLfloat* value);
	GLAPI void GLAPIENTRY glPathParameterfNV(GLuint path, GLenum pname, GLfloat value);
	GLAPI void GLAPIENTRY glPathDashArrayNV(GLuint path, GLsizei dashCount, const GLfloat* dashArray);
	GLAPI void GLAPIENTRY glPathStencilFuncNV(GLenum func, GLint ref, GLuint mask);
	GLAPI void GLAPIENTRY glPathStencilDepthOffsetNV(GLfloat factor, GLfloat units);
	GLAPI void GLAPIENTRY glStencilFillPathNV(GLuint path, GLenum fillMode, GLuint mask);
	GLAPI void GLAPIENTRY glStencilStrokePathNV(GLuint path, GLint reference, GLuint mask);
	GLAPI void GLAPIENTRY glStencilFillPathInstancedNV(GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum transformType, const GLfloat* transformValues);
	GLAPI void GLAPIENTRY glStencilStrokePathInstancedNV(GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLint reference, GLuint mask, GLenum transformType, const GLfloat* transformValues);
	GLAPI void GLAPIENTRY glPathCoverDepthFuncNV(GLenum func);
	GLAPI void GLAPIENTRY glPathColorGenNV(GLenum color, GLenum genMode, GLenum colorFormat, const GLfloat* coeffs);
	GLAPI void GLAPIENTRY glPathTexGenNV(GLenum texCoordSet, GLenum genMode, GLint components, const GLfloat* coeffs);
	GLAPI void GLAPIENTRY glPathFogGenNV(GLenum genMode);
	GLAPI void GLAPIENTRY glCoverFillPathNV(GLuint path, GLenum coverMode);
	GLAPI void GLAPIENTRY glCoverStrokePathNV(GLuint path, GLenum coverMode);
	GLAPI void GLAPIENTRY glCoverFillPathInstancedNV(GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat* transformValues);
	GLAPI void GLAPIENTRY glCoverStrokePathInstancedNV(GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat* transformValues);
	GLAPI void GLAPIENTRY glGetPathParameterivNV(GLuint path, GLenum pname, GLint* value);
	GLAPI void GLAPIENTRY glGetPathParameterfvNV(GLuint path, GLenum pname, GLfloat* value);
	GLAPI void GLAPIENTRY glGetPathCommandsNV(GLuint path, GLubyte* commands);
	GLAPI void GLAPIENTRY glGetPathCoordsNV(GLuint path, GLfloat* coords);
	GLAPI void GLAPIENTRY glGetPathDashArrayNV(GLuint path, GLfloat* dashArray);
	GLAPI void GLAPIENTRY glGetPathMetricsNV(GLbitfield metricQueryMask, GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLsizei stride, GLfloat* metrics);
	GLAPI void GLAPIENTRY glGetPathMetricRangeNV(GLbitfield metricQueryMask, GLuint firstPathName, GLsizei numPaths, GLsizei stride, GLfloat* metrics);
	GLAPI void GLAPIENTRY glGetPathSpacingNV(GLenum pathListMode, GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLfloat advanceScale, GLfloat kerningScale, GLenum transformType, GLfloat* returnedSpacing);
	GLAPI void GLAPIENTRY glGetPathColorGenivNV(GLenum color, GLenum pname, GLint* value);
	GLAPI void GLAPIENTRY glGetPathColorGenfvNV(GLenum color, GLenum pname, GLfloat* value);
	GLAPI void GLAPIENTRY glGetPathTexGenivNV(GLenum texCoordSet, GLenum pname, GLint* value);
	GLAPI void GLAPIENTRY glGetPathTexGenfvNV(GLenum texCoordSet, GLenum pname, GLfloat* value);
	GLAPI GLboolean GLAPIENTRY glIsPointInFillPathNV(GLuint path, GLuint mask, GLfloat x, GLfloat y);
	GLAPI GLboolean GLAPIENTRY glIsPointInStrokePathNV(GLuint path, GLfloat x, GLfloat y);
	GLAPI GLfloat GLAPIENTRY glGetPathLengthNV(GLuint path, GLsizei startSegment, GLsizei numSegments);
	GLAPI GLboolean GLAPIENTRY glPointAlongPathNV(GLuint path, GLsizei startSegment, GLsizei numSegments, GLfloat distance, GLfloat* x, GLfloat* y, GLfloat* tangentX, GLfloat* tangentY);
#endif /* GL_GLEXT_PROTOTYPES */
	typedef GLuint(GLAPIENTRYP PFNGLGENPATHSNVPROC) (GLsizei range);
	typedef void (GLAPIENTRYP PFNGLDELETEPATHSNVPROC) (GLuint path, GLsizei range);
	typedef GLboolean(GLAPIENTRYP PFNGLISPATHNVPROC) (GLuint path);
	typedef void (GLAPIENTRYP PFNGLPATHCOMMANDSNVPROC) (GLuint path, GLsizei numCommands, const GLubyte* commands, GLsizei numCoords, GLenum coordType, const GLvoid* coords);
	typedef void (GLAPIENTRYP PFNGLPATHCOORDSNVPROC) (GLuint path, GLsizei numCoords, GLenum coordType, const GLvoid* coords);
	typedef void (GLAPIENTRYP PFNGLPATHSUBCOMMANDSNVPROC) (GLuint path, GLsizei commandStart, GLsizei commandsToDelete, GLsizei numCommands, const GLubyte* commands, GLsizei numCoords, GLenum coordType, const GLvoid* coords);
	typedef void (GLAPIENTRYP PFNGLPATHSUBCOORDSNVPROC) (GLuint path, GLsizei coordStart, GLsizei numCoords, GLenum coordType, const GLvoid* coords);
	typedef void (GLAPIENTRYP PFNGLPATHSTRINGNVPROC) (GLuint path, GLenum format, GLsizei length, const GLvoid* pathString);
	typedef void (GLAPIENTRYP PFNGLPATHGLYPHSNVPROC) (GLuint firstPathName, GLenum fontTarget, const GLvoid* fontName, GLbitfield fontStyle, GLsizei numGlyphs, GLenum type, const GLvoid* charcodes, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
	typedef void (GLAPIENTRYP PFNGLPATHGLYPHRANGENVPROC) (GLuint firstPathName, GLenum fontTarget, const GLvoid* fontName, GLbitfield fontStyle, GLuint firstGlyph, GLsizei numGlyphs, GLenum handleMissingGlyphs, GLuint pathParameterTemplate, GLfloat emScale);
	typedef void (GLAPIENTRYP PFNGLWEIGHTPATHSNVPROC) (GLuint resultPath, GLsizei numPaths, const GLuint* paths, const GLfloat* weights);
	typedef void (GLAPIENTRYP PFNGLCOPYPATHNVPROC) (GLuint resultPath, GLuint srcPath);
	typedef void (GLAPIENTRYP PFNGLINTERPOLATEPATHSNVPROC) (GLuint resultPath, GLuint pathA, GLuint pathB, GLfloat weight);
	typedef void (GLAPIENTRYP PFNGLTRANSFORMPATHNVPROC) (GLuint resultPath, GLuint srcPath, GLenum transformType, const GLfloat* transformValues);
	typedef void (GLAPIENTRYP PFNGLPATHPARAMETERIVNVPROC) (GLuint path, GLenum pname, const GLint* value);
	typedef void (GLAPIENTRYP PFNGLPATHPARAMETERINVPROC) (GLuint path, GLenum pname, GLint value);
	typedef void (GLAPIENTRYP PFNGLPATHPARAMETERFVNVPROC) (GLuint path, GLenum pname, const GLfloat* value);
	typedef void (GLAPIENTRYP PFNGLPATHPARAMETERFNVPROC) (GLuint path, GLenum pname, GLfloat value);
	typedef void (GLAPIENTRYP PFNGLPATHDASHARRAYNVPROC) (GLuint path, GLsizei dashCount, const GLfloat* dashArray);
	typedef void (GLAPIENTRYP PFNGLPATHSTENCILFUNCNVPROC) (GLenum func, GLint ref, GLuint mask);
	typedef void (GLAPIENTRYP PFNGLPATHSTENCILDEPTHOFFSETNVPROC) (GLfloat factor, GLfloat units);
	typedef void (GLAPIENTRYP PFNGLSTENCILFILLPATHNVPROC) (GLuint path, GLenum fillMode, GLuint mask);
	typedef void (GLAPIENTRYP PFNGLSTENCILSTROKEPATHNVPROC) (GLuint path, GLint reference, GLuint mask);
	typedef void (GLAPIENTRYP PFNGLSTENCILFILLPATHINSTANCEDNVPROC) (GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLenum fillMode, GLuint mask, GLenum transformType, const GLfloat* transformValues);
	typedef void (GLAPIENTRYP PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC) (GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLint reference, GLuint mask, GLenum transformType, const GLfloat* transformValues);
	typedef void (GLAPIENTRYP PFNGLPATHCOVERDEPTHFUNCNVPROC) (GLenum func);
	typedef void (GLAPIENTRYP PFNGLPATHCOLORGENNVPROC) (GLenum color, GLenum genMode, GLenum colorFormat, const GLfloat* coeffs);
	typedef void (GLAPIENTRYP PFNGLPATHTEXGENNVPROC) (GLenum texCoordSet, GLenum genMode, GLint components, const GLfloat* coeffs);
	typedef void (GLAPIENTRYP PFNGLPATHFOGGENNVPROC) (GLenum genMode);
	typedef void (GLAPIENTRYP PFNGLCOVERFILLPATHNVPROC) (GLuint path, GLenum coverMode);
	typedef void (GLAPIENTRYP PFNGLCOVERSTROKEPATHNVPROC) (GLuint path, GLenum coverMode);
	typedef void (GLAPIENTRYP PFNGLCOVERFILLPATHINSTANCEDNVPROC) (GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat* transformValues);
	typedef void (GLAPIENTRYP PFNGLCOVERSTROKEPATHINSTANCEDNVPROC) (GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLenum coverMode, GLenum transformType, const GLfloat* transformValues);
	typedef void (GLAPIENTRYP PFNGLGETPATHPARAMETERIVNVPROC) (GLuint path, GLenum pname, GLint* value);
	typedef void (GLAPIENTRYP PFNGLGETPATHPARAMETERFVNVPROC) (GLuint path, GLenum pname, GLfloat* value);
	typedef void (GLAPIENTRYP PFNGLGETPATHCOMMANDSNVPROC) (GLuint path, GLubyte* commands);
	typedef void (GLAPIENTRYP PFNGLGETPATHCOORDSNVPROC) (GLuint path, GLfloat* coords);
	typedef void (GLAPIENTRYP PFNGLGETPATHDASHARRAYNVPROC) (GLuint path, GLfloat* dashArray);
	typedef void (GLAPIENTRYP PFNGLGETPATHMETRICSNVPROC) (GLbitfield metricQueryMask, GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLsizei stride, GLfloat* metrics);
	typedef void (GLAPIENTRYP PFNGLGETPATHMETRICRANGENVPROC) (GLbitfield metricQueryMask, GLuint firstPathName, GLsizei numPaths, GLsizei stride, GLfloat* metrics);
	typedef void (GLAPIENTRYP PFNGLGETPATHSPACINGNVPROC) (GLenum pathListMode, GLsizei numPaths, GLenum pathNameType, const GLvoid* paths, GLuint pathBase, GLfloat advanceScale, GLfloat kerningScale, GLenum transformType, GLfloat* returnedSpacing);
	typedef void (GLAPIENTRYP PFNGLGETPATHCOLORGENIVNVPROC) (GLenum color, GLenum pname, GLint* value);
	typedef void (GLAPIENTRYP PFNGLGETPATHCOLORGENFVNVPROC) (GLenum color, GLenum pname, GLfloat* value);
	typedef void (GLAPIENTRYP PFNGLGETPATHTEXGENIVNVPROC) (GLenum texCoordSet, GLenum pname, GLint* value);
	typedef void (GLAPIENTRYP PFNGLGETPATHTEXGENFVNVPROC) (GLenum texCoordSet, GLenum pname, GLfloat* value);
	typedef GLboolean(GLAPIENTRYP PFNGLISPOINTINFILLPATHNVPROC) (GLuint path, GLuint mask, GLfloat x, GLfloat y);
	typedef GLboolean(GLAPIENTRYP PFNGLISPOINTINSTROKEPATHNVPROC) (GLuint path, GLfloat x, GLfloat y);
	typedef GLfloat(GLAPIENTRYP PFNGLGETPATHLENGTHNVPROC) (GLuint path, GLsizei startSegment, GLsizei numSegments);
	typedef GLboolean(GLAPIENTRYP PFNGLPOINTALONGPATHNVPROC) (GLuint path, GLsizei startSegment, GLsizei numSegments, GLfloat distance, GLfloat* x, GLfloat* y, GLfloat* tangentX, GLfloat* tangentY);
	/* Tokens */
#define GL_CLOSE_PATH_NV                                    0x00
#define GL_MOVE_TO_NV                                       0x02
#define GL_RELATIVE_MOVE_TO_NV                              0x03
#define GL_LINE_TO_NV                                       0x04
#define GL_RELATIVE_LINE_TO_NV                              0x05
#define GL_HORIZONTAL_LINE_TO_NV                            0x06
#define GL_RELATIVE_HORIZONTAL_LINE_TO_NV                   0x07
#define GL_VERTICAL_LINE_TO_NV                              0x08
#define GL_RELATIVE_VERTICAL_LINE_TO_NV                     0x09
#define GL_QUADRATIC_CURVE_TO_NV                            0x0A
#define GL_RELATIVE_QUADRATIC_CURVE_TO_NV                   0x0B
#define GL_CUBIC_CURVE_TO_NV                                0x0C
#define GL_RELATIVE_CUBIC_CURVE_TO_NV                       0x0D
#define GL_SMOOTH_QUADRATIC_CURVE_TO_NV                     0x0E
#define GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV            0x0F
#define GL_SMOOTH_CUBIC_CURVE_TO_NV                         0x10
#define GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV                0x11
#define GL_SMALL_CCW_ARC_TO_NV                              0x12
#define GL_RELATIVE_SMALL_CCW_ARC_TO_NV                     0x13
#define GL_SMALL_CW_ARC_TO_NV                               0x14
#define GL_RELATIVE_SMALL_CW_ARC_TO_NV                      0x15
#define GL_LARGE_CCW_ARC_TO_NV                              0x16
#define GL_RELATIVE_LARGE_CCW_ARC_TO_NV                     0x17
#define GL_LARGE_CW_ARC_TO_NV                               0x18
#define GL_RELATIVE_LARGE_CW_ARC_TO_NV                      0x19
#define GL_CIRCULAR_CCW_ARC_TO_NV                           0xF8
#define GL_CIRCULAR_CW_ARC_TO_NV                            0xFA
#define GL_CIRCULAR_TANGENT_ARC_TO_NV                       0xFC
#define GL_ARC_TO_NV                                        0xFE
#define GL_RELATIVE_ARC_TO_NV                               0xFF
#define GL_PATH_FORMAT_ClipArt_NV                               0x9070
#define GL_PATH_FORMAT_PS_NV                                0x9071
#define GL_STANDARD_FONT_NAME_NV                            0x9072
#define GL_SYSTEM_FONT_NAME_NV                              0x9073
#define GL_FILE_NAME_NV                                     0x9074
#define GL_PATH_ClipArt_stroke_width_NV                             0x9075
#define GL_PATH_END_CAPS_NV                                 0x9076
#define GL_PATH_INITIAL_END_CAP_NV                          0x9077
#define GL_PATH_TERMINAL_END_CAP_NV                         0x9078
#define GL_PATH_JOIN_STYLE_NV                               0x9079
#define GL_PATH_MITER_LIMIT_NV                              0x907A
#define GL_PATH_DASH_CAPS_NV                                0x907B
#define GL_PATH_INITIAL_DASH_CAP_NV                         0x907C
#define GL_PATH_TERMINAL_DASH_CAP_NV                        0x907D
#define GL_PATH_DASH_OFFSET_NV                              0x907E
#define GL_PATH_CLIENT_LENGTH_NV                            0x907F
#define GL_PATH_FILL_MODE_NV                                0x9080
#define GL_PATH_FILL_MASK_NV                                0x9081
#define GL_PATH_FILL_COVER_MODE_NV                          0x9082
#define GL_PATH_STROKE_COVER_MODE_NV                        0x9083
#define GL_PATH_STROKE_MASK_NV                              0x9084
#define GL_PATH_SAMPLE_QUALITY_NV                           0x9085
#define GL_COUNT_UP_NV                                      0x9088
#define GL_COUNT_DOWN_NV                                    0x9089
#define GL_PATH_OBJECT_BOUNDING_BOX_NV                      0x908A
#define GL_CONVEX_HULL_NV                                   0x908B
#define GL_BOUNDING_BOX_NV                                  0x908D
#define GL_TRANSLATE_X_NV                                   0x908E
#define GL_TRANSLATE_Y_NV                                   0x908F
#define GL_TRANSLATE_2D_NV                                  0x9090
#define GL_TRANSLATE_3D_NV                                  0x9091
#define GL_AFFINE_2D_NV                                     0x9092
#define GL_AFFINE_3D_NV                                     0x9094
#define GL_TRANSPOSE_AFFINE_2D_NV                           0x9096
#define GL_TRANSPOSE_AFFINE_3D_NV                           0x9098
#define GL_UTF8_NV                                          0x909A
#define GL_UTF16_NV                                         0x909B
#define GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV                0x909C
#define GL_PATH_COMMAND_COUNT_NV                            0x909D
#define GL_PATH_COORD_COUNT_NV                              0x909E
#define GL_PATH_DASH_ARRAY_COUNT_NV                         0x909F
#define GL_PATH_COMPUTED_LENGTH_NV                          0x90A0
#define GL_PATH_FILL_BOUNDING_BOX_NV                        0x90A1
#define GL_PATH_STROKE_BOUNDING_BOX_NV                      0x90A2
#define GL_SQUARE_NV                                        0x90A3
#define GL_ROUND_NV                                         0x90A4
#define GL_TRIANGULAR_NV                                    0x90A5
#define GL_BEVEL_NV                                         0x90A6
#define GL_MITER_REVERT_NV                                  0x90A7
#define GL_MITER_TRUNCATE_NV                                0x90A8
#define GL_SKIP_MISSING_GLYPH_NV                            0x90A9
#define GL_USE_MISSING_GLYPH_NV                             0x90AA
#define GL_PATH_DASH_OFFSET_RESET_NV                        0x90B4
#define GL_MOVE_TO_RESETS_NV                                0x90B5
#define GL_MOVE_TO_CONTINUES_NV                             0x90B6
#define GL_BOLD_BIT_NV                                      0x01
#define GL_ITALIC_BIT_NV                                    0x02
#define GL_PATH_ERROR_POSITION_NV                           0x90AB
#define GL_PATH_FOG_GEN_MODE_NV                             0x90AC
#define GL_GLYPH_WIDTH_BIT_NV                               0x01
#define GL_GLYPH_HEIGHT_BIT_NV                              0x02
#define GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV                0x04
#define GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV                0x08
#define GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV          0x10
#define GL_GLYPH_VERTICAL_BEARING_X_BIT_NV                  0x20
#define GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV                  0x40
#define GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV            0x80
#define GL_GLYPH_HAS_KERNING_NV                             0x100
#define GL_FONT_X_MIN_BOUNDS_NV                             0x00010000
#define GL_FONT_Y_MIN_BOUNDS_NV                             0x00020000
#define GL_FONT_X_MAX_BOUNDS_NV                             0x00040000
#define GL_FONT_Y_MAX_BOUNDS_NV                             0x00080000
#define GL_FONT_UNITS_PER_EM_NV                             0x00100000
#define GL_FONT_ASCENDER_NV                                 0x00200000
#define GL_FONT_DESCENDER_NV                                0x00400000
#define GL_FONT_HEIGHT_NV                                   0x00800000
#define GL_FONT_MAX_ADVANCE_WIDTH_NV                        0x01000000
#define GL_FONT_MAX_ADVANCE_HEIGHT_NV                       0x02000000
#define GL_FONT_UNDERLINE_POSITION_NV                       0x04000000
#define GL_FONT_UNDERLINE_THICKNESS_NV                      0x08000000
#define GL_FONT_HAS_KERNING_NV                              0x10000000
#define GL_ACCUM_ADJACENT_PAIRS_NV                          0x90AD
#define GL_ADJACENT_PAIRS_NV                                0x90AE
#define GL_FIRST_TO_REST_NV                                 0x90AF
#define GL_PATH_GEN_MODE_NV                                 0x90B0
#define GL_PATH_GEN_COEFF_NV                                0x90B1
#define GL_PATH_GEN_COLOR_FORMAT_NV                         0x90B2
#define GL_PATH_GEN_COMPONENTS_NV                           0x90B3
#define GL_PATH_STENCIL_FUNC_NV                             0x90B7
#define GL_PATH_STENCIL_REF_NV                              0x90B8
#define GL_PATH_STENCIL_VALUE_MASK_NV                       0x90B9
#define GL_PATH_STENCIL_DEPTH_OFFSET_FACTOR_NV              0x90BD
#define GL_PATH_STENCIL_DEPTH_OFFSET_UNITS_NV               0x90BE
#define GL_PATH_COVER_DEPTH_FUNC_NV                         0x90BF
#endif

#ifndef __APPLE__

#if defined(sun)
#define FUNC(x) x##_
#else
#define FUNC(x) x
#endif

/* NV_path_rendering */
#ifndef __GLEW_H__
	extern PFNGLGENPATHSNVPROC FUNC(glGenPathsNV);
	extern PFNGLDELETEPATHSNVPROC FUNC(glDeletePathsNV);
	extern PFNGLISPATHNVPROC FUNC(glIsPathNV);
	extern PFNGLPATHCOMMANDSNVPROC FUNC(glPathCommandsNV);
	extern PFNGLPATHCOORDSNVPROC FUNC(glPathCoordsNV);
	extern PFNGLPATHSUBCOMMANDSNVPROC FUNC(glPathSubCommandsNV);
	extern PFNGLPATHSUBCOORDSNVPROC FUNC(glPathSubCoordsNV);
	extern PFNGLPATHSTRINGNVPROC FUNC(glPathStringNV);
	extern PFNGLPATHGLYPHSNVPROC FUNC(glPathGlyphsNV);
	extern PFNGLPATHGLYPHRANGENVPROC FUNC(glPathGlyphRangeNV);
	extern PFNGLWEIGHTPATHSNVPROC FUNC(glWeightPathsNV);
	extern PFNGLCOPYPATHNVPROC FUNC(glCopyPathNV);
	extern PFNGLINTERPOLATEPATHSNVPROC FUNC(glInterpolatePathsNV);
	extern PFNGLPATHPARAMETERIVNVPROC FUNC(glPathParameterivNV);
	extern PFNGLPATHPARAMETERINVPROC FUNC(glPathParameteriNV);
	extern PFNGLPATHPARAMETERFVNVPROC FUNC(glPathParameterfvNV);
	extern PFNGLPATHPARAMETERFNVPROC FUNC(glPathParameterfNV);
	extern PFNGLPATHDASHARRAYNVPROC FUNC(glPathDashArrayNV);
	extern PFNGLPATHSTENCILFUNCNVPROC FUNC(glPathStencilFuncNV);
	extern PFNGLSTENCILFILLPATHNVPROC FUNC(glStencilFillPathNV);
	extern PFNGLSTENCILSTROKEPATHNVPROC FUNC(glStencilStrokePathNV);
	extern PFNGLSTENCILFILLPATHINSTANCEDNVPROC FUNC(glStencilFillPathInstancedNV);
	extern PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC FUNC(glStencilStrokePathInstancedNV);
	extern PFNGLPATHCOLORGENNVPROC FUNC(glPathColorGenNV);
	extern PFNGLPATHTEXGENNVPROC FUNC(glPathTexGenNV);
	extern PFNGLPATHFOGGENNVPROC FUNC(glPathFogGenNV);
	extern PFNGLCOVERFILLPATHNVPROC FUNC(glCoverFillPathNV);
	extern PFNGLCOVERSTROKEPATHNVPROC FUNC(glCoverStrokePathNV);
	extern PFNGLCOVERFILLPATHINSTANCEDNVPROC FUNC(glCoverFillPathInstancedNV);
	extern PFNGLCOVERSTROKEPATHINSTANCEDNVPROC FUNC(glCoverStrokePathInstancedNV);
	extern PFNGLGETPATHPARAMETERIVNVPROC FUNC(glGetPathParameterivNV);
	extern PFNGLGETPATHPARAMETERFVNVPROC FUNC(glGetPathParameterfvNV);
	extern PFNGLGETPATHCOMMANDSNVPROC FUNC(glGetPathCommandsNV);
	extern PFNGLGETPATHCOORDSNVPROC FUNC(glGetPathCoordsNV);
	extern PFNGLGETPATHDASHARRAYNVPROC FUNC(glGetPathDashArrayNV);
	extern PFNGLGETPATHMETRICSNVPROC FUNC(glGetPathMetricsNV);
	extern PFNGLGETPATHMETRICRANGENVPROC FUNC(glGetPathMetricRangeNV);
	extern PFNGLGETPATHSPACINGNVPROC FUNC(glGetPathSpacingNV);
	extern PFNGLGETPATHCOLORGENIVNVPROC FUNC(glGetPathColorGenivNV);
	extern PFNGLGETPATHCOLORGENFVNVPROC FUNC(glGetPathColorGenfvNV);
	extern PFNGLGETPATHTEXGENIVNVPROC FUNC(glGetPathTexGenivNV);
	extern PFNGLGETPATHTEXGENFVNVPROC FUNC(glGetPathTexGenfvNV);
	extern PFNGLISPOINTINFILLPATHNVPROC FUNC(glIsPointInFillPathNV);
	extern PFNGLISPOINTINSTROKEPATHNVPROC FUNC(glIsPointInStrokePathNV);
	extern PFNGLGETPATHLENGTHNVPROC FUNC(glGetPathLengthNV);
	extern PFNGLPOINTALONGPATHNVPROC FUNC(glPointAlongPathNV);
	extern PFNGLPATHSTENCILDEPTHOFFSETNVPROC FUNC(glPathStencilDepthOffsetNV);
	extern PFNGLPATHCOVERDEPTHFUNCNVPROC FUNC(glPathCoverDepthFuncNV);
#endif // __GLEW_H__
#endif /* __APPLE__ */

#if defined(sun) && !defined(NO_NVPR_API_REMAP)
#define glGenPathsNV glGenPathsNV_
#define glDeletePathsNV glDeletePathsNV_
#define glIsPathNV glIsPathNV_
#define glPathCommandsNV glPathCommandsNV_
#define glPathCoordsNV glPathCoordsNV_
#define glPathSubCommandsNV glPathSubCommandsNV_
#define glPathSubCoordsNV glPathSubCoordsNV_
#define glPathStringNV glPathStringNV_
#define glPathGlyphsNV glPathGlyphsNV_
#define glPathGlyphRangeNV glPathGlyphRangeNV_
#define glWeightPathsNV glWeightPathsNV_
#define glCopyPathNV glCopyPathNV_
#define glInterpolatePathsNV glInterpolatePathsNV_
#define glPathParameterivNV glPathParameterivNV_
#define glPathParameteriNV glPathParameteriNV_
#define glPathParameterfvNV glPathParameterfvNV_
#define glPathParameterfNV glPathParameterfNV_
#define glPathDashArrayNV glPathDashArrayNV_
#define glPathStencilFuncNV glPathStencilFuncNV_
#define glStencilFillPathNV glStencilFillPathNV_
#define glStencilStrokePathNV glStencilStrokePathNV_
#define glStencilFillPathInstancedNV glStencilFillPathInstancedNV_
#define glStencilStrokePathInstancedNV glStencilStrokePathInstancedNV_
#define glPathColorGenNV glPathColorGenNV_
#define glPathTexGenNV glPathTexGenNV_
#define glPathFogGenNV glPathFogGenNV_
#define glCoverFillPathNV glCoverFillPathNV_
#define glCoverStrokePathNV glCoverStrokePathNV_
#define glCoverFillPathInstancedNV glCoverFillPathInstancedNV_
#define glCoverStrokePathInstancedNV glCoverStrokePathInstancedNV_
#define glGetPathParameterivNV glGetPathParameterivNV_
#define glGetPathParameterfvNV glGetPathParameterfvNV_
#define glGetPathCommandsNV glGetPathCommandsNV_
#define glGetPathCoordsNV glGetPathCoordsNV_
#define glGetPathDashArrayNV glGetPathDashArrayNV_
#define glGetPathMetricsNV glGetPathMetricsNV_
#define glGetPathMetricRangeNV glGetPathMetricRangeNV_
#define glGetPathSpacingNV glGetPathSpacingNV_
#define glGetPathColorGenivNV glGetPathColorGenivNV_
#define glGetPathColorGenfvNV glGetPathColorGenfvNV_
#define glGetPathTexGenivNV glGetPathTexGenivNV_
#define glGetPathTexGenfvNV glGetPathTexGenfvNV_
#define glIsPointInFillPathNV glIsPointInFillPathNV_
#define glIsPointInStrokePathNV glIsPointInStrokePathNV_
#define glGetPathLengthNV glGetPathLengthNV_
#define glPointAlongPathNV glPointAlongPathNV_
#define glPathStencilDepthOffsetNV glPathStencilDepthOffsetNV_
#define glPathCoverDepthFuncNV glPathCoverDepthFuncNV_
#endif
#ifdef  __cplusplus
}
#endif

// ClipArt GPU
// color
enum pg_ClipArt_Colors_Types { ClipArt_nat = 0, ClipArt_white, ClipArt_red, ClipArt_green, ClipArt_blue, ClipArt_yellow, ClipArt_cyan, ClipArt_magenta, ClipArt_black };

class ClipArt {
public:
	// number of paths for each file
	int pg_nb_paths_in_ClipArt;
	// file names
	string pg_ClipArt_fileNames;
	// geometrical transformations
	float pg_ClipArt_Scale;
	float pg_ClipArt_Rotation;
	float pg_ClipArt_Translation_X;
	float pg_ClipArt_Translation_Y;
	// color
	pg_ClipArt_Colors_Types* pg_ClipArt_Colors;
	// subpath display
	bool* pg_ClipArt_SubPath;
	// base ID of the GPU files
	GLuint ClipArt_file_baseID;
	ClipArt(void) {
		// number of paths for each file
		pg_nb_paths_in_ClipArt = 0;
		// file names
		pg_ClipArt_fileNames = "";
		// geometrical transformations
		pg_ClipArt_Scale = 1.f;
		pg_ClipArt_Rotation = 0.f;
		pg_ClipArt_Translation_X = 0.f;
		pg_ClipArt_Translation_Y = 0.f;
		// color
		pg_ClipArt_Colors = NULL;
		// subpath display
		pg_ClipArt_SubPath = NULL;
		// base ID of the GPU files
		ClipArt_file_baseID = NULL_ID;
	}
	~ClipArt(void) {
	}
};
// last activated CLipArt
extern ClipArt* pg_last_activated_ClipArt;

extern vector<ClipArt>pg_ClipArts[PG_MAX_CONFIGURATIONS];
// total number of paths
extern int pg_nb_tot_SvgGpu_paths[PG_MAX_CONFIGURATIONS];
// lastisClearAllLayersctivated SvgGpu
extern ClipArt* pg_last_activated_ClipArt;
// base ID of the GPU paths
extern GLuint ClipArt_path_baseID[PG_MAX_CONFIGURATIONS];
// fill color table
extern unsigned int** ClipArt_path_fill_color[PG_MAX_CONFIGURATIONS];


extern bool has_NV_path_rendering;

extern bool ClipArt_stroke_onOff;
extern bool ClipArt_fill_onOff;
extern float ClipArt_fill_color_shift;
extern float ClipArt_stroke_width;
extern int ClipArt_fill_color_swap;
extern float ClipArt_fill_color_brightness;
extern int ClipArt_stroke_color_swap;

// display speed
extern float				interframe_delay;

void initializeNVPathRender(void);
void LoadClipArtPathsToGPU(string fileName, int nb_gpu_paths, int indClipArtFile, int indConfiguration);
void pg_Display_All_ClipArt(int activeFiles);
float pg_Translate_ClipArt_Text(int indDisplayText);
void pg_Display_ClipArt_Text(int* indActiveDisplayText, int mobile);
void pg_loadAllClipArts(void);
void pg_listAllClipArts(void);
#endif /* __PG_CLIPARTS_H__ */

