/*! \file pg-init.h
*
*
*     File pg-init.h
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
/*
based on Francesco Caruso GLSLParametricCurves
https://github.com/fcaruso/GLSLParametricCurve
*/

#ifndef PG_INIT_H
#define PG_INIT_H


#define PG_SIZE_QUAD_ARRAY 4  // quads made of a 4-point triangle strip

#if PG_NB_TRACKS == 1
enum pg_FBO_Update_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_sampler = 0,
#ifdef ALKEMI
	pg_PreviousCA_FBO_Update_sampler,
#endif
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler,
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) || defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler,
#endif
#else
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler = 0,
#endif
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler = 0,
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler,
#endif
#else
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler = 0,
#endif
#endif
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_Camera_frame_Update_sampler,
	pg_Camera_BG_Update_sampler,
#endif
	pg_Movie_frame_Update_sampler,
	pg_Noise_Update_sampler,
#ifdef PG_WITH_REPOP_DENSITY
	pg_RepopDensity_Update_sampler,
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
	pg_Photo0_Update_sampler,
	pg_Photo1_Update_sampler,
#endif
#if defined(PG_NB_PARALLEL_CLIPS) && PG_NB_PARALLEL_CLIPS >= 2
	pg_Clip0_Update_sampler,
	pg_Clip1_Update_sampler,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Update_sampler,
#endif
#if defined (TVW)
	pg_TVWPixels0,
	pg_TVWPixels1,
	pg_TVWPixels2,
	pg_TVWPixels3,
	pg_TVWPixels4,
	pg_TVWPixels5,
	pg_TVWMask02,
	pg_TVWMask35,
	pg_TVWPixelsSwap0,
	pg_TVWPixelsSwap1,
	pg_TVWPixelsSwap2,
	pg_TVWPixelsSwap3,
	pg_TVWPixelsSwap4,
	pg_TVWPixelsSwap5,
#endif
	pg_Trk0_FBO_Update_sampler,
#if defined (GN) || defined (CAAUDIO)
	pg_CATable_Update_sampler,
#endif
#ifdef GN
	pg_Camera_BGIni_FBO_Update_sampler,
#endif
#ifdef WITH_BURST_MASK
	pg_Burst_Mask_FBO_Update_sampler,
#endif
};
enum pg_FBO_Update_attachments
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_attcht = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht = 0,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#endif
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Mixing_sampler = 0,
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler,
#endif
	pg_Render_prec_FBO_Mixing_sampler,
#else
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler = 0,
	pg_Render_prec_FBO_Mixing_sampler,
#else
	pg_Render_prec_FBO_Mixing_sampler = 0,
#endif
#endif
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
#if defined (TVW)
	pg_Display_Font_FBO_Mixing_sampler,
	pg_Display_Message1_FBO_Mixing_sampler,
	pg_Display_Message2_FBO_Mixing_sampler,
#endif
	pg_Trk0_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Master_sampler,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Master_sampler,
#endif
	pg_Trk0_FBO_Master_sampler,
#if defined (PG_WITH_MASTER_MASK)
	pg_Mask_FBO_Master_sampler,
#endif
#if defined (GN) || defined (INTERFERENCE)
	pg_LYMlogo_Master_sampler,
#endif
};
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_ParticleAnimation_sampler,
	pg_Part_acc_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_ParticleAnimation_sampler,
#ifdef PG_WITH_REPOP_DENSITY
	pg_RepopDensity_ParticleAnimation_sampler,
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_Camera_frame_ParticleAnimation_sampler,
#endif
	pg_Movie_frame_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
};
#endif
#endif

#if PG_NB_TRACKS == 2
enum pg_FBO_Update_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_sampler = 0,
#ifdef ALKEMI
	pg_PreviousCA_FBO_Update_sampler,
#endif
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler,
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler,
#endif
#else
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler = 0,
#endif
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler = 0,
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler,
#endif
#else
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler = 0,
#endif
#endif
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_Camera_frame_Update_sampler,
	pg_Camera_BG_Update_sampler,
#endif
	pg_Movie_frame_Update_sampler,
	pg_Noise_Update_sampler,
#ifdef PG_WITH_REPOP_DENSITY
	pg_RepopDensity_Update_sampler,
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
	pg_Photo0_Update_sampler,
	pg_Photo1_Update_sampler,
#endif
#if defined(PG_NB_PARALLEL_CLIPS) && PG_NB_PARALLEL_CLIPS >= 2
	pg_Clip0_Update_sampler,
	pg_Clip1_Update_sampler,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Update_sampler,
