all:
	cd os; make
#	cd makeprocs; make
#	cd hello_world; make
#	cd max_va; make
#	cd recursion; make
#	cd seg_fault; make
#	cd multi; make
clean:
	cd os; make clean
#	cd makeprocs; make clean
#	cd hello_world; make clean
#	cd max_va; make clean 
#	cd recursion; make clean
#	cd seg_fault; make clean
#	cd multi; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 5; ee469_fixterminal
