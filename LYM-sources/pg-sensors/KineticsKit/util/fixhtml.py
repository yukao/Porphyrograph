import os, string

oldWin = '''span {
	font-family: 'Verdana';
	color: #000000;
	background: #FFFFFF;
	font-size: 9pt;
}
</style>
</head>
<body bgcolor="#FFFFFF">
'''

oldLinux = '''span {
	font-family: 'Verdana';
	color: #000000;
	background: #e0e0d0;
	font-size: 13pt;
}
</style>
</head>
<body bgcolor="#FFFFFF">
'''

new = '''span {
	font-family: 'Verdana';
	color: #000000;
	font-size: 9pt;
}
</style>
</head>
<body bgcolor=#f0f0f8>
'''

oldBackground = '''	background: #FFFFFF;
}
'''

newBackground = '''}
'''

def fixhtmlfile(file):
    if os.path.isfile(file) and file[-5:] == '.html':
        print file
        fp = open(file, 'rb')
        cont = fp.read()
        fp.close()
        cont = string.replace(cont, '\r\n', '\n')
        cont = string.replace(cont, oldWin, new)
        cont = string.replace(cont, oldLinux, new)
        cont = string.replace(cont, oldBackground, newBackground)
        fp = open(file, 'wb')
        fp.write(cont)
        fp.close()

def fixhtmlfiles(dir):
    files = os.listdir(dir)
    for file in files:
        fixhtmlfile(dir + os.sep + file)
