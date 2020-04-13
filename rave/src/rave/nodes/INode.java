package rave.nodes;

import java.io.Serializable;
import java.util.HashMap;

import rave.common.Automata;

public interface INode extends Serializable {
	public void setCoefficient(int c);
	public int getCoefficient();
	
	public HashMap<String, Object> settings();
	
	public void execute(Automata state);
}
