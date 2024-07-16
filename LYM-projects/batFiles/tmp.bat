echo off

cd C:\home\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\home\LYM-projects\
C:\home\LYM-sources\Porphyrograph-VC17\Release\core_Core\Porphyrograph_core_Core.exe ./Projects/configuration/LYM_Song-head-single.csv ./Projects/scenarios/new_scenario_1_double.csv