#endif
#if defined (TVW)
	pg_TVWPixels0,
	pg_TVWPixels1,
	pg_TVWPixels2,
	pg_TVWPixels3,
	pg_TVWPixels4,
	pg_TVWPixels5,
	pg_TVWMask02,
	pg_TVWMask35,
	pg_TVWPixelsSwap0,
	pg_TVWPixelsSwap1,
	pg_TVWPixelsSwap2,
	pg_TVWPixelsSwap3,
	pg_TVWPixelsSwap4,
	pg_TVWPixelsSwap5,
#endif
	pg_Trk0_FBO_Update_sampler,
	pg_Trk1_FBO_Update_sampler,
#ifdef WITH_BURST_MASK
	pg_Burst_Mask_FBO_Update_sampler,
#endif
};
enum pg_FBO_Update_attachments
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_attcht = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht = 0,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#endif
	pg_Trk1_FBO_Update_attcht,
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Mixing_sampler = 0,
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler,
#endif
	pg_Render_prec_FBO_Mixing_sampler,
#else
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler = 0,
	pg_Render_prec_FBO_Mixing_sampler,
#else
	pg_Render_prec_FBO_Mixing_sampler = 0,
#endif
#endif
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
#if defined (TVW)
	pg_Display_Font_FBO_Mixing_sampler,
	pg_Display_Message1_FBO_Mixing_sampler,
	pg_Display_Message2_FBO_Mixing_sampler,
#endif
	pg_Trk0_FBO_Mixing_sampler,
	pg_Trk1_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Master_sampler,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Master_sampler,
#endif
	pg_Trk0_FBO_Master_sampler,
	pg_Trk1_FBO_Master_sampler,
#if defined (PG_WITH_MASTER_MASK)
	pg_Mask_FBO_Master_sampler,
#endif
#if defined (GN) || defined (INTERFERENCE)
	pg_LYMlogo_Master_sampler,
#endif
};
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_ParticleAnimation_sampler,
	pg_Part_acc_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_ParticleAnimation_sampler,
#ifdef PG_WITH_REPOP_DENSITY
	pg_RepopDensity_ParticleAnimation_sampler,
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_Camera_frame_ParticleAnimation_sampler,
#endif
	pg_Movie_frame_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
	pg_Trk1_FBO_ParticleAnimation_sampler,
};
#endif
#endif

#if PG_NB_TRACKS == 3
enum pg_FBO_Update_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_sampler = 0,
#ifdef ALKEMI
	pg_PreviousCA_FBO_Update_sampler,
#endif
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler,
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler,
#endif
#else
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler = 0,
#endif
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler = 0,
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler,
#endif
#else
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler = 0,
#endif
#endif
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_Camera_frame_Update_sampler,
	pg_Camera_BG_Update_sampler,
#endif
	pg_Movie_frame_Update_sampler,
	pg_Noise_Update_sampler,
#ifdef PG_WITH_REPOP_DENSITY
	pg_RepopDensity_Update_sampler,
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
	pg_Photo0_Update_sampler,
	pg_Photo1_Update_sampler,
#endif
#if defined(PG_NB_PARALLEL_CLIPS) && PG_NB_PARALLEL_CLIPS >= 2
	pg_Clip0_Update_sampler,
	pg_Clip1_Update_sampler,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Update_sampler,
#endif
	pg_Trk0_FBO_Update_sampler,
	pg_Trk1_FBO_Update_sampler,
	pg_Trk2_FBO_Update_sampler,
#if defined (RIVETS)
	pg_CATable_Update_sampler,
#endif
#ifdef PG_WITH_BURST_MASK
	pg_Burst_Mask_FBO_Update_sampler,
#endif
};
enum pg_FBO_Update_attachments
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_attcht = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht = 0,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#endif
	pg_Trk1_FBO_Update_attcht,
	pg_Trk2_FBO_Update_attcht,
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Mixing_sampler = 0,
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler,
#endif
	pg_Render_prec_FBO_Mixing_sampler,
#else
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler = 0,
	pg_Render_prec_FBO_Mixing_sampler,
#else
	pg_Render_prec_FBO_Mixing_sampler = 0,
#endif
#endif
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
#if defined (TVW)
	pg_Display_Font_FBO_Mixing_sampler,
	pg_Display_Message1_FBO_Mixing_sampler,
	pg_Display_Message2_FBO_Mixing_sampler,
