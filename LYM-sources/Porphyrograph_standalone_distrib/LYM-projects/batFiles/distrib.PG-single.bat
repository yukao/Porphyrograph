tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd ..\
python .\Projects\cmd\PG_confGenerator_Projects-v2.py --nb_configurations 1 --configuration Projects\configuration\YN_distrib-head-single.csv --scenario Projects\scenarios\YN_distrib-scenario-v1.csv --script_header_out ..\LYM-sources\Porphyrograph-src\pg_script_header_Core.h --script_body_out ..\LYM-sources\Porphyrograph-src\pg_script_body_Core.cpp  --shader_header_out ..\LYM-sources\Porphyrograph-src\pg_shader_header_Core.h --shader_body_decl_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_decl_Core.cpp --shader_body_bind_out ..\LYM-sources\Porphyrograph-src\pg_shader_body_bind_Core.cpp --update_body_out ..\LYM-sources\Porphyrograph-src\pg_update_body_Core.cpp
.\bin\Porphyrograph-distrib.exe ./Projects/configuration/YN_distrib-head-single.csv ./Projects/scenarios/YN_distrib-scenario-v1.csv