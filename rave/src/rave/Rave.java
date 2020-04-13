package rave;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

public class Rave {

	public Rave(String filename) {
		File input = new File(filename);
		
		try {
			byte[] file_contents = new byte[(int) input.length()];
			FileInputStream in = new FileInputStream(input);
			in.read(file_contents);
			in.close();
			
			new Interpreter(file_contents);
		} catch (FileNotFoundException e) {
			System.err.println("Error: " + filename + " not found.");
			return;
		} catch (IOException e) {
			System.err.println("Error: Couldn't read " + filename + ".");
			return;
		}
	}
	
	public static void main(String[] args) {
		if(args.length != 1) {
			System.err.println("Error: No file supplied.");
			return;
		}
		
		new Rave(args[0]);
	}

}
