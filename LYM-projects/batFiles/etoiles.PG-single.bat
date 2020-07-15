echo off

cd D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd D:\sync.com\Sync\LYM-projects\
perl D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v4.prl Projects\configuration\LYM_etoiles-head-single.csv Projects\scenarios\LYM_etoiles-scenario-v1.csv Projects\shaders\LYM_ParticleAnimation_etoiles.frag Projects\shaders\LYM_ParticleAnimation_etoiles_full.frag Projects\shaders\LYM_Update_etoiles.frag Projects\shaders\LYM_Update_etoiles_full.frag Projects\shaders\LYM_Mixing_etoiles.frag Projects\shaders\LYM_Mixing_etoiles_full.frag Projects\shaders\LYM_ParticleSplat_etoiles.frag Projects\shaders\LYM_ParticleSplat_etoiles_full.frag Projects\shaders\LYM_Master_etoiles.frag Projects\shaders\LYM_Master_etoiles_full.frag ..\LYM-sources\Porphyrograph-src\pg_script_header_etoiles.h ..\LYM-sources\Porphyrograph-src\pg_script_body_etoiles.cpp  ..\LYM-sources\Porphyrograph-src\pg_shader_header_etoiles.h ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_etoiles.cpp ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_etoiles.cpp ..\LYM-sources\Porphyrograph-src\pg_update_body_etoiles.cpp
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_etoiles\Porphyrograph_core_etoiles.exe ./Projects/configuration/LYM_etoiles-head-single.csv ./Projects/scenarios/LYM_etoiles-scenario-v1.csv
