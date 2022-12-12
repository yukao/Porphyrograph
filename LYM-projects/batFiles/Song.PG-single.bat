echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py  --configuration Projects\configuration\LYM_Song-head-single.csv --scenario Projects\scenarios\LYM_Song-scenario-v1.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_Song.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_Song_full.frag --Update_in Projects\shaders\LYM_Update_Song.frag --Update_out Projects\shaders\LYM_Update_Song_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Song.frag --Mixing_out Projects\shaders\LYM_Mixing_Song_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Song.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Song_full.frag --Master_in Projects\shaders\LYM_Master_Song.frag --Master_out Projects\shaders\LYM_Master_Song_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Song.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Song.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Song.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Song.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Song.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Song.cpp
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Song\Porphyrograph_core_Song.exe ./Projects/configuration/LYM_Song-head-single.csv ./Projects/scenarios/LYM_Song-scenario-v1.csv
