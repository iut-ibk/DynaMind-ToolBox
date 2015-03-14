# -*- coding: utf-8 -*-
"""
/***************************************************************************
 DynaMind-Toolbox
        begin                : 2014-26-012
        copyright            : (C) 2014 Christian Urich
        copyright            : (C) 2014 Michael Mair
        email                : christian.urich@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
"""

__docformat__ = 'restructuredtext en'

import shutil
import os
import sys

def compile_documents_modules(sourcepath,modules):
    indexedmodules = []
    for m in modules:
        try:
            directory = sourcepath + "/" + m + "/doc"
            remove_compiled_module(m)
            shutil.copytree(directory,  os.curdir+"/"+m)
            indexedmodules.append(m)
        except:
            print "No doc found for " + m

    return indexedmodules


def create_compiled_index(modules):

    f = open("index_template.rst", "r")

    index = f.readlines()

    for m in modules:
        index.append("  " + m + '/index\n')

    index.append("\n")
    index.append("Indices and tables\n")
    index.append("==================\n")
    index.append("\n")
    index.append("* :ref:`genindex`\n")
    # index.append("* :ref:`modindex`\n")
    # index.append("* :ref:`search`\n")


    #print ''.join([str(x) for x in index])

    f.close()

    f = open("index.rst", "w")
    f.write(''.join([str(x) for x in index]))


def remove_compiled_module(m):
        try:
            shutil.rmtree(os.curdir+"/"+m)
        except:
            return


if __name__ == "__main__":
    path = ""
    modules = []
    args = sys.argv[1:]
    path = args[0]
    args = args[1:]

    print "Source path: " +  path

    for i in args:
        modules.append(i)

    indexedmodules  = compile_documents_modules(path,modules)
    create_compiled_index(indexedmodules)
