cd G:\home-win\LYM
set namedate=%DATE:/=_%
set nametime=%TIME:,=_%
set nametime=%nametime::=_%
set nametime=%nametime: =_%
if exist pic move pic pic_%namedate%_%nametime%
if not exist pic mkdir pic
perl cmd\PG_configAndHeaderGenerator.prl geneMuse\LYM_geneMuse-head.conf geneMuse\LYM_geneMuse.csv geneMuse\LYM_geneMuse.conf
G:\home-win\Porphyrograph\Porphyrograph-VC\Release\geneMuse\Porphyrograph_geneMuse.exe ./geneMuse/LYM_geneMuse.conf
