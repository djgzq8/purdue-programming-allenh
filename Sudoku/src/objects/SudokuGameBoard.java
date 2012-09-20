package objects;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

import javax.swing.JFileChooser;

public class SudokuGameBoard extends Thread {
	private ArrayList<SudokuCell> cells = new ArrayList<SudokuCell>();
	private SudokuRow[] rows = new SudokuRow[9];
	private SudokuCol[] cols = new SudokuCol[9];
	private SudokuCube[][] cubes = new SudokuCube[3][3];

	public int[][] test
	= {
			{6, 0, 4, 0, 0, 0, 8, 0, 5}, 
			{0, 5, 0, 0, 4, 0, 0, 0, 1}, 
			{0, 0, 8, 5, 3, 0, 0, 9, 0}, 
			{9, 0, 1, 0, 5, 0, 2, 7, 0}, 
			{8, 0, 2, 0, 0, 0, 5, 0, 6}, 
			{0, 4, 3, 0, 7, 0, 9, 0, 8}, 
			{0, 8, 0, 0, 1, 9, 7, 0, 0}, 
			{7, 0, 0, 0, 6, 0, 0, 8, 0}, 
			{2, 0, 6, 0, 0, 0, 1, 0, 9}
	};

	public int[][] test2
	= {
			{3, 6, 0, 0, 0, 7, 0, 0, 0}, 
			{0, 0, 8, 5, 6, 0, 0, 0, 4}, 
			{0, 2, 0, 0, 0, 0, 6, 0, 0}, 
			{0, 1, 0, 3, 0, 0, 0, 8, 0}, 
			{0, 3, 0, 0, 5, 0, 0, 6, 0}, 
			{0, 8, 0, 0, 0, 4, 0, 9, 0}, 
			{0, 0, 7, 0, 0, 0, 0, 4, 0}, 
			{6, 0, 0, 0, 8, 3, 9, 0, 0}, 
			{0, 0, 0, 7, 0, 0, 0, 5, 1}
	};

