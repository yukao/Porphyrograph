echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py  --configuration Projects\configuration\LYM_Light-SoundInitiative-head-single.csv --scenario Projects\scenarios\LYM_Light-SoundInitiative-scenario-v1.csv --Update_in Projects\shaders\LYM_Update_Light.frag --Update_out Projects\shaders\LYM_Update_Light_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Light.frag --Mixing_out Projects\shaders\LYM_Mixing_Light_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Light.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Light_full.frag --Master_in Projects\shaders\LYM_Master_Light.frag --Master_out Projects\shaders\LYM_Master_Light_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Light.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Light.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Light.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Light.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Light.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Light.cpp
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Light\Porphyrograph_core_Light.exe ./Projects/configuration/LYM_Light-SoundInitiative-head-single.csv ./Projects/scenarios/LYM_Light-SoundInitiative-scenario-v1.csv
