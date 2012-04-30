default:
	cd makeprocs; make
	cd spawn_rx1; make
	cd spawn_rx2; make
	cd spawn_rx3; make
	cd spawn_water; make
	cd spawn_sulfate; make

clean:
	cd makeprocs; make clean
	cd spawn_rx1; make clean
	cd spawn_rx2; make clean
	cd spawn_rx3; make clean
	cd spawn_water; make clean
	cd spawn_sulfate; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u mp.dlx.obj 3; ee469_fixterminal
