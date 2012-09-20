package gui;

import threadmanager.ThreadService;
import objects.SudokuGameBoard;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.*;
import javax.swing.border.*;


import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import java.util.concurrent.*;

@SuppressWarnings("serial")
public class MainWindow extends JFrame implements ActionListener{
	static private final String newline = "\n";
	JMenuItem mntmOpen;
	JMenu mnFile;
	JFileChooser fc;
	private JTextArea log;
	static SudokuGameBoard game1;
	private JTextField textField;
	private JButton solveButton;
	/**
	 * Launch the application.
	 */
	//	public static ThreadService Threads =  (ThreadService) Executors.newFixedThreadPool(5);


	public static void main(String[] args) {
		game1 = new SudokuGameBoard();
		game1.start();
		//game1.create(input);
		//game1.solve();
		
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				UIManager.put("swing.boldMetal", Boolean.FALSE);


				try {
					MainWindow window = new MainWindow();
					window.setTitle("Sudoku Solver");
					window.setBounds(100, 100, 450, 300);
					window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
					window.pack();
					window.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public MainWindow() {
		
		setMinimumSize(new Dimension(400, 400));
		getContentPane().setMinimumSize(new Dimension(400, 400));
		setFocusTraversalKeysEnabled(false);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		//		super(new BorderLayout());
		initialize();
	}

	
	public void paint(Graphics g){
//	java.awt.color
		Graphics2D g2 = (Graphics2D)g;
		
	}
	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		fc = new JFileChooser();
		
		JMenuBar menuBar = new JMenuBar();
		this.setJMenuBar(menuBar);

		mnFile = new JMenu("File");
		menuBar.add(mnFile);

		mntmOpen = new JMenuItem("Open");
		mntmOpen.addActionListener(this);
		mnFile.add(mntmOpen);
		getContentPane().setLayout(new GridLayout(9, 9, 0, 0));
		//		frmSudokuSolver.getContentPane().setLayout(new BorderLayout(0, 0));

		log = new JTextArea();
		log.setBorder(new BevelBorder(BevelBorder.RAISED, null, null, null, null));
		log.setFocusable(false);
		log.setPreferredSize(new Dimension(5, 20));
		log.setSize(new Dimension(5, 20));
		log.setMargin(new Insets(5,5,5,5));
		log.setToolTipText("this is were debugging information will print");
		log.setEditable(false);
		JScrollPane logScrollPane = new JScrollPane(log);
		getContentPane().add(logScrollPane);

		(textField = new JTextField()).setText("1");
		//		textField.setText("1");
		getContentPane().add(textField);
		textField.setColumns(10);

		solveButton = new JButton("Solve!");
		solveButton.addActionListener(this) ;
		getContentPane().add(solveButton);
		//		frmSudokuSolver.getContentPane().add(log);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == mntmOpen){
			BufferedReader TextReader = null;
			System.out.println("Open Button Was Pressed");
			fc.setCurrentDirectory(new File("."));
			
			int returnVal = fc.showOpenDialog(MainWindow.this);
			 
			if (returnVal == JFileChooser.APPROVE_OPTION) {
				try {
					TextReader = new BufferedReader(new FileReader(fc.getSelectedFile()));
					game1.initialize(TextReader);
				} catch (FileNotFoundException e1) {
					System.err.println("File " + fc.getSelectedFile().getName() + " does not exist");
					e1.printStackTrace();
				} 


				log.append("Opening: " + fc.getSelectedFile().getName() + "." + newline);
				//TODO: Figure out how people typically rely on a thread to do their bidding
				//without continually starting it??
			} else if (returnVal == JFileChooser.CANCEL_OPTION){
				System.exit(-2);
			}
			else {
				log.append("Open command cancelled by user." + newline);
			}
			log.setCaretPosition(log.getDocument().getLength());
		}
		else if (e.getSource() == solveButton){
			synchronized (game1) {
				game1.notify();
			}
			System.out.println("Solve Button was pressed");
		}
	}

}
