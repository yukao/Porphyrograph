/*! \file pg-conf.h
 * 
 * 
* 
 *     File pg-conf.h
 * 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as publifscenesshed by the Free Software Foundation; either version 2.1
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

#ifndef PG_CONF_H
#define PG_CONF_H

enum InterpolationType{ pg_linear_interpolation = 0, pg_cosine_interpolation , pg_stepwise_interpolation , EmptyInterpolationType };

struct pg_Interpolation
{
  InterpolationType interpolation_mode;
  float offSet;
  float duration;
};

class pg_Window {
  public:
  /// glut ID for the window
  int                     glutID;

  pg_Window ();
  ~pg_Window();
};

extern int                      nb_scenes;
extern float                   *scene_durations;
extern float                   *scene_originalDurations;
extern float                   *scene_initial_times;
extern float                   *scene_originalInitial_times;
extern float                   *scene_final_times;
extern float                   *scene_originalFinal_times;
extern string                  *scene_IDs;
extern float                  **scene_initial_parameters;
extern float                  **scene_final_parameters;
extern pg_Interpolation       **scene_interpolations;

// PNG capture
extern string                    Png_file_name;
extern string                    Png_shot_dir_name;
extern int                      beginPng;
extern int                      endPng;
extern int                      stepPng;
extern bool                     outputPng;
// \}

// SVG capture
extern string                    Svg_file_name;
extern string                    Svg_shot_dir_name;
extern int                      beginSvg;
extern int                      endSvg;
extern int                      stepSvg;
extern bool                     outputSvg;

// JPG capture
extern string                    Jpg_file_name;
extern string                    Jpg_shot_dir_name;
extern int                      beginJpg;
extern int                      endJpg;
extern int                      stepJpg;
extern bool                     outputJpg;

// VIDEO capture
extern string                    Video_file_name;
extern int                      beginVideo_file;
extern int                      endVideo_file;
extern int                      stepVideo_file;
extern bool                     outputVideo_file;

// UDP servers and clients
extern pg_IPServer            **IP_Servers;
extern int                      nb_IP_Servers;
extern pg_IPClient            **IP_Clients;
extern int                      nb_IP_Clients;

extern string log_file_name            ;
extern string font_file_name           ;

/// the params of the font file
extern string                   font_file_encoding;
extern TextureEncoding          font_texture_encoding;
extern int                      font_size;

void saveInitialTimesAndDurations(void);
void restoreInitialTimesAndDurations( void );

void parseConfigurationFile( std::ifstream& confFin , std::ifstream&  scenarioFin );
void LoadConfigurationFile( const char * confFileName , const char * scenarioFileName );

#endif
