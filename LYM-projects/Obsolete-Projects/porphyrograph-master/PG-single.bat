set namedate=%DATE:/=_%
set nametime=%TIME:,=_%
set nametime=%nametime::=_%
set nametime=%nametime: =_%
if exist snapshots\pic move snapshots\pic snapshots\pic_%namedate%_%nametime%
if not exist snapshots mkdir snapshots
if not exist snapshots\pic mkdir snapshots\pic
:: perl cmd\PG_confGenerator.prl .\configuration\LYM_core-head-single.csv scenarios\LYM_core-scenario.csv shaders\LYM_Drawing_core-FS.glsl shaders\LYM_Drawing_core-FS_full.glsl shaders\LYM_Composition_core-FS.glsl shaders\LYM_Composition_core-FS_full.glsl shaders\LYM_Final_core-FS.glsl shaders\LYM_Final_core-FS_full.glsl configuration\LYM_core-single.conf
.\bin\Porphyrograph_core.exe  ./configuration/LYM_core-head-single.csv ./scenarios/LYM_core-scenario.csv
