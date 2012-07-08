package gpa2;

public class Matrix {
	private double[][] data;
	public int row, col;

	Matrix(int r, int c){
		data = new double[r][c];
		this.row = r;
		this.col = c;
	}
	
	public double get(int r, int c){
		return data[r][c];
	}
	
	public void set(int r, int c, double d){
		data[r][c] = d;
	}
	
	public void print(){
		for (int i = 0; i < this.row; i++){
            for (int j = 0; j < this.col; j++){
                System.out.print(this.data[i][j]);
            }
            System.out.println();
		}
	}
}
