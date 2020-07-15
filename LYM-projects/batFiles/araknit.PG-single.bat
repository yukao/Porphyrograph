echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_araknit-head-single.csv Projects\scenarios\LYM_araknit-scenario-v1.csv Projects\shaders\LYM_ParticleAnimation_araknit.frag Projects\shaders\LYM_ParticleAnimation_araknit_full.frag Projects\shaders\LYM_Update_araknit.frag Projects\shaders\LYM_Update_araknit_full.frag Projects\shaders\LYM_Mixing_araknit.frag Projects\shaders\LYM_Mixing_araknit_full.frag Projects\shaders\LYM_ParticleSplat_araknit.frag Projects\shaders\LYM_ParticleSplat_araknit_full.frag Projects\shaders\LYM_Master_araknit.frag Projects\shaders\LYM_Master_araknit_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_araknit.h ..\LYM-sources\Porphyrograph-src\pg_script_body_araknit.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_araknit.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_araknit.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_araknit.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_araknit.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_araknit\Porphyrograph_core_araknit.exe ./Projects/configuration/LYM_araknit-head-single.csv ./Projects/scenarios/LYM_araknit-scenario-v1.csv
