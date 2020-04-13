package rave.common;

import java.io.Serializable;
import java.util.ArrayList;

import rave.nodes.INode;

public class Automata implements Serializable {
	public ArrayList<Integer> tape;
	public ArrayList<INode> program;
	public Pointer ip, mp, sp;
	
	public Automata(ArrayList<Integer> tape, ArrayList<INode> program,
			Pointer ip, Pointer mp) {
		super();
		
		this.tape = tape;
		this.program = program;
		
		this.ip = ip;
		this.mp = mp;
		
		this.sp = new Pointer();
	}
}
