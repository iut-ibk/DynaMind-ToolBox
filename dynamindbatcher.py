from subprocess import *
import time
import datetime
import os,errno
import shutil

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
    
    def run(self,dynamindmodel,repeat,resultdir,maxcpu,scenarios,scenarioresults,addtimestamp=False):
        if addtimestamp :
            ts = time.time()
            st = datetime.datetime.fromtimestamp(ts).strftime('%Y_%m_%d_%H_%M_%S')
            self.__mkdir_p(resultdir + "/" + st)
            resultdir = resultdir + "/" + st
        
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
            
            logfile = resultdir + "/" + a[1] + "/dynamind.log"

            executionarguments.append([("./dynamind","--repeat",str(repeat),"--replace",replacemodeloutput,"--parameter",a[0],"--logpath",logfile,"--cpfile",copyfiles,dynamindmodel),(resultdir + "/" + a[1] + "/dynamind.log")])
        
        running_procs = []
        
        while executionarguments.__len__() or running_procs.__len__():
            st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
            if running_procs.__len__() < maxcpu and executionarguments.__len__():
                ex = executionarguments.pop(0)
                #print " ".join(ex[0])
                FLOG = open(ex[1], 'w') 
                FNULL = open(os.devnull, 'w')
                running_procs.append([(Popen(ex[0],stdout=FNULL, stderr=FLOG)),(ex[1]),(ex)])
                print st + " Started new dynamind instance: Still running: " + str(running_procs.__len__()) + " Still waiting: " + str(executionarguments.__len__())
                
            for proc in running_procs:
                retcode = proc[0].poll()
                if retcode is not None: # Process finished.
                    running_procs.remove(proc)
                    print  st + " Instance finished. Still running: " + str(running_procs.__len__()) + " Still waiting: " + str(executionarguments.__len__())
                    if retcode != 0:
                        if(retcode == -11 or retcode == -12):
                            print st + " Trying to restart process because it returned " + str(retcode)
                            savelog = proc[1] + "." + str(retcode) + ".log"
                            shutil.copyfile(proc[1], savelog)
                            print st + " Saved log: " + savelog
                            executionarguments.append(proc[2])
                        else:
                            print st + " Error: " + str(retcode) + " Log: " + proc[1]
                    else:
                        os.remove(proc[1])
                    break
                else: # No process is done, wait a bit and check again.
                    time.sleep(.1)
                    continue
        
        print "All processes finished"
