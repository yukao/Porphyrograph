cd "C:\Users\DAF VIDEO\Desktop\MaisonSensible\maisSensPorphyrograph"
if not exist pic mkdir pic
perl cmd\PG_confGenerator.prl configuration\LYM_maisSens-head-single.txt scenarios\LYM_maisSens-v11-LaReunion.txt configuration\LYM_maisSens.conf
"C:\Users\DAF VIDEO\Desktop\MaisonSensible\bin\Porphyrograph_maisSens.exe" ./configuration/LYM_maisSens.conf
cd ..\
