cd D:\sync.com\Sync\LYM-projects\ProjectsBefore18\MaisonSensible\maisSensPorphyrograph
if not exist pic mkdir pic
perl cmd\PG_confGenerator.prl configuration\LYM_maisSens-head-single.txt scenarios\LYM_maisSens-v11-LaReunion.txt configuration\LYM_maisSens.conf
D:\sync.com\Sync\LYM-projects\ProjectsBefore18\MaisonSensible\bin\Porphyrograph_maisSens.exe ./configuration/LYM_maisSens.conf
cd ..\
