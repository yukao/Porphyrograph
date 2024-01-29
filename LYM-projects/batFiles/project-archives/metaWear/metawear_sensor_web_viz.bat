echo off

cd C:\sync.com\Sync\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

REM Araknit program + scenario with 33 clip arts corresponding to the positions of the masses in the web 
REM                        and with drawing of positions of the 2 sensor controlled mass (through replay of 2 paths on track 2)
cd C:\sync.com\Sync\LYM-projects\
python C:\sync.com\Sync\LYM-projects\Projects\cmd\PG_confGenerator_Projects-v1.py --configuration Projects\configuration\metawear_sensor_web_vizualization-configuration-v1.csv --scenario Projects\scenarios\metawear_sensor_web_vizualization-scenario-v1.csv --particleAnimation_in Projects\shaders\LYM_ParticleAnimation_araknit.frag --particleAnimation_out Projects\shaders\LYM_ParticleAnimation_araknit_full.frag --Update_in Projects\shaders\LYM_Update_araknit.frag --Update_out Projects\shaders\LYM_Update_araknit_full.frag --Mixing_in Projects\shaders\LYM_Mixing_araknit.frag --Mixing_out Projects\shaders\LYM_Mixing_araknit_full.frag --ParticleSplat_in Projects\shaders\LYM_ParticleSplat_araknit.frag --ParticleSplat_out Projects\shaders\LYM_ParticleSplat_araknit_full.frag --Master_in Projects\shaders\LYM_Master_araknit.frag --Master_out Projects\shaders\LYM_Master_araknit_full.frag --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_araknit.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_araknit.cpp --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_araknit.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_araknit.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_araknit.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_araknit.cpp
C:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\core_araknit\Porphyrograph_core_araknit.exe ./Projects/configuration/metawear_sensor_web_vizualization-configuration-v1.csv ./Projects/scenarios/metawear_sensor_web_vizualization-scenario-v1.csv
