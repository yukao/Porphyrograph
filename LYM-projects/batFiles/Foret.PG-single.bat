echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
python D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py --configuration Projects\configuration\LYM_SONG-head-single.csv --scenario Projects\scenarios\LYM_Foret-scenario-v1.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_SONG.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_SONG_full.frag --Update_in Projects\shaders\LYM_Update_SONG.frag --Update_out Projects\shaders\LYM_Update_SONG_full.frag --Mixing_in Projects\shaders\LYM_Mixing_SONG.frag --Mixing_out Projects\shaders\LYM_Mixing_SONG_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_SONG.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_SONG_full.frag --Master_in Projects\shaders\LYM_Master_SONG.frag --Master_out Projects\shaders\LYM_Master_SONG_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_SONG.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_SONG.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_SONG.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_SONG.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_SONG.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_SONG.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_SONG\Porphyrograph_core_SONG.exe ./Projects/configuration/LYM_SONG-head-single.csv ./Projects/scenarios/LYM_Foret-scenario-v1.csv
