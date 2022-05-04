echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\cavernePlaton.PD.bat

cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py --configuration Projects\configuration\LYM_Pierres-head-single_screen.csv --scenario Projects\scenarios\LYM_Pierres-scenario-v2.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_Pierres.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_Pierres_full.frag --Update_in Projects\shaders\LYM_Update_Pierres.frag --Update_out Projects\shaders\LYM_Update_Pierres_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Pierres.frag --Mixing_out Projects\shaders\LYM_Mixing_Pierres_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Pierres.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Pierres_full.frag --Master_in Projects\shaders\LYM_Master_Pierres.frag --Master_out Projects\shaders\LYM_Master_Pierres_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Pierres.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Pierres.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Pierres.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Pierres.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Pierres.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Pierres.cpp
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Pierres\Porphyrograph_core_Pierres.exe ./Projects/configuration/LYM_Pierres-head-single_screen.csv ./Projects/scenarios/LYM_Pierres-scenario-v2.csv
