all:
	cd os; make
	cd apps/fdisk; make
	cd apps/ostests; make

clean:
	cd os; make clean
	cd apps/fdisk; make clean
	cd apps/ostests; make clean

run:
	cd os; make; cd ../apps/fdisk; make; cd  ../../bin; dlxsim  -x os.dlx.obj -a -D i -u fdisk.dlx.obj #; ee469_fixterminal

ostests:
	cd os; make; cd ../apps/ostests; make; cd  ../../bin; dlxsim  -x os.dlx.obj -a -D y -u ostests.dlx.obj #; ee469_fixterminal
	
filesAPI:
	cd os; make; cd ../apps/filesAPI; make; cd  ../../bin; dlxsim  -x os.dlx.obj -a -u filesAPI.dlx.obj #; ee469_fixterminal