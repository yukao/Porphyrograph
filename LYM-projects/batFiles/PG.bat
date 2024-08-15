echo off

cd C:\home\LYM-projects\batFiles
tasklist /fi "imagename eq pd.exe" |find /i "pd.exe" > nul
if errorlevel 1 START .\core.PD.bat

cd C:\home\LYM-projects\
C:\home\LYM-sources\Porphyrograph-VC17\Release\core_%2\Porphyrograph_core_%2.exe ./Projects/scenarios/%1
