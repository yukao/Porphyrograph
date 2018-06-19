cd D:\cloud\LYM
set namedate=%DATE:/=-%
set namedate=%namedate::=-%
set namedate=%namedate: =-%
set namedate=%namedate:~4,10%
set nametime=%TIME:,=_%
set nametime=%nametime::=-%
set nametime=%nametime: =-%
set nametime=%nametime:.=-%
set nametime=%nametime:~0,8%
if not exist snapshots mkdir snapshots
if exist snapshots\pic_effe move snapshots\pic_effe "snapshots\pic_effe_%namedate%_%nametime%"
if not exist snapshots\pic_effe mkdir snapshots\pic_effe
D:\cloud\Porphyrograph\Porphyrograph-VC15\Release\effe\Porphyrograph_effe.exe ./effe/configuration/LYM_effe-head-single-hausfrau.csv effe/scenarios/LYM_effe-scenario-hausfrau.csv
