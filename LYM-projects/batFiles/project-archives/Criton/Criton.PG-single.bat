echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq porphyrograph-sound-JUCE.exe" |find /i "porphyrograph-sound-JUCE.exe" > nul
if errorlevel 1 START .\Criton.JUCE.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v3.prl Projects\configuration\LYM_Criton-head-single.csv Projects\scenarios\LYM_Criton-scenario-v1.csv Projects\shaders\LYM_Update_Criton.frag Projects\shaders\LYM_Update_Criton_full.frag Projects\shaders\LYM_Mixing_Criton.frag Projects\shaders\LYM_Mixing_Criton_full.frag Projects\shaders\LYM_Master_Criton.frag Projects\shaders\LYM_Master_Criton_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_Criton.h ..\LYM-sources\Porphyrograph-src\pg_script_body_Criton.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_Criton.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Criton.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Criton.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_Criton.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Criton\Porphyrograph_core_Criton.exe ./Projects/configuration/LYM_Criton-head-single.csv ./Projects/scenarios/LYM_Criton-scenario-v1.csv
