echo off

cd C:\sync.com\Sync\LYM-projects\batFiles

tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

tasklist /fi "imagename eq Renoise.exe" |find /i "Renoise.exe" > nul
if errorlevel 1 START .\GN.renoise.bat

cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v2.py --nb_configurations 1 --configuration Projects\configuration\LYM_GN-head-single.csv --scenario Projects\scenarios\LYM_GN-scenario-FermeBlanche-v1.csv --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Core.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Core.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Core.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Core.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Core.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Core.cpp
REM copy C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Core\GN.exe
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Core\GN.exe ./Projects/configuration/LYM_GN-head-single.csv ./Projects/scenarios/LYM_GN-scenario-FermeBlanche-v1.csv