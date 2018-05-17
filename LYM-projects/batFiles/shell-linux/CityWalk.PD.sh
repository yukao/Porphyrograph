# kill-process artsd
cd /home/host/LYM
pd -midiindev 1 -midioutdev 1 -alsamidi -path /usr/local/lib/pd-externals/oscx/ -path /usr/lib/pd/extra/iemnet/ -path /usr/lib/pd/extra/osc/ -jack /home/host/LYM/CityWalk/XML/LYM_CW.pd&
