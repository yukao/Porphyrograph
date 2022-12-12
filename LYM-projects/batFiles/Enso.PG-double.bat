echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\cavernePlaton.PD.bat

cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py --configuration Projects\configuration\LYM_Enso-head-double.csv --scenario Projects\scenarios\LYM_Enso-scenario-v1.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_Enso.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_Enso_full.frag --Update_in Projects\shaders\LYM_Update_Enso.frag --Update_out Projects\shaders\LYM_Update_Enso_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Enso.frag --Mixing_out Projects\shaders\LYM_Mixing_Enso_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Enso.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Enso_full.frag --Master_in Projects\shaders\LYM_Master_Enso.frag --Master_out Projects\shaders\LYM_Master_Enso_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Enso.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Enso.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Enso.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Enso.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Enso.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Enso.cpp
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Enso\Porphyrograph_core_Enso.exe ./Projects/configuration/LYM_Enso-head-double.csv ./Projects/scenarios/LYM_Enso-scenario-v1.csv
