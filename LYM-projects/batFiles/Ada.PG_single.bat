echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py  --configuration Projects\configuration\YN_Ada-head-single.csv --scenario Projects\scenarios\YN_Ada-scenario-v1.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_Core.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_Core_full.frag --Update_in Projects\shaders\LYM_Update_Core.frag --Update_out Projects\shaders\LYM_Update_Core_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Core.frag --Mixing_out Projects\shaders\full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Ada.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Ada_full.frag --Master_in Projects\shaders\LYM_Master_Core.frag --Master_out Projects\shaders\LYM_Master_Core_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Core.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Core.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Core.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Core.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Core.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Core.cpp
REM copy C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Core\Ada.exe
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe ./Projects/configuration/YN_Ada-head-single.csv ./Projects/scenarios/YN_Ada-scenario-v1.csv
