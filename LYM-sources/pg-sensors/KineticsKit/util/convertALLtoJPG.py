import os
from PIL import Image

files = os.listdir('.')
for file in files:
    if file[-4:] == '.bmp':
        basename = os.path.splitext(file)[0]
        outfile = basename + '.jpg'
        print "converting '" + file + "' to '" + outfile + "'"
        try:
            im = Image.open(file)
            im.save(outfile, quality=100)
            #os.remove(file)
        except:
            print "cannot convert '" + file
