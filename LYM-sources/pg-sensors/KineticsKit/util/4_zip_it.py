import zipfile, os.path, string

def visit(dummy, dirname, names):
    for name in names:
        include = 1
        for ex in excludeending:
            i = name.rfind(ex)
            if i+1 and i == len(name) - len(ex):
                include = 0
                break
        for ex in excludedir:
            if dirname.find(ex)+1:
                include = 0
                break
        filename = os.path.join(dirname, name)
        if include and os.path.isfile(filename):
            print filename[slashpos:]
            archive.write(filename, filename[slashpos:])

reldirtozip = '..' # must be given relative to the cwd
absdirtozip = os.path.normpath(os.getcwd() + os.sep + reldirtozip)
slashpos = string.rfind(absdirtozip, os.sep)+1
dirname = absdirtozip[slashpos:]
archivename = dirname + '.zip'
archive = zipfile.ZipFile(absdirtozip + os.sep + archivename, 'w', zipfile.ZIP_DEFLATED)
#archive.write('../../KineticsKit.pth', 'KineticsKit.pth')

excludeending = [archivename, '.pyc', '.pov', '.bmp', '.avi']
excludedir = []
os.path.walk(absdirtozip, visit, None)

archive.close()
