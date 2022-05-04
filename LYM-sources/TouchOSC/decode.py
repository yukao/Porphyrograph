#!/usr/bin/env python3

import sys, zipfile, base64
import xml.etree.ElementTree, xml.dom.minidom

z = zipfile.ZipFile(sys.argv[1])
f = z.open("index.xml")
tree = xml.etree.ElementTree.parse(f)
root = tree.getroot()

def decode_dict(d):
    for k, v in d.items():
        d[k] = base64.b64decode(v).decode() if k in ("name", "osc_cs") else v
    return d

for child in root:
    decode_dict(child.attrib)
    for c in child:
        decode_dict(c.attrib)

rough_string = xml.etree.ElementTree.tostring(root, 'utf-8')
reparsed = xml.dom.minidom.parseString(rough_string)
of = open(sys.argv[1] + ".xml", "w")
of.write(reparsed.toprettyxml(indent="\t"))
