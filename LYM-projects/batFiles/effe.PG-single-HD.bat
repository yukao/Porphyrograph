echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_effe-head-single.csv Projects\scenarios\LYM_effe-scenario-v3.csv Projects\shaders\LYM_ParticleAnimation_effe.frag Projects\shaders\LYM_ParticleAnimation_effe_full.frag Projects\shaders\LYM_Update_effe.frag Projects\shaders\LYM_Update_effe_full.frag Projects\shaders\LYM_Mixing_effe.frag Projects\shaders\LYM_Mixing_effe_full.frag Projects\shaders\LYM_ParticleSplat_effe.frag Projects\shaders\LYM_ParticleSplat_effe_full.frag Projects\shaders\LYM_Master_effe.frag Projects\shaders\LYM_Master_effe_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_effe.h ..\LYM-sources\Porphyrograph-src\pg_script_body_effe.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_effe.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_effe.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_effe.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_effe.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_effe\Porphyrograph_core_effe.exe ./Projects/configuration/LYM_effe-head-single.csv ./Projects/scenarios/LYM_effe-scenario-v3.csv