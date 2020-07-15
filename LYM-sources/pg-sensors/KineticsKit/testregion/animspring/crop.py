import os
from PIL import Image

files = os.listdir('.')
for file in files:
    if file[-4:] == '.bmp':
        basename = os.path.splitext(file)[0]
        outfile = basename + '.gif'
        print "converting '" + file + "' to '" + outfile + "'"
        try:
            im = Image.open(file)
            im = im.crop((0, 0, 50, 9))
            #im.save(outfile)
            #os.remove(basename + '.pov')
            #os.remove(file)
            im.save(file)
        except:
            print "cannot convert '" + file
