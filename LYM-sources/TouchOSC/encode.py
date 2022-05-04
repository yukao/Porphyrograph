#!/usr/bin/env python3

import sys, zipfile, base64
import xml.etree.ElementTree

tree = xml.etree.ElementTree.parse(sys.argv[1])
root = tree.getroot()

def encode_dict(d):
    for k, v in d.items():
        d[k] = base64.b64encode(v.encode()).decode() if k in ("name",
                                                              "osc_cs") else v
    return d

for child in root:
    encode_dict(child.attrib)
    for c in child:
        encode_dict(c.attrib)

with zipfile.ZipFile(sys.argv[1].rstrip(".xml"),
                     "w", zipfile.ZIP_DEFLATED) as z:
    with z.open("index.xml", "w") as f:
        tree.write(f)
