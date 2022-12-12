echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py  --configuration Projects\configuration\LYM_alKemi-head-single.csv --scenario Projects\scenarios\LYM_alKemi-scenario-v1.csv --Update_in Projects\shaders\LYM_Update_alKemi.frag --Update_out Projects\shaders\LYM_Update_alKemi_full.frag --Mixing_in Projects\shaders\LYM_Mixing_alKemi.frag --Mixing_out Projects\shaders\LYM_Mixing_alKemi_full.frag --Master_in Projects\shaders\LYM_Master_alKemi.frag --Master_out Projects\shaders\LYM_Master_alKemi_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_alKemi.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_alKemi.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_alKemi.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_alKemi.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_alKemi.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_alKemi.cpp
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_alKemi\Porphyrograph_core_alKemi.exe ./Projects/configuration/LYM_alKemi-head-single.csv ./Projects/scenarios/LYM_alKemi-scenario-v1.csv
