all: os.dlx.obj
#	cd apps/lottery_test; make
	cd apps/q2; make
	cd bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 4 4
#	cd bin; dlxsim -x os.dlx.obj -a -u userprog4.dlx.obj 2
#; ee469_fixterminal


#q2: os.dlx.obj
#	cd apps/q2; make
#	cd bin; dlxsim  -x os.dlx.obj -a -D y -u makeprocs.dlx.obj 4; ee469_fixterminal
#	
#q3: os.dlx.obj
#	cd apps/q3; make
#	cd bin; dlxsim  -x os.dlx.obj -a -D y -u makeprocs.dlx.obj; ee469_fixterminal 
#	
#q4: os.dlx.obj
#	cd apps/q4; make
#	cd bin; dlxsim  -x os.dlx.obj -a -D y -u makeprocs.dlx.obj 4; ee469_fixterminal
#	
#q5: os.dlx.obj
#	cd apps/q5; make
#	cd bin; dlxsim  -x os.dlx.obj -a -D y -u makeprocs.dlx.obj 7 2 ; ee469_fixterminal

clean:
	cd os; make clean
	cd apps/example; make clean
	cd apps/lottery_test; make clean
	cd apps/q2; make clean
#	cd apps/q3; make clean
#	cd apps/q4; make clean
#	cd apps/q5; make clean

os.dlx.obj:
	cd os; make
