package rave;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;

public class Rave {
	
	public static boolean quiet = false;
	public static boolean bytecode_in = false;
	public static boolean bytecode_out = false;
	public static boolean integer_io = false;
	public static boolean compatibility = false;
	
	public static Scanner inputScanner;

	public Rave(String filename) {
		File input = new File(filename);
		
		if(bytecode_in && bytecode_out) {
			System.err.println("Error: -a and -b are exclusive.");
			return;
		}
		
		if(!bytecode_in) {
			try {
				byte[] file_contents = new byte[(int) input.length()];
				FileInputStream in = new FileInputStream(input);
				in.read(file_contents);
				in.close();
				
				new Interpreter(filename, file_contents);
			} catch (FileNotFoundException e) {
				System.err.println("Error: " + filename + " not found.");
			} catch (IOException e) {
				System.err.println("Error: Couldn't read " + filename + ".");
			}
		} else {
			try {
				new CodeRunner(ASTSerializer.read(new File(filename))).execute();
			} catch (IOException e) {
				System.err.println("Error: Couldn't load the bytecode.");
			} catch (ClassNotFoundException e) {
				System.err.println("Error: Outdated bytecode. Please regenerate it or load from source.");
			}
		}
	}
	
	public static void main(String[] args) {
		if(args.length < 1) {
			System.err.println("Error: At least one argument expected. Try -h for help.");
			return;
		}
		
		String file = null;
		
		for(int i = 0; i < args.length; i++) {
			if(args[i].charAt(0) == '-') {
				switch(args[i].charAt(1)) {
				case 'h':
					System.err.println("rave: a fast brainfuck interpreter targeted at running asm2bf code.");
					System.err.println("copyright(C) 2020 by Krzysztof Szewczyk (Palaiologos/MENACE).");
					System.err.println("");
					System.err.println("usage:");
					System.err.println(" java -jar rave.jar [-h/-b/-a/-q] program.b");
					System.err.println("");
					System.err.println("switches:");
					System.err.println(" -h\t-\tdisplay this text.");
					System.err.println(" -b\t-\tread prebuilt bytecode file.");
					System.err.println(" -a\t-\tcompile to bytecode file and exit.");
					System.err.println(" -q\t-\tshut up all the banners.");
					System.err.println(" -n\t-\tinteger i/o.");
					System.err.println(" -c\t-\tcompatible mode - 16-bit cells.");
					return;
				case 'q':
					Rave.quiet = true;
					break;
				case 'a':
					Rave.bytecode_out  = true;
					break;
				case 'b':
					Rave.bytecode_in = true;
					break;
				case 'n':
					Rave.integer_io = true;
					inputScanner = new Scanner(System.in);
					break;
				case 'c':
					Rave.compatibility = true;
					break;
				}
			} else {
				if(file != null) {
					System.err.println("Error: Multiple input files specified.");
					return;
				}
				
				file = args[i];
			}
		}
		
		new Rave(file);
	}

}
