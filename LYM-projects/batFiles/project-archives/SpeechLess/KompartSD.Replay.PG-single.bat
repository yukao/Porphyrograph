echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq OSC_RecordReplay.exe" |find /i "OSC_RecordReplay.exe" > nul
if errorlevel 1 START .\OSC_replay.bat

cd D:\sync.com\Sync\LYM-projects\
REM perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_KompartSD-head-single.csv Projects\scenarios\LYM_KompartSD-scenarioReplay-v1.csv Projects\shaders\LYM_Update_KompartSD.frag Projects\shaders\LYM_Update_KompartSD_full.frag Projects\shaders\LYM_Mixing_KompartSD.frag Projects\shaders\LYM_Mixing_KompartSD_full.frag Projects\shaders\LYM_Master_KompartSD.frag Projects\shaders\LYM_Master_KompartSD_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_KompartSD.h ..\LYM-sources\Porphyrograph-src\pg_script_body_KompartSD.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_KompartSD.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_KompartSD.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_KompartSD.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_KompartSD.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_KompartSD\Porphyrograph_core_KompartSD.exe ./Projects/configuration/LYM_KompartSD-replay-head-single.csv ./Projects/scenarios/LYM_KompartSD-scenarioReplay-v1.csv
