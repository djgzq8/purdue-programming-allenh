default:
	cd makeprocs; make
	cd q3; make

clean:
	cd makeprocs; make clean
	cd q3; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3; ee469_fixterminal
