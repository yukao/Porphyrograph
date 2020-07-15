echo off

chdir /d D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START D:\sync.com\Sync\LYM-projects\batFiles\core.PD.bat

chdir /d D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_Tempete-head-single.csv Projects\scenarios\LYM_Tempete-scenario-v1.csv Projects\shaders\LYM_ParticleAnimation_demo.frag Projects\shaders\LYM_ParticleAnimation_demo_full.frag Projects\shaders\LYM_Update_Tempete.frag Projects\shaders\LYM_Update_Tempete_full.frag Projects\shaders\LYM_Mixing_Tempete.frag Projects\shaders\LYM_Mixing_Tempete_full.frag Projects\shaders\LYM_ParticleSplat_demo.frag Projects\shaders\LYM_ParticleSplat_demo_full.frag Projects\shaders\LYM_Master_Tempete.frag Projects\shaders\LYM_Master_Tempete_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_Tempete.h ..\LYM-sources\Porphyrograph-src\pg_script_body_Tempete.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_Tempete.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Tempete.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Tempete.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_Tempete.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Tempete\Porphyrograph_core_Tempete.exe ./Projects/configuration/LYM_Tempete-head-single.csv ./Projects/scenarios/LYM_Tempete-scenario-v1.csv
