cd C:\Users\jacquemi.LIMSI\Desktop\LYM
set namedate=%DATE:/=_%
set nametime=%TIME:,=_%
set nametime=%nametime::=_%
set nametime=%nametime: =_%
if exist pic move pic pic_%namedate%_%nametime%
if not exist pic mkdir pic
C:\Users\jacquemi.LIMSI\DM\src\DM\Release\DM.exe  ./dcONT/XML/LYM_dcONT.conf ./dcONT/XML/LYM_scene_dcONT.xml

