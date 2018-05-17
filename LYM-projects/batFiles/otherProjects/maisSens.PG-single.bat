cd G:\home-win\LYM
set namedate=%DATE:/=_%
set nametime=%TIME:,=_%
set nametime=%nametime::=_%
set nametime=%nametime: =_%
if exist pic move pic pic_%namedate%_%nametime%
if not exist pic mkdir pic
:: perl cmd\PG_configAndHeaderGenerator.prl maisSens\LYM_maisSens-head-single.conf maisSens\LYM_maisSens.csv ..\Porphyrograph\Porphyrograph-maisSens-src\pg_script_header.h ..\Porphyrograph\Porphyrograph-maisSens-src\pg_script_body.cpp maisSens\LYM_maisSens.conf
perl cmd\PG_configAndHeaderGenerator.prl maisSens\LYM_maisSens-head-single.conf maisSens\LYM_maisSens.csv maisSens\LYM_maisSens.conf
G:\home-win\Porphyrograph\Porphyrograph-VC\Release\Porphyrograph_maisSens.exe ./maisSens/LYM_maisSens.conf
