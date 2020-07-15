echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v2.prl Projects\configuration\LYM_demo-CavernePlaton-head-single.txt Projects\scenarios\LYM_demo-CavernePlaton-scenario-v1.txt Projects\shaders\LYM_ParticleAnimation_demo.frag Projects\shaders\LYM_ParticleAnimation_demo_full.frag Projects\shaders\LYM_Update_CavernePlaton.frag Projects\shaders\LYM_Update_CavernePlaton_full.frag Projects\shaders\LYM_Mixing_CavernePlaton.frag Projects\shaders\LYM_Mixing_CavernePlaton_full.frag Projects\shaders\LYM_ParticleSplat_CavernePlaton.frag Projects\shaders\LYM_ParticleSplat_CavernePlaton_full.frag Projects\shaders\LYM_Master_CavernePlaton.frag Projects\shaders\LYM_Master_CavernePlaton_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_CavernePlaton.h ..\LYM-sources\Porphyrograph-src\pg_script_body_CavernePlaton.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_CavernePlaton.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_CavernePlaton.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_CavernePlaton.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_CavernePlaton.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_CavernePlaton\Porphyrograph_core_CavernePlaton.exe ./Projects/configuration/LYM_demo-CavernePlaton-head-single.txt ./Projects/scenarios/LYM_demo-CavernePlaton-scenario-v1.txt
