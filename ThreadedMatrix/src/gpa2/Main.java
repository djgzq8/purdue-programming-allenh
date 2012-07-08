package gpa2;

/***************************************************
 * 
 * @author Allen Humphreys 12/17/2011
 * Results
 * 
 * Elements 		400				1000			4000
 * Single			.355S			9.172S			14.86min(466MB)
 * Multi			.18S			3.371S			5.433(466MB)
 *	
 */

public class Main {
	public static void main( String[] args ) throws Exception{ 
        int N = Integer.parseInt("512");
        long start, stop;
        double elapsed;

        if (N % 4 != 0){
        	throw new Exception("The matrix has to be a multiple of 4 for this to work, for the purposes of demonstration");
        }

        // generate input
        start = System.currentTimeMillis(); 

        Matrix A = new Matrix(N, N);
        Matrix B = new Matrix(N, N);
        Matrix C = new Matrix(N, N);

        
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                A.set(i,j, 1);//(int) (Math.random()*100));
                B.set(i,j, 1);//(int) (Math.random()*100));
                C.set(i,j, 0);

            }
        }

        stop = System.currentTimeMillis();
        elapsed = (stop - start) / 1000.0;
        System.out.println("Generating input:  " + elapsed + " seconds");
        
        
        MatrixMultiply T1 = new MatrixMultiply(A, B, C, 0,		(N/4 - 1));
        MatrixMultiply T2 = new MatrixMultiply(A, B, C, (N/4),	(N/2 - 1));
        MatrixMultiply T3 = new MatrixMultiply(A, B, C, (N/2),	(N*3/4 - 1));
        MatrixMultiply T4 = new MatrixMultiply(A, B, C, (N*3/4),(N-1));
        
        start = System.currentTimeMillis();
        T1.start();
        T2.start();
        T3.start();
        T4.start();
        T1.join();
        T2.join();
        T3.join();
        T4.join();
        
       // C.print();
        stop = System.currentTimeMillis();
        elapsed = (stop - start) / 1000.0;
        System.out.println("Finished:   " + elapsed + " seconds");
	}

}