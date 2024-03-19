echo off

REM cd C:\sync.com\Sync\LYM-projects\batFiles
REM tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
REM if errorlevel 1 START .\cavernePlaton.PD.bat

cd C:\home\LYM-projects\
python C:\home\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v2.py  --nb_configurations 1 --configuration Projects\configuration\LYM_Pierres-head-single_screen.csv --scenario Projects\scenarios\LYM_Pierres-scenario-v3.csv --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Core.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Core.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Core.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Core.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Core.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Core.cpp
REM copy C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Pierre.exe
C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe ./Projects/configuration/LYM_Pierres-head-single_screen.csv ./Projects/scenarios/LYM_Pierres-scenario-v3.csv
