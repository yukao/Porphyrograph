echo off

cd C:\home\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\Alexis.PD.bat

cd C:\home\LYM-projects\
python C:\home\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py  --configuration Projects\configuration\LYM_Alexis-head-double.csv --scenario Projects\scenarios\YN_Alexis-scenario-pen-v2.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_Core.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_Core_full.frag --Update_in Projects\shaders\LYM_Update_Core.frag --Update_out Projects\shaders\LYM_Update_Core_full.frag --Mixing_in Projects\shaders\LYM_Mixing_Core.frag --Mixing_out Projects\shaders\LYM_Mixing_Core_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_Core.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_Core_full.frag --Master_in Projects\shaders\LYM_Master_Alexis.frag --Master_out Projects\shaders\LYM_Master_Alexis_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\include\pg_script_header_Core.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Core.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\include\pg_shader_header_Core.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Core.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Core.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Core.cpp
REM copy C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Alexis.exe
C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe ./Projects/configuration/LYM_Alexis-head-double.csv ./Projects/scenarios/YN_Alexis-scenario-pen-v2.csv
