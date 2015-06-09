#!/usr/bin/python3

import glob, os, sys, shutil
import multiprocessing
import subprocess
import tempfile
import time
import smtplib
import argparse
from email.mime.text import MIMEText

EMAIL = "christian.mikovits@uibk.ac.at"
SMTP = "smtp.uibk.ac.at"

def create_argparser():
    parser = argparse.ArgumentParser(description='runs swmm5 processes in parallel')
    parser.add_argument('-p', '--procs', default=multiprocessing.cpu_count()-1, dest='max_procs',
    					help='how many processors should be used,\n\t default and maximum is number of processors - 1')
    parser.add_argument('-e', '--email', default=EMAIL, dest='email',
    					help='email address for the finish notification')
    parser.add_argument('-s', '--smtp', default=SMTP, dest='smtp',
    					help='smtp server to send the message')

    parser.add_argument('-w', '--workdir', default=os.getcwd(), dest='workdir',
    					help='sets ')
    parser.add_argument('-d', '--dryrun', dest='dryrun', action='store_true',
    					help='')

    return parser

def execswmm(swmm5bin, simname, cursimdir, num, totalnum):
	binextender = "./"
	if sys.platform == 'win32':
		binextender == ""

	cmd = (binextender+swmm5bin,simname+".inp",simname+".rep")
	print("Starting process %s of %s: %s" % (num, totalnum, simname))
	sp = subprocess.call(cmd,cwd=cursimdir,stdout=subprocess.DEVNULL)
	print("Finished process: %s of %s" % (num, totalnum))
	return 0
	
def execswmm_parallel(args):
	return execswmm(*args)

def sendmessage():
	msg = MIMEText("")
	msg['Subject'] = "runswmm just finished"
	msg['From'] = EMAIL
	msg['To'] = EMAIL
	c = smtplib.SMTP(SMTP)
	c.send_message(msg)
	c.quit()

def main():

	parser = create_argparser()
	args = parser.parse_args()
	workdir = args.workdir
	tempdir = args.workdir
	max_procs = args.max_procs
	
	swmm5bin = "swmm5"
	if sys.platform == 'win32':
		swmm5bin = "swmm5.exe"

	inpdirs = os.path.join(workdir,"inpfiles")
	datdirs = os.path.join(workdir,"datfiles")
	simdirs = os.path.join(tempdir,"simdirs")
	swmm5loc = os.path.join(workdir,swmm5bin)
	
	print(args)
	
	if args.dryrun != True:

		try:
			shutil.rmtree(simdirs)
		except:
			pass
	
		os.mkdir(simdirs)

	pool = multiprocessing.Pool(processes=max_procs)
	inpfiles = glob.glob(inpdirs+"/*inp")
	if len(inpfiles) == 0:
		print("no inputfiles found")
		sys.exit(0)
	
	datfiles = glob.glob(datdirs+"/*dat")
	if len(datfiles) == 0:
		print("no rainfiles found")
		sys.exit(0)

	totaliterations = len(inpfiles)*len(datfiles)

	print("Starting a total of %s itarations using %s CPUs" % (totaliterations,max_procs))
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
		
			params = [(swmm5bin,simname,cursimdir,x,totaliterations)]
			
			res = pool.map_async(execswmm_parallel, params)
			
	res.get()
	print("FINISHED")
	sendmessage()

if __name__ == "__main__":
	main()
