import os, sys

if sys.platform == 'win32':
    scite = r'C:\Programme\wscite\SciTE.exe'
else:
    scite = '/iuehome/gritsch/bin/scite'

files = os.listdir('../examples/')
for file in files:
    if file[:3] == 'exa':
        src = '../examples/' + file
        dest = '../html/' + file + '.html'
        os.system(scite + ' -open:%s -exportashtml:%s -quit:' % (src, dest))

files = os.listdir('../')
for file in files:
    if file[-3:] == '.py':
        src = '../' + file
        dest = '../html/KineticsKit.' + file + '.html'
        os.system(scite + ' -open:%s -exportashtml:%s -quit:' % (src, dest))
