////////////////////////////////////////////////////////////////////
// 
// 
//     File pg-FX.h
// 
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#ifndef PG_FX_H
#define PG_FX_H

////////////////////////////////////////////////////////////////////
// FX ON OFF
////////////////////////////////////////////////////////////////////
//extern float sampleFXMix;
//extern float lineInFXMix;

////////////////////////////////////////////////////////////////////
// VST DATA
////////////////////////////////////////////////////////////////////
extern AEffect* myPlugin;
extern MVerb<float> VST_reverb_samples;
extern mdaDelay VST_delay_samples;
extern MVerb<float> VST_reverb_inLine;
extern mdaDelay VST_delay_inLine;

////////////////////////////////////////////////////////////////////
// VST PLUG-INS DATA
////////////////////////////////////////////////////////////////////

// Plugin's entry point
typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);
// Plugin's dispatcher function
typedef VstIntPtr(*dispatcherFuncPtr)(AEffect *effect, VstInt32 opCode,
	VstInt32 index, VstInt32 value, void *ptr, float opt);
// Plugin's getParameter() method
typedef float(*getParameterFuncPtr)(AEffect *effect, VstInt32 index);
// Plugin's setParameter() method
typedef void(*setParameterFuncPtr)(AEffect *effect, VstInt32 index, float value);
// Plugin's processEvents() method
typedef VstInt32(*processEventsFuncPtr)(VstEvents *events);
// Plugin's process() method
typedef void(*processFuncPtr)(AEffect *effect, float **inputs,
	float **outputs, VstInt32 sampleFrames);

AEffect* loadVSTPlugin(char *vstPath);
bool configureVSTPlugin(AEffect *plugin, dispatcherFuncPtr *dispatcher);
bool canPluginDo(char *canDoString, AEffect *plugin, dispatcherFuncPtr dispatcher);
void resumePlugin(AEffect *plugin, dispatcherFuncPtr dispatcher);
void suspendPlugin(AEffect *plugin, dispatcherFuncPtr dispatcher);
void initVSTPlugin(AEffect *plugin, dispatcherFuncPtr dispatcher);
bool pg_VST_Sample_processor(string *inputFileName, string *outputFileName, AEffect* myPlugin);
void process_file_plugin(void);
void initial_values_FX( void );

#endif
