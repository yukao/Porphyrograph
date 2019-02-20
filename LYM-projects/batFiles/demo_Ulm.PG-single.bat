echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v2.prl Projects\configuration\LYM_demo_Ulm-head-single.txt Projects\scenarios\LYM_demo-Ulm-scenario-v1.txt Projects\shaders\LYM_ParticleAnimation_demo.frag Projects\shaders\LYM_ParticleAnimation_demo_full.frag Projects\shaders\LYM_Update_Ulm.frag Projects\shaders\LYM_Update_Ulm_full.frag Projects\shaders\LYM_Mixing_Ulm.frag Projects\shaders\LYM_Mixing_Ulm_full.frag Projects\shaders\LYM_ParticleSplat_Ulm.frag Projects\shaders\LYM_ParticleSplat_Ulm_full.frag Projects\shaders\LYM_Master_Ulm.frag Projects\shaders\LYM_Master_Ulm_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_Ulm.h ..\LYM-sources\Porphyrograph-src\pg_script_body_Ulm.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_Ulm.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Ulm.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Ulm.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_Ulm.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Ulm\Porphyrograph_core_Ulm.exe ./Projects/configuration/LYM_demo_Ulm-head-single.txt ./Projects/scenarios/LYM_demo-Ulm-scenario-v1.txt
