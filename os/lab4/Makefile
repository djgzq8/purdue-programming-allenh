all:
	cd os; make

clean:
	cd os; make clean

q2:
	cd os; 
	make; 
	cd ../apps/q2; 
	make; 
	cd  ../../bin; 
	dlxsim  -x os.dlx.obj -a -D y -u q2.dlx.obj #; ee469_fixterminal