#endif
	pg_Trk0_FBO_Mixing_sampler,
	pg_Trk1_FBO_Mixing_sampler,
	pg_Trk2_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Master_sampler,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Master_sampler,
#endif
	pg_Trk0_FBO_Master_sampler,
	pg_Trk1_FBO_Master_sampler,
	pg_Trk2_FBO_Master_sampler,
#if defined (PG_WITH_MASTER_MASK)
	pg_Mask_FBO_Master_sampler,
#endif
#if defined (GN) || defined (INTERFERENCE)
	pg_LYMlogo_Master_sampler,
#endif
};
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_ParticleAnimation_sampler,
	pg_Part_acc_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_ParticleAnimation_sampler,
#ifdef PG_WITH_REPOP_DENSITY
	pg_RepopDensity_ParticleAnimation_sampler,
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_Camera_frame_ParticleAnimation_sampler,
#endif
	pg_Movie_frame_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
	pg_Trk1_FBO_ParticleAnimation_sampler,
	pg_Trk2_FBO_ParticleAnimation_sampler,
};
#endif
#endif

#if PG_NB_TRACKS == 4
enum pg_FBO_Update_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_sampler = 0,
#ifdef ALKEMI
	pg_PreviousCA_FBO_Update_sampler,
#endif
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler,
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler,
#endif
#else
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler = 0,
#endif
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_sampler = 0,
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler,
#endif
#else
#if !defined (PG_BEZIER_PATHS) || defined(PIERRES) || defined (ENSO) ||defined(SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(ALKEMI) || defined(ATELIERSENFANTS) || defined(CORE)
	pg_Brushes_Update_sampler = 0,
#endif
#endif
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_Camera_frame_Update_sampler,
	pg_Camera_BG_Update_sampler,
#endif
	pg_Movie_frame_Update_sampler,
	pg_Noise_Update_sampler,
#ifdef PG_WITH_REPOP_DENSITY
	pg_RepopDensity_Update_sampler,
#endif
#ifdef PG_WITH_PHOTO_DIAPORAMA
	pg_Photo0_Update_sampler,
	pg_Photo1_Update_sampler,
#endif
#if defined(PG_NB_PARALLEL_CLIPS) && PG_NB_PARALLEL_CLIPS >= 2
	pg_Clip0_Update_sampler,
	pg_Clip1_Update_sampler,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Update_sampler,
#endif
	pg_Trk0_FBO_Update_sampler,
	pg_Trk1_FBO_Update_sampler,
	pg_Trk2_FBO_Update_sampler,
	pg_Trk3_FBO_Update_sampler,
#ifdef WITH_BURST_MASK
	pg_Burst_Mask_FBO_Update_sampler,
#endif
};
enum pg_FBO_Update_attachments
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Update_attcht = 0,
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#else
#ifdef PG_NB_PIXEL_MODES
	pg_Pixels_FBO_Update_attcht = 0,
	pg_Trk0_FBO_Update_attcht,
#else
	pg_Trk0_FBO_Update_attcht = 0,
#endif
#endif
	pg_Trk1_FBO_Update_attcht,
	pg_Trk2_FBO_Update_attcht,
	pg_Trk3_FBO_Update_attcht,
	PG_FBO_UPDATE_NBATTACHTS
};
enum pg_FBO_Mixing_samplers
{
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Mixing_sampler = 0,
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler,
#endif
	pg_Render_prec_FBO_Mixing_sampler,
#else
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Mixing_sampler = 0,
	pg_Render_prec_FBO_Mixing_sampler,
#else
	pg_Render_prec_FBO_Mixing_sampler = 0,
#endif
#endif
	pg_Screen_Font_FBO_Mixing_sampler,
	pg_Screen_Message_FBO_Mixing_sampler,
#if defined (TVW)
	pg_Display_Font_FBO_Mixing_sampler,
	pg_Display_Message1_FBO_Mixing_sampler,
	pg_Display_Message2_FBO_Mixing_sampler,
#endif
	pg_Trk0_FBO_Mixing_sampler,
	pg_Trk1_FBO_Mixing_sampler,
	pg_Trk2_FBO_Mixing_sampler,
	pg_Trk3_FBO_Mixing_sampler,
};
enum pg_FBO_Master_samplers
{
	pg_Render_curr_FBO_Master_sampler = 0,
#ifdef PG_NB_CA_TYPES
	pg_CA_FBO_Master_sampler,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
	pg_Part_render_FBO_Master_sampler,
#endif
	pg_Trk0_FBO_Master_sampler,
	pg_Trk1_FBO_Master_sampler,
	pg_Trk2_FBO_Master_sampler,
	pg_Trk3_FBO_Master_sampler,
#if defined (PG_WITH_MASTER_MASK)
	pg_Mask_FBO_Master_sampler,
#endif
};
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 

enum pg_FBO_ParticleAnimation_samplers
{
	pg_Part_init_pos_speed_ParticleAnimation_sampler = 0,
	pg_Part_init_col_rad_ParticleAnimation_sampler,
	pg_Part_acc_ParticleAnimation_sampler,
	pg_CA_FBO_ParticleAnimation_sampler,
	pg_Part_pos_speed_FBO_ParticleAnimation_sampler,
	pg_Part_col_rad_FBO_ParticleAnimation_sampler,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_sampler,
	pg_Noise_ParticleAnimation_sampler,
#ifdef PG_WITH_REPOP_DENSITY
	pg_RepopDensity_ParticleAnimation_sampler,
#endif
#ifdef PG_WITH_CAMERA_CAPTURE
	pg_Camera_frame_ParticleAnimation_sampler,
#endif
	pg_Movie_frame_ParticleAnimation_sampler,
	pg_Trk0_FBO_ParticleAnimation_sampler,
	pg_Trk1_FBO_ParticleAnimation_sampler,
	pg_Trk2_FBO_ParticleAnimation_sampler,
	pg_Trk3_FBO_ParticleAnimation_sampler,
};
#endif
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 

enum pg_FBO_ParticleAnimation_attachments
{
	pg_Part_pos_speed_FBO_ParticleAnimation_attcht = 0,
	pg_Part_col_rad_FBO_ParticleAnimation_attcht,
	pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht,
	PG_FBO_PARTICLEANIMATION_NBATTACHTS
};
#endif

enum eVertexArrayObject
{
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 

	pg_VAOParticle,
#endif
	pg_VAOQuad,
	pg_VAOQuadMaster,
	pg_VAOCount
};

enum eVertexBufferObject
{
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 

	pg_VBOParticle,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES)
	pg_VBOParticleTexCoords,
	pg_VBOpartRadius,
	pg_VBOParticleColors,
#endif
#if defined CURVE_PARTICLES
	pg_VBOParticleTexCoords,
	pg_VBOpartRadius,
	pg_VBOParticleColors,
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 

	pg_EAOParticle,
#endif
	pg_EABQuad,
	pg_EABQuadMaster,
	pg_VBOCount
};

/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
//////////////////////////////////////////////////////////////////////
extern GLfloat pg_orthoWindowProjMatrix[16];
extern GLfloat doubleProjMatrix[16];
extern GLfloat pg_identityViewMatrix[16];
extern GLfloat pg_identityModelMatrix[16];
#if defined (PIERRES)
extern GLfloat pg_homographyForTexture[9];
#endif
#ifdef PG_SENSORS
extern GLfloat modelMatrixSensor[16];
#endif

/////////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////////////
extern GLuint pg_vaoID[pg_VAOCount];
extern GLuint pg_vboID[pg_VBOCount];
#ifdef PG_MIRRORED_SECOND_SCREEN
extern GLuint pg_Master_vaoID[pg_VAOCount];
extern GLuint pg_Master_vboID[pg_VBOCount];
#endif
#ifdef PG_SENSORS
extern unsigned int quadSensor_vao;
#endif
#ifdef PG_MESHES
extern unsigned int **mesh_vao;
extern vector <vector <string>> mesh_IDs; 
extern int *nbMeshesPerMeshFile;
extern int **nbFacesPerMesh;
extern unsigned int **mesh_index_vbo;
#endif

// geometrical data
extern float quadDraw_points[];
extern float quadDraw_texCoords[];
extern unsigned int quadDraw_indices[PG_SIZE_QUAD_ARRAY];

// geometrical data
extern float quadMaster_points[];
extern float quadMaster_texCoords[];
extern unsigned int quadMaster_indices[PG_SIZE_QUAD_ARRAY];

// particle curves
#ifdef CURVE_PARTICLES
extern GLfloat *pg_Particle_control_points;
extern GLfloat *pg_Particle_radius;
extern GLfloat *pg_Particle_colors;
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES)
extern GLfloat *pg_Particle_vertices;
extern GLfloat *pg_Particle_radius;
extern GLfloat *pg_Particle_colors;
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 

extern unsigned int *pg_Particle_indices;
#endif

#ifdef PG_SENSORS
//////////////////////////////////////////////////////////////////////
// SENSORS
//////////////////////////////////////////////////////////////////////

