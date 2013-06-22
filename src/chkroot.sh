#! /bin/sh
# check to see if the RCPSROOT environment variable is set
# borrowed shell part of the general CWP installing and maintenence environment
# John Stockwell 9 Dec 1992 -- Center for Wave Phenomena

PATH=$PATH:/bin:/usr/bin:/usr/ucb:/usr/bsd

CMD=`basename $0`

if test "${RCPSROOT}" = ""
then
	echo "RCPSROOT environment variable is not set!!"
	echo "Please read README_TO_INSTALL for more information!!"
	echo ".... Aborting make"; 1>&2 exit 1 
fi

echo "Installing the RCPsuite codes under the ROOT = ${RCPSROOT}"
echo "Continue install? [y/n] "  | tr -d "\012"
	read RESP
		case $RESP in
			y*|Y*) # continue
			;;
			*) # abort
	
				echo "Aborting make " ; exit 1
			;;
		esac
exit 0
