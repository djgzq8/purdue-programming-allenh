all: os.dlx.obj
	cd apps/q3; make
	cd bin; dlxsim  -x os.dlx.obj -a -D y -u makeprocs.dlx.obj #; ee469_fixterminal

clean:
	cd os; make clean
	cd apps/example; make clean
	cd apps/q2; make clean
	cd apps/q3; make clean
	cd apps/q4; make clean
	cd apps/q5; make clean

os.dlx.obj:
	cd os; make
