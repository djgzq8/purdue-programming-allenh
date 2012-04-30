all: os.dlx.obj mp.dlx.obj userprog4.dlx.obj
	cd bin; dlxsim -x os.dlx.obj -a -u userprog4.dlx.obj 2 ; ee469_fixterminal
	
lottery: os.dlx.obj userprog4.dlx.obj mp.dlx.obj
	cd bin; dlxsim -x os.dlx.obj -a -D 4 -u mp.dlx.obj 2 2 ; ee469_fixterminal
	cd bin; dlxsim -x os.dlx.obj -a -D 4 -u userprog4.dlx.obj 2 ; ee469_fixterminal
	
reaction: os.dlx.obj mp.dlx.obj
	cd bin; dlxsim -x os.dlx.obj -a -u mp.dlx.obj 20 11 ; ee469_fixterminal


clean:
	cd os; make clean
	cd apps/lottery_test; make clean
	cd apps/q2; make clean


os.dlx.obj:
	cd os; make
mp.dlx.obj:
	cd apps/q2; make

userprog4.dlx.obj:
	cd apps/lottery_test; make