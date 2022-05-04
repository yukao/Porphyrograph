echo off

chdir /d C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START C:\sync.com\Sync\LYM-projects\batFiles\core.PD.bat

chdir /d C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py --configuration Projects\configuration\LYM_Rivets-head-single.csv --scenario Projects\scenarios\LYM_Rivets-scenario-v1.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_demo.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_demo_full.frag --Update_in Projects\shaders\LYM_Update_Rivets.frag --Update_out Projects\shaders\LYM_Update_Rivets_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Rivets.frag --Mixing_out Projects\shaders\LYM_Mixing_Rivets_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Rivets.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Rivets_full.frag --Master_in Projects\shaders\LYM_Master_Rivets.frag --Master_out Projects\shaders\LYM_Master_Rivets_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Rivets.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Rivets.cpp --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Rivets.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Rivets.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Rivets.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Rivets.cpp

C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Rivets\Porphyrograph_core_Rivets.exe ./Projects/configuration/LYM_Rivets-head-single.csv ./Projects/scenarios/LYM_Rivets-scenario-v1.csv
