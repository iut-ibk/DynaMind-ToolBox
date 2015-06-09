#!/usr/bin/python3

import glob, os, sys, shutil
import multiprocessing
import subprocess
import tempfile
import time
import smtplib
from email.mime.text import MIMEText


MAX_PROCS = 20 #multiprocessing.cpu_count()-1 #or give a number


def execswmm(swmm5bin, simname, cursimdir, num):
	binextender = "./"
	if sys.platform == 'win32':
		binextender == ""

	cmd = (binextender+swmm5bin,simname+".inp",simname+".rep")
	print("Starting process %s: %s" % (num, simname))
	sp = subprocess.call(cmd,cwd=cursimdir,stdout=subprocess.DEVNULL)
	print("Finished process: %s" % num)
	return 0
	
def execswmm_parallel(args):
	return execswmm(*args)

def sendmessage():
	msg = MIMEText("")
	msg['Subject'] = "runswmm just finished"
	msg['From'] = "christian.mikvits@uibk.ac.at"
	msg['To'] = "christian.mikovits@uibk.ac.at"
	c = smtplib.SMTP('smtp.uibk.ac.at')
	c.send_message(msg)
	c.quit()

def main():

	workdir = os.getcwd()
	tempdir = tempfile.gettempdir()

	swmm5bin = "swmm5"
	if sys.platform == 'win32':
		swmm5bin = "swmm5.exe"

	inpdirs = os.path.join(workdir,"inpfiles")
	datdirs = os.path.join(workdir,"datfiles")
	simdirs = os.path.join(tempdir,"simdirs")
	swmm5loc = os.path.join(workdir,swmm5bin)

	try:
		shutil.rmtree(simdirs)
	except:
		pass
	
	os.mkdir(simdirs)

	pool = multiprocessing.Pool(processes=MAX_PROCS)
	inpfiles = glob.glob(inpdirs+"/*inp")
	datfiles = glob.glob(datdirs+"/*dat")
	totaliterations = len(inpfiles)*len(datfiles)

	print("Starting a total of %s itarations using %s CPUs" % (totaliterations,MAX_PROCS))
	x=0

	for inpfile in inpfiles:
		for datfile in datfiles:
			x=x+1
			inpname=os.path.split(inpfile)[1].split('.')[0]
			datname=os.path.split(datfile)[1].split('.')[0]
			simname=inpname+'_'+datname
		
			cursimdir = os.path.join(simdirs,simname)
			curswmmbin = os.path.join(cursimdir,swmm5bin)
			curinpfile = os.path.join(cursimdir,simname+".inp")
			curdatfile = os.path.join(cursimdir,"rain.dat")
			currepfile = os.path.join(cursimdir,simname+".rep")
		
			os.mkdir(cursimdir)
		
			shutil.copy(swmm5loc,cursimdir)
			shutil.copyfile(inpfile,curinpfile)
			shutil.copyfile(datfile,curdatfile)
		
			params = [(swmm5bin,simname,cursimdir,x)]
		
			res = pool.map_async(execswmm_parallel, params)

	res.get()
	print("FINISHED")
	sendmessage()

if __name__ == "__main__":
	main()
