#!/usr/bin/env python
#
# $Id: happydocwin.py,v 1.1 2001/04/15 18:53:11 doughellmann Exp $
#
# Time-stamp: <01/04/15 14:52:36 dhellmann>
#
# Copyright 2001 Doug Hellmann.
#
#
#                         All Rights Reserved
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby
# granted, provided that the above copyright notice appear in all
# copies and that both that copyright notice and this permission
# notice appear in supporting documentation, and that the name of Doug
# Hellmann not be used in advertising or publicity pertaining to
# distribution of the software without specific, written prior
# permission.
#
# DOUG HELLMANN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
# NO EVENT SHALL DOUG HELLMANN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
# OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

"""HappyDoc command-line app for Windows.

"""

__rcs_info__ = {
    #
    #  Creation Information
    #
    'module_name'  : '$RCSfile: happydocwin.py,v $',
    'rcs_id'       : '$Id: happydocwin.py,v 1.1 2001/04/15 18:53:11 doughellmann Exp $',
    'creator'      : 'Doug Hellmann <DougHellmann@bigfoot.com>',
    'project'      : 'UNSPECIFIED',
    'created'      : 'Sun, 15-Apr-2001 14:51:51 EDT',

    #
    #  Current Information
    #
    'author'       : '$Author: doughellmann $',
    'version'      : '$Revision: 1.1 $',
    'date'         : '$Date: 2001/04/15 18:53:11 $',
}

#
# Import system modules
#


#
# Import Local modules
#
from happydoc_class import HappyDoc


#
# Module
#

if __name__ == '__main__':
    try:
        HappyDoc().run()
    except HappyDoc.HelpRequested:
        pass
