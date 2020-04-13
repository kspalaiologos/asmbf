package rave;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;

import rave.nodes.INode;

public class ASTSerializer {
	public static void write(File f, ArrayList<INode> data) throws IOException {
		FileOutputStream fo = new FileOutputStream(f);
		ObjectOutputStream oo = new ObjectOutputStream(fo);
		oo.writeObject(data);
		oo.close();
		fo.close();
	}
	
	public static ArrayList<INode> read(File f) throws IOException, ClassNotFoundException {
		FileInputStream fi = new FileInputStream(f);
		ObjectInputStream oi = new ObjectInputStream(fi);
		ArrayList<INode> data = (ArrayList<INode>) oi.readObject();
		oi.close();
		fi.close();
		return data;
	}
}
