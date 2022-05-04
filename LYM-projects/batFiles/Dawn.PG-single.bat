echo off

chdir /d D:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START D:\sync.com\Sync\LYM-projects\batFiles\core.PD.bat

chdir /d D:\sync.com\Sync\LYM-projects\
python D:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py --configuration Projects\configuration\LYM_Dawn-head-single.csv --scenario Projects\scenarios\LYM_Dawn-scenario-v1.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_demo.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_demo_full.frag --Update_in Projects\shaders\LYM_Update_Dawn.frag --Update_out Projects\shaders\LYM_Update_Dawn_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Dawn.frag --Mixing_out Projects\shaders\LYM_Mixing_Dawn_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_demo.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_demo_full.frag --Master_in Projects\shaders\LYM_Master_Dawn.frag --Master_out Projects\shaders\LYM_Master_Dawn_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Dawn.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Dawn.cpp --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Dawn.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Dawn.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Dawn.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Dawn.cpp

D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Dawn\Porphyrograph_core_Dawn.exe ./Projects/configuration/LYM_Dawn-head-single.csv ./Projects/scenarios/LYM_Dawn-scenario-v1.csv