	public void run(){
		System.out.println(this.getState());
		for (;;){
			try {
				synchronized (this){
					wait();

				}
				this.solve();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public SudokuGameBoard(){
		//creates and initializes 9 rows
		for (int i = 0; i < 9; i++){
			rows[i] = new SudokuRow(i);
		}
		//creates and initializes 9 columns
		for (int i = 0; i < 9; i++){
			cols[i] = new SudokuCol(i);
		}
		//creates and initializes 9 subcubes
		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 3; j++){
				cubes[i][j] = new SudokuCube(i, j);
			}
		}
		ArrayList<Integer> a = new ArrayList<Integer>();
		a.add(1);
		a.add(1);
		System.out.println(a);
		ArrayList<Integer> b = new ArrayList<Integer>(new HashSet<Integer>(a));
		System.out.println(b);


	}

	public class SudokuRow{
		public int rowNumber;
		public ArrayList<SudokuCell> cells = new ArrayList<SudokuCell>();
		public MySet elim = new MySet();

		SudokuRow(int num){

			this.rowNumber = num;
			for (int i = 1; i <= 9; i++){
				elim.add(new Integer(i));
			}
		}

		public void add(SudokuCell sudokuCell)
		{
			cells.add(sudokuCell);
			if (sudokuCell.cellValue > 0){
				elim.remove(Integer.valueOf(sudokuCell.cellValue));
			}
		}

		public void print()
		{
			for (int i = 0; i < 9; i++){
				System.out.print(this.cells.get(i).cellValue);
			}
		}

		public SudokuCell get(int col){
			return cells.get(col);
		}

		public void setValue(int j, int intValue)
		{
			cells.get(j).setValue(intValue);

		}
	}

	public class SudokuCol{
		public int columnNumber;
		private ArrayList<SudokuCell> cells = new ArrayList<SudokuCell>();
		public MySet elim = new MySet();

		SudokuCol(int num){
			this.columnNumber = num;
			for (int i = 1; i <= 9; i++){
				elim.add(new Integer(i));
			}
		}

		public void add(SudokuCell sudokuCell)
		{
			cells.add(sudokuCell);
			if (sudokuCell.cellValue > 0){
				elim.remove(Integer.valueOf(sudokuCell.cellValue));
			}
		}

		public SudokuCell get(int row){
			return cells.get(row);
		}
	}

	public class SudokuCube {
		private SudokuCell[][] cells = new SudokuCell[3][3];;
		public MySet elim = new MySet();
		public int columnNumber;
		public int rowNumber;

		SudokuCube(int c, int r) {
			columnNumber = c;
			rowNumber = r;
			for (int i = 1; i <= 9; i++){
				elim.add(new Integer(i));
			}
		};

		//		public void setCell(int row, int col, int val){
		//			cells[row][col].setValue(val);
		//		}

		public void add(int rowNumber, int columnNumber, SudokuCell sudokuCell)
		{
			cells[rowNumber%3][columnNumber%3] = sudokuCell;
			if (sudokuCell.cellValue > 0) {
				elim.remove(Integer.valueOf(sudokuCell.cellValue));
			}
		}

		public SudokuCell get(int r, int c){
			return cells[r%3][c%3];
		}

		public void print(){

			for (int i = 0; i < 3; i++){
				for (int j = 0; j < 3; j++){

					//					if (cells[i][j] == null){
					//						System.out.println(cells[i][j]);
					//					}else {
					//						System.out.println(cells[i][j].cellValue);
					//					}
					System.out.print(cells[i][j].cellValue);
				}
				System.out.println();
			}
		}
	}

	public class SudokuCell {
		//pointers to boarding cells
		private SudokuRow row = null;
		private SudokuCol col = null;
		private SudokuCube cube = null;

		int cellValue = -1;

		SudokuCell(SudokuRow trow, SudokuCol tcol, SudokuCube tcube, int tval){
			if (trow == null || tcol == null || tcube == null){
				System.err.println("Something isn't right!");
				System.exit(-1);
			}

			this.cellValue = tval;

			this.row = trow;
			this.row.add(this);
			this.col = tcol;
			this.col.add(this);
			this.cube = tcube;
			//			System.out.println("cube index: " + tcube.rowNumber + tcube.columnNumber);
			cube.add(trow.rowNumber, tcol.columnNumber, this);
		};


		public void setValue(int val)
		{
			this.cellValue = val;
			this.row.elim.remove(Integer.valueOf(this.cellValue));
			this.col.elim.remove(Integer.valueOf(this.cellValue));
			this.cube.elim.remove(Integer.valueOf(this.cellValue));
		}
	}

	@SuppressWarnings("serial")
	public class MySet extends TreeSet<Integer> {
		/**
		 * 
		 */
		//private static final long	serialVersionUID	= 7796630520488069051L;

		MySet(){

		}

		public MySet intersection(MySet b){
			MySet c = new MySet();
			Iterator<Integer> iter = b.iterator();
			//			System.out.println("iterator: " + iter.next().toString());
			Integer temp = null;
			//			temp = iter.
			while (iter.hasNext()){
				temp = iter.next();
				if (this.contains(temp)){
					c.add(temp);
				}

			}


			return c;
		}
	}

	public void solve(){


		for (int i = 0; i < 9; i++){
			for (int j = 0; j < 9; j++){	
			}
		}
		for (int k = 0; k < 10; k++){
			this.print();
			
			try {
				Thread.sleep(500);
			}
			catch (InterruptedException fuckup) {
				// TODO Auto-generated catch block
				fuckup.printStackTrace();
			}
			for (int i = 0; i < 9; i++){
				for (int j = 0; j < 9; j++){

					if (rows[i].get(j).cellValue == 0){
						MySet sol = cubes[i/3][j/3].elim.intersection(rows[i].elim.intersection(cols[j].elim));
						if (sol.size() == 1){
							//							System.out.print("Sol["+(i+1)+","+(j+1)+"]:");
							//							System.out.print(" " + sol + " ");
							rows[i].setValue(j,sol.first().intValue());

						}

					}else {
						//					System.out.print(" <"+rows[i].get(j).cellValue+"> ");
					}
				}
				//				System.out.println();
			}
		}

		//		for (int i = 0; i < 3; i++){
		//			for (int j = 0; j < 3; j++){
		//				cubes[i][j].print();
		//			}
		//			System.out.println();
		//		}
		//		for (int i = 0; i < 9; i++){
		//			rows[i].print();
		//			System.out.println();
		//		}
		//		Integer temp = null;
		//		for (int i = 0; i < 9; i++){
		//
		//			for (int j = 0; j < 9; j++){
		//				temp = cols[j].elim.pollFirst();
		//				if (temp == null){
		//					System.out.print( "  ");
		//				} else {
		//					System.out.print(temp + " ");
		//				}
		//			}
		//			System.out.println();
		//		}
	}

	public void print(){
		for (int i = 0; i < 9; i++){
			for (int j = 0; j < 9; j++){
				System.out.print(rows[i].cells.get(j).cellValue + " ");
			}
			System.out.println();
		}
		System.out.println();
	}

	public void initialize(BufferedReader TextReader){
		String[] row;// = new String[10]; 
		String line;
		try {
			int i = 0;
			while((line = TextReader.readLine()) != null){
				row = line.split("\\s");
				for (int j = 0; j<row.length; j++){
//					System.out.print(row[j].trim());
					cells.add(new SudokuCell(rows[i], cols[j], cubes[i/3][j/3], Integer.valueOf(row[j].trim()) )  );
				}
				System.out.println();
				i++;
			}
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		this.print();


		//				java.util.regex.Pattern re = java.util.regex.Pattern.compile("");
		//				java.util.regex.Matcher matcher = 
		//			            re.matcher();

		//				boolean found = false;
		//	            while (matcher.find()) {
		////	                console.format("I found the text" +
		//	                    " \"%s\" starting at " +
		//	                    "index %d and ending at index %d.%n",
		//	                    matcher.group(),
		//	                    matcher.start(),
		//	                    matcher.end());
		//	                found = true;
		//	            }


		//cells is sort of a master location for the cells
		//this double loop creates all of the cells with the test values and adds them to
		//the cell array
		//TODO: This is bad because the idea should be to make it possible to add the stuff
		//later... or something
//		for (int i = 0; i < 9; i++){
//			for (int j = 0; j < 9; j++){
//				cells.add(new SudokuCell(rows[i], cols[j], cubes[i/3][j/3], test[i][j]));
//			}
//		}

	}
}


