# kill-process artsd
cd /home/christian/3D/LYM
pd -midiindev 1 -midioutdev 1 -alsamidi -path /usr/local/lib/pd-externals/oscx/ -path /usr/lib/pd/extra/iemnet/ -path /usr/lib/pd/extra/osc/ -jack /home/christian/3D/LYM/GN/LYM_GN.pd&
