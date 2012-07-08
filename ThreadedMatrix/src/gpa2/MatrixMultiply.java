package gpa2;


class MatrixMultiply extends Thread {
	Matrix A, B, pResult;
	int si, ei;

	MatrixMultiply(Matrix A, Matrix B, Matrix pResult, int si, int ei) {
		this.A = A;
		this.B = B;
		this.pResult = pResult;
		this.si = si;
		this.ei = ei;
		

	}
	public void run( ) {
		//Do multiply, data accesses aren't thread safe
		for (int i = si; i <= ei; i++){
			for (int j = 0; j < B.col; j++){
				for (int k = 0; k < B.row; k++){
					//System.out.println(pResult.get(i,j) + " " + A.get(i,k) + " " + B.get(k, j));
					pResult.set(i, j, pResult.get(i,j) + A.get(i, k) * B.get(k, j));
				}
			}
		}
	}

}