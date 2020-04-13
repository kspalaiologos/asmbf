package rave;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.List;

import rave.common.INode;

public class ASTSerializer {
	public static void write(File f, List<INode> data) throws Exception {
		FileOutputStream fo = new FileOutputStream(f);
		ObjectOutputStream oo = new ObjectOutputStream(fo);
		oo.writeObject(data);
		oo.close();
		fo.close();
	}
	
	public static List<INode> read(File f) throws Exception {
		FileInputStream fi = new FileInputStream(f);
		ObjectInputStream oi = new ObjectInputStream(fi);
		List<INode> data = (List<INode>) oi.readObject();
		oi.close();
		fi.close();
		return data;
	}
}
