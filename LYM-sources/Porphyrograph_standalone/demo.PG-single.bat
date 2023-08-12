echo off

cd C:\Users\yukao\Desktop\Porphyrograph
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\batFiles\core.PD.bat

cd C:\Users\yukao\Desktop\Porphyrograph
perl .\cmd\PG_confGenerator_Projects-v3.prl .\configuration\LYM_demo-head-single.txt .\scenarios\LYM_demo-scenario-v3.txt .\Projects\shaders\LYM_ParticleAnimation_demo.frag .\Projects\shaders\LYM_ParticleAnimation_demo_full.frag .\Projects\shaders\LYM_Update_demo.frag .\Projects\shaders\LYM_Update_demo_full.frag .\Projects\shaders\LYM_Mixing_demo.frag .\Projects\shaders\LYM_Mixing_demo_full.frag .\Projects\shaders\LYM_ParticleSplat_demo.frag .\Projects\shaders\LYM_ParticleSplat_demo_full.frag .\Projects\shaders\LYM_Master_demo.frag .\Projects\shaders\LYM_Master_demo_full.frag .\LYM-sources\Porphyrograph-src\pg_script_header_demo.h .\LYM-sources\Porphyrograph-src\pg_script_body_demo.cpp  .\LYM-sources\Porphyrograph-src\pg_shader_header_demo.h .\LYM-sources\Porphyrograph-src\pg_shader_body_decl_demo.cpp .\LYM-sources\Porphyrograph-src\pg_shader_body_bind_demo.cpp .\LYM-sources\Porphyrograph-src\pg_update_body_demo.cpp
.\Release\Porphyrograph_core_demo.exe ./configuration/LYM_demo-head-single.txt ./scenarios/LYM_demo-scenario-v3.txt
