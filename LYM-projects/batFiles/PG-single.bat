echo off

cd C:\home\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\home\LYM-projects\
python C:\home\LYM-projects\Projects\cmd\PG_source_generator.py  --nb_scenarios 1 --header ..\LYM-sources\Porphyrograph-src\include\pg-header.h --scenario Projects\scenarios\%1 --full_scenario Projects\scenarios\PG_full_scenario.csv --output_scenario Projects\scenarios\new_scenario.csv --script_header_out ..\LYM-sources\Porphyrograph-src\include\pg_script_header_Core.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Core.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\include\pg_shader_header_Core.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Core.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Core.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Core.cpp
C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe ./Projects/scenarios/%1
