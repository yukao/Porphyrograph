echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\Foret.PD.bat

cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py  --configuration Projects\configuration\LYM_Foret-head-single.csv --scenario Projects\scenarios\LYM_Foret-scenario-v2.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_Foret.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_Foret_full.frag --Update_in Projects\shaders\LYM_Update_Foret.frag --Update_out Projects\shaders\LYM_Update_Foret_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Foret.frag --Mixing_out Projects\shaders\LYM_Mixing_Foret_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Foret.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Foret_full.frag --Master_in Projects\shaders\LYM_Master_Foret.frag --Master_out Projects\shaders\LYM_Master_Foret_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Foret.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Foret.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Foret.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Foret.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Foret.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Foret.cpp
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Foret\Porphyrograph_core_Foret.exe ./Projects/configuration/LYM_Foret-head-single.csv ./Projects/scenarios/LYM_Foret-scenario-v2.csv
