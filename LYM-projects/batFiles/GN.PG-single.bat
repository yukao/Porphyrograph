echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq Renoise.exe" |find /i "Renoise.exe" > nul
if errorlevel 1 START .\GN.renoise.bat

cd C:\sync.com\Sync\LYM-projects\
REM perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_GN-head-single.csv Projects\scenarios\LYM_GN-scenario-v1.csv Projects\shaders\LYM_Update_GN.frag Projects\shaders\LYM_Update_GN_full.frag Projects\shaders\LYM_Mixing_GN.frag Projects\shaders\LYM_Mixing_GN_full.frag Projects\shaders\LYM_Master_GN.frag Projects\shaders\LYM_Master_GN_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_GN.h ..\LYM-sources\Porphyrograph-src\pg_script_body_GN.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_GN.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_GN.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_GN.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_GN.cpp
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py  --configuration Projects\configuration\LYM_GN-head-single.csv --scenario Projects\scenarios\LYM_GN-scenario-v1.csv --Update_in Projects\shaders\LYM_Update_GN.frag --Update_out Projects\shaders\LYM_Update_GN_full.frag --Mixing_in Projects\shaders\LYM_Mixing_GN.frag --Mixing_out Projects\shaders\LYM_Mixing_GN_full.frag --Master_in Projects\shaders\LYM_Master_GN.frag --Master_out Projects\shaders\LYM_Master_GN_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_GN.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_GN.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_GN.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_GN.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_GN.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_GN.cpp
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_GN\Porphyrograph_core_GN.exe ./Projects/configuration/LYM_GN-head-single.csv ./Projects/scenarios/LYM_GN-scenario-v1.csv
