#
# Makefile for the RCPsuite software
#
include $(CWPROOT)/src/Makefile.config

donothing: # To protect against an idle "make" to "see what happens"
	@echo " This make default is do_nothing"

install: checkroot makedirs rcpsstuff

# check to see if the CWPROOT path is set
checkroot	:
	@./chkroot.sh
	@echo $(RCPSROOT)

# Make the bin/include/lib directories if not already there
makedirs: 
	@echo "Making necessary directories"
	@./mkdirectories.sh

rcpsstuff:
	cd ./rcps; ls
#	cd ./par; $(MAKE)


