cd G:\home-win\LYM\MaisonSensible\maisSensPorphyrograph
if not exist pic mkdir picG
perl cmd\PG_confGenerator.prl configuration\LYM_maisSens-head-flat.csv scenarios\LYM_maisSens-v11.csv configuration\LYM_maisSens.conf
G:\home-win\LYM\MaisonSensible\bin\Porphyrograph_maisSens.exe ./configuration/LYM_maisSens.conf
cd ..\
