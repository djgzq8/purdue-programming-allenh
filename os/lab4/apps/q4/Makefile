default:
	cd makeprocs; make
	cd q4; make

clean:
	cd makeprocs; make clean
	cd q4; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3; ee469_fixterminal
