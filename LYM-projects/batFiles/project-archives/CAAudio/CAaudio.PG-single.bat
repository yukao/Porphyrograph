echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\CAaudio.PD.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_CAaudio-head-single.csv Projects\scenarios\LYM_CAaudio-scenario-v1.csv Projects\shaders\LYM_ParticleAnimation_CAaudio.frag Projects\shaders\LYM_ParticleAnimation_CAaudio_full.frag Projects\shaders\LYM_Update_CAaudio.frag Projects\shaders\LYM_Update_CAaudio_full.frag Projects\shaders\LYM_Mixing_CAaudio.frag Projects\shaders\LYM_Mixing_CAaudio_full.frag Projects\shaders\LYM_ParticleSplat_CAaudio.frag Projects\shaders\LYM_ParticleSplat_CAaudio_full.frag Projects\shaders\LYM_Master_CAaudio.frag Projects\shaders\LYM_Master_CAaudio_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_CAaudio.h ..\LYM-sources\Porphyrograph-src\pg_script_body_CAaudio.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_CAaudio.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_CAaudio.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_CAaudio.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_CAaudio.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_CAaudio\Porphyrograph_core_CAaudio.exe ./Projects/configuration/LYM_CAaudio-head-single.csv ./Projects/scenarios/LYM_CAaudio-scenario-v1.csv
