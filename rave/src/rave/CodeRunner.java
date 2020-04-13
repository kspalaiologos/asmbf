package rave;

import java.util.ArrayList;
import java.util.Collections;

import rave.common.Automata;
import rave.common.INode;
import rave.common.Pointer;

public class CodeRunner {

	private Automata instance;
	
	public CodeRunner(ArrayList<INode> program) {
		instance = new Automata(new ArrayList<Integer>(Collections.nCopies(60, 0)), program, new Pointer(), new Pointer());
	}
	
	public void execute() {
		try {
			while(instance.ip.getValue() < instance.program.size()) {
				instance.program.get(instance.ip.getValue()).execute(instance);
				instance.ip.advance();
			}
		} catch(Exception e) {
			System.err.println("IP = " + instance.ip.getValue() + ", MP = " + instance.mp.getValue());
			e.printStackTrace();
		}
	}

}