// sensor translations
// current sensor layout
extern float sensorPositions[3 * PG_NB_SENSORS];
// all possible sensor layouts
extern float sensorLayouts[3 * PG_NB_SENSORS * PG_NB_MAX_SENSOR_LAYOUTS];

// sensor on off
// current sensor activation pattern
extern bool sensor_onOff[PG_NB_SENSORS];
extern double sensor_last_activation_time;
// all sensor activation patterns
extern bool sensorActivations[PG_NB_SENSORS * PG_NB_MAX_SENSOR_ACTIVATIONS];

// sample choice
// current sample choice
extern int sample_choice[PG_NB_SENSORS];
// all possible sensor layouts
extern int sensor_sample_setUps[PG_NB_MAX_SAMPLE_SETUPS][PG_NB_SENSORS];
// sample start playing time for muting after 1 cycle
extern double sample_play_start[PG_NB_MAX_SAMPLE_SETUPS * PG_NB_SENSORS];
#endif

// +++++++++++++++++++++++ Metawear sensors ++++++++++++++++++++
#ifdef PG_METAWEAR
#define PG_MW_NB_MAX_SENSORS 2
struct metawear_sensor_data {
	float mw_mss_pos[3];		// x y z
	bool  mw_mss_pos_update;
};
extern struct metawear_sensor_data pg_mw_sensors[PG_MW_NB_MAX_SENSORS];
#endif


/////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
extern GLuint drawBuffers[16];

/////////////////////////////////////////////
// FBO 
extern GLuint pg_FBO_Update[PG_FBO_PINGPONG_SIZE]; // update
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 

extern GLuint pg_FBO_ParticleAnimation[2]; // particle animation
extern GLuint pg_FBO_ParticleRendering; // particle rendering
extern GLuint pg_FBO_ParticleAnimation_texID[2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS]; // particle animation
extern GLuint pg_FBO_ParticleRendering_texID; // particle rendering
#endif
extern GLuint pg_FBO_Mixing_capturedFB_prec[2]; //  drawing memory on odd and even frames for echo
// extern GLuint FBO_CameraFrame; //  video preprocessing outcome
#ifdef PG_AUGMENTED_REALITY
extern GLuint pg_FBO_Master_capturedFB_prec; // master output memory for mapping on mesh  
#endif

// FBO texture
extern GLuint pg_FBO_Update_texID[PG_FBO_PINGPONG_SIZE * PG_FBO_UPDATE_NBATTACHTS]; // update 
extern GLuint pg_FBO_Mixing_capturedFB_prec_texID[2]; // drawing memory on odd and even frames for echo 
// extern GLuint FBO_CameraFrame_texID; // video preprocessing outcome 
#ifdef PG_AUGMENTED_REALITY
extern GLuint pg_FBO_Master_capturedFB_prec_texID; // master output memory for mapping on mesh  
#endif

/////////////////////////////////////////////////////////////////
// TEXTURES
/////////////////////////////////////////////////////////////////
//extern GLuint pg_Particle_Pos_Texture_texID;
//extern GLfloat *pg_Particle_Pos_Texture;

#ifdef CURVE_PARTICLES
// comet texture
extern GLuint comet_texture_2D_texID;
#endif
#if defined (TEXTURED_QUAD_PARTICLES)
// blurred disk texture
extern GLuint blurredDisk_texture_2D_texID;
#endif


//////////////////
// FUNCTIONS
// Perlin Noise
double noise(double x, double y, double z);

// opengl error
int printOglError(int no);
// error report
void ReportError(char *errorString);

// current directory
std::string GetCurrentWorkingDir(void);

// time management
double RealTime(void);
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp);
#endif

void MemoryUsage(void);

void InterfaceInitializations(void);

void pg_initGeometry_quads(void);
bool pg_initFBOTextures(GLuint *textureID, int nb_attachments, bool with_stencil_andOr_depth);
bool pg_initFBO(void);
void pg_initRenderingMatrices(void);

void pg_screenMessage_update(void);
#if defined (TVW) || defined (ETOILES)
bool pg_ReadAllDisplayMessages(string dir, string basefilename);
int pg_displayMessage_update(int indMesg);
#endif

void pg_initParticlePosition_Texture(void);

#ifdef PG_SENSORS
void SensorInitialization(void);
void assignSensorPositions(void);
void assignSensorActivations(void);
void sensor_sample_setUp_interpolation(void);
#endif

#ifdef PG_METAWEAR
void MetawearSensorInitialization();
#endif

#ifdef PG_MESHES
void MeshInitialization(void);
#endif

#endif
