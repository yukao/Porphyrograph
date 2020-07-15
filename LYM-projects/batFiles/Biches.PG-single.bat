echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_Biches-head-single.csv Projects\scenarios\LYM_Biches-scenario-v1.csv Projects\shaders\LYM_ParticleAnimation_Biches.frag Projects\shaders\LYM_ParticleAnimation_Biches_full.frag Projects\shaders\LYM_Update_Biches.frag Projects\shaders\LYM_Update_Biches_full.frag Projects\shaders\LYM_Mixing_Biches.frag Projects\shaders\LYM_Mixing_Biches_full.frag Projects\shaders\LYM_ParticleSplat_Biches.frag Projects\shaders\LYM_ParticleSplat_Biches_full.frag Projects\shaders\LYM_Master_Biches.frag Projects\shaders\LYM_Master_Biches_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_Biches.h ..\LYM-sources\Porphyrograph-src\pg_script_body_Biches.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_Biches.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Biches.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Biches.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_Biches.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Biches\Porphyrograph_core_Biches.exe ./Projects/configuration/LYM_Biches-head-single.csv ./Projects/scenarios/LYM_Biches-scenario-v1.csv
