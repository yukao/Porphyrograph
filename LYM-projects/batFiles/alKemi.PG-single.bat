cd D:\LYM-projects\
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
if exist snapshots\pic_alKemi move snapshots\pic_alKemi "snapshots\pic_alKemi_%namedate%_%nametime%"
if not exist snapshots\pic_alKemi mkdir snapshots\pic_alKemi
D:\LYM-sources\Porphyrograph-VC17\Release\alKemi\Porphyrograph_alKemi.exe ./ProjectsBefore18/alKemi/LYM_alKemi-head-single.csv ./ProjectsBefore18/alKemi/LYM_alKemi-scenario.csv
