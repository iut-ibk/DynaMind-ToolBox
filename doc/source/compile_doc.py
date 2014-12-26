# -*- coding: utf-8 -*-
"""
/***************************************************************************
 DynaMind-Toolbox
        begin                : 2014-26-012
        copyright            : (C) 2014 Christian Urich
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

def compile_documents_modules(modules):
    for m in modules:
        directory = "../../" + m + "/doc"
        remove_compiled_module(m)
        shutil.copytree(directory,  os.curdir+"/"+m)

def create_compiled_index(modules):

    f = open("index.rst", "r")

    index = f.readlines()

    for m in modules:
        index.append("  " + m+'/index\n')

    index.append("\n")
    index.append("Indices and tables\n")
    index.append("==================\n")
    index.append("\n")
    index.append("* :ref:`genindex`\n")
    index.append("* :ref:`modindex`\n")
    index.append("* :ref:`search`\n")


    #print ''.join([str(x) for x in index])

    f.close()

    f = open("index_compile.rst", "w")
    f.write(''.join([str(x) for x in index]))


def remove_compiled_module(m):
        try:
            shutil.rmtree(os.curdir+"/"+m)
        except:
            return


if __name__ == "__main__":
    modules = ["DynaMind", "DynaMind-GDALModules"]
    compile_documents_modules(modules)
    create_compiled_index(modules)
    # remove_compiled_module(modules)





