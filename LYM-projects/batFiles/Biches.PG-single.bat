echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
python D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py --configuration Projects\configuration\LYM_Biches-head-single.csv --scenario Projects\scenarios\LYM_Biches-scenario-v1.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_Biches.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_Biches_full.frag --Update_in Projects\shaders\LYM_Update_Biches.frag --Update_out Projects\shaders\LYM_Update_Biches_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Biches.frag --Mixing_out Projects\shaders\LYM_Mixing_Biches_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Biches.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Biches_full.frag --Master_in Projects\shaders\LYM_Master_Biches.frag --Master_out Projects\shaders\LYM_Master_Biches_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Biches.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Biches.cpp --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Biches.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Biches.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Biches.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Biches.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Biches\Porphyrograph_core_Biches.exe ./Projects/configuration/LYM_Biches-head-single.csv ./Projects/scenarios/LYM_Biches-scenario-v1.csv
