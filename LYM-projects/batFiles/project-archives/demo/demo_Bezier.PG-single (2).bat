echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_demo_Bezier-head-single.csv Projects\scenarios\LYM_demo-Bezier-scenario-v1.csv Projects\shaders\LYM_ParticleAnimation_demo.frag Projects\shaders\LYM_ParticleAnimation_demo_full.frag Projects\shaders\LYM_Update_demo_Bezier.frag Projects\shaders\LYM_Update_demo_Bezier_full.frag Projects\shaders\LYM_Mixing_demo.frag Projects\shaders\LYM_Mixing_demo_full.frag Projects\shaders\LYM_ParticleSplat_demo.frag Projects\shaders\LYM_ParticleSplat_demo_full.frag Projects\shaders\LYM_Master_demo.frag Projects\shaders\LYM_Master_demo_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_demo.h ..\LYM-sources\Porphyrograph-src\pg_script_body_demo.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_demo.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_demo.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_demo.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_demo.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_demo_Bezier\Porphyrograph_core_demo_Bezier.exe ./Projects/configuration/LYM_demo_Bezier-head-single.csv ./Projects/scenarios/LYM_demo-Bezier-scenario-v1.csv
