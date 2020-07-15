import os, sys

#os.rename('../__init__.py', '../__init__.py_')
#f = open('../__init__.py', 'wt')
#f.close()

if sys.platform == 'win32':
    os.system('pydoc.py -w KineticsKit KineticsKit.__init__ KineticsKit.system KineticsKit.mass KineticsKit.springs KineticsKit.povexport')
    os.system('move KineticsKit* ..\html\ ')
else:
    os.system('python2 pydoc.py -w KineticsKit KineticsKit.__init__ KineticsKit.system KineticsKit.mass KineticsKit.springs KineticsKit.povexport')
    os.system('mv KineticsKit* ../html/')

#os.remove('../__init__.py')
#os.rename('../__init__.py_', '../__init__.py')
