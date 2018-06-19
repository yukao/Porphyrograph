cd D:\LYM-projects\
set namedate=%DATE:/=_%
set nametime=%TIME:,=_%
set nametime=%nametime::=_%
set nametime=%nametime: =_%
if exist pic move pic pic_%namedate%_%nametime%
if not exist pic mkdir pic
D:\LYM-sources\Porphyrograph-VC17\Release\DM_dcONT\Porphyrograph_DM_dcONT.exe ./ProjectsBefore18/dcONT/XML/LYM_dcONT.conf ./ProjectsBefore18/dcONT/XML/LYM_scene_dcONT.xml

