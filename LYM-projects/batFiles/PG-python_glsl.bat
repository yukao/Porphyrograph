echo off

cd C:\home\LYM-projects\
python C:\home\LYM-projects\Projects\cmd\PG_glsl_source_generator.py  --header ..\LYM-sources\Porphyrograph-src\include\pg-header.h --nb_scenarios %1 --scenarios Projects\scenarios\%2 --full_scenario Projects\scenarios\%3 --output_scenario Projects\scenarios\new_scenario.csv --TouchOSC_interface ..\LYM-sources\TouchOSC\scenes.tosc
