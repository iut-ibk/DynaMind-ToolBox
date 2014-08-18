# This file is part of DynaMite
#
# Copyright (C) 2014 Michael Mair
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
#

from subprocess import *
import time
import os,errno

class DynaMindScenarios:
    def __createargumentlist(self,scenarios,scenarioresults,index=0):
        result = []
        for atomicscenario in scenarios[index]:
            atomicstring = ""
    
            for pv in atomicscenario[0]:
                atomicstring = atomicstring + pv[0] + "=" + pv[1] + ";"
                
            atomicstring = atomicstring[:-1]
    
            atomicpath = atomicscenario[1]
            if scenarios.__len__()==index+1:
                result.append((atomicstring,atomicpath))
            else:
                succ = self.__createargumentlist(scenarios,scenarioresults,index+1)
                for s in succ:
                    result.append((atomicstring + ";" + s[0],atomicpath + "/" + s[1]))
        
        return result
    
    def __mkdir_p(self,path):
        try:
            os.makedirs(path)
        except OSError as exc: # Python >2.5
            if exc.errno == errno.EEXIST and os.path.isdir(path):
                pass
            else: raise
    
    def run(self,dynamindmodel,repeat,resultdir,maxcpu,scenarios,scenarioresults):
        rawargs = self.__createargumentlist(scenarios,scenarioresults)
        executionarguments = []
        
        for a in rawargs:
            self.__mkdir_p(resultdir + "/" + a[1])
            
            replacemodeloutput=""
            copyfiles=""
            for name in scenarioresults:
                open(resultdir + "/" + a[1] + "/" + name[1], 'a').close()
                copyfiles = copyfiles + resultdir + "/" + a[1] + "/" + name[1] + "," + resultdir + "/" + a[1] + "/DMITERATION_" + name[1] + ";"
                replacemodeloutput = replacemodeloutput + name[0] + ","+ resultdir + "/" + a[1] + "/" + name[1] + ";"
                
            replacemodeloutput = replacemodeloutput[:-1]
            copyfiles = copyfiles[:-1]    
                    
            executionarguments.append([("./dynamind","--repeat",str(repeat),"--replace","\"" + replacemodeloutput + "\"","--parameter","\"" + a[0] + "\"","--cpfile","\"" + copyfiles +"\"",dynamindmodel),(resultdir + "/" + a[1] + "/dynamind.log")])
        
        running_procs = []
        
        while executionarguments.__len__():
            if running_procs.__len__() < maxcpu:
                ex = executionarguments.pop(0)
                #print " ".join(ex[0])
                FNULL = open(ex[1], 'w') 
                running_procs.append(Popen(ex[0],stdout=FNULL, stderr=STDOUT))
                
            for proc in running_procs:
                retcode = proc.poll()
                if retcode is not None: # Process finished.
                    running_procs.remove(proc)
                    print  "Instance finished. Still running: " + str(running_procs.__len__()) + " Still waiting: " + str(executionarguments.__len__())
                    if retcode != 0:
                        print "Error: " + str(retcode)
                    break
                else: # No process is done, wait a bit and check again.
                    time.sleep(.1)
                    continue
                
        while running_procs:
            for proc in running_procs:
                retcode = proc.poll()
                if retcode is not None: # Process finished.
                    running_procs.remove(proc)
                    print  "Instance finished. Still running: " + str(running_procs.__len__()) + " Still waiting: " + str(executionarguments.__len__())
                    if retcode != 0:
                        print "Error: " + str(retcode)
                    break
                else: # No process is done, wait a bit and check again.
                    time.sleep(.1)
                    continue
        
        print "All processes finished"
