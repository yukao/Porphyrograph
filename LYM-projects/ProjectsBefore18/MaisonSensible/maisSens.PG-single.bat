cd C:\Users\Heracles\Desktop\MaisonSensibleCompiegne\maisSensPorphyrograph
if not exist pic mkdir pic
perl cmd\PG_confGenerator.prl configuration\LYM_maisSens-head-single.csv scenarios\LYM_maisSens-v11-Compiegne.csv configuration\LYM_maisSens.conf
C:\Users\Heracles\Desktop\MaisonSensibleCompiegne\bin\Porphyrograph_maisSens.exe ./configuration/LYM_maisSens.conf
cd ..\
