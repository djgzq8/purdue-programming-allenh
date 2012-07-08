package gui;

import java.awt.EventQueue;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JScrollPane;
import javax.swing.UIManager;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.File;
import javax.swing.JTextArea;
import java.awt.Dimension;
import java.awt.Insets;

import javax.swing.border.BevelBorder;

import objects.SudokuGameBoard;
import java.awt.GridLayout;
import javax.swing.JTextField;

public class MainWindow extends JFrame implements ActionListener{
	static private final String newline = "\n";
	JMenuItem mntmOpen;
	JMenu mnFile;
	JFileChooser fc;
	private JTextArea log;
	static SudokuGameBoard game1;
	private JTextField textField;
	/**
	 * Launch the application.
	 */

	public static void main(String[] args) {
		game1 = new SudokuGameBoard();
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
		
		textField = new JTextField();
		textField.setText("1");
		getContentPane().add(textField);
		textField.setColumns(10);
//		frmSudokuSolver.getContentPane().add(log);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == mntmOpen){
			System.out.println("Open Button Was Pressed");
			int returnVal = fc.showOpenDialog(MainWindow.this);

			if (returnVal == JFileChooser.APPROVE_OPTION) {
				File file = fc.getSelectedFile();
				//This is where a real application would open the file.
				log.append("Opening: " + file.getName() + "." + newline);
				game1.print();
			} else {
				log.append("Open command cancelled by user." + newline);
			}
			log.setCaretPosition(log.getDocument().getLength());
		}
	}

}
