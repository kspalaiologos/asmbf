package rave;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

public class Rave {

	public static void main(String[] args) {
		if(args.length < 1) {
			System.err.println("Error: No file supplied.");
			return;
		}
		
		File input = new File(args[0]);
		
		try {
			byte[] file_contents = new byte[(int) input.length()];
			FileInputStream in = new FileInputStream(input);
			in.read(file_contents);
			in.close();
			
			new Interpreter(file_contents);
		} catch (FileNotFoundException e) {
			System.err.println("Error: " + args[1] + " not found.");
			return;
		} catch (IOException e) {
			System.err.println("Error: Couldn't read " + args[1] + ".");
			return;
		}
	}

}
