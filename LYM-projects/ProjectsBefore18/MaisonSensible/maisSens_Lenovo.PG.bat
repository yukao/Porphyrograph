cd D:\sync.com\Sync\LYM-projects\ProjectsBefore18\MaisonSensible\maisSensPorphyrograph 
if not exist pic mkdir pic
perl cmd\PG_confGenerator.prl configuration\LYM_maisSens-head-double.txt scenarios\LYM_maisSens-v11-LaReunion.txt configuration\LYM_maisSens.conf
REM D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\MaisonSensible\Porphyrograph_maisSens.exe ./configuration/LYM_maisSens.conf
D:\sync.com\Sync\LYM-sources\Porphyrograph-VC17\Release\MaisonSensible\Porphyrograph_maisSens.exe ./configuration/LYM_maisSens.conf
cd ..\
