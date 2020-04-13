package rave.common;

import java.io.Serializable;
import java.util.HashMap;

public interface INode extends Serializable {
	public void setCoefficient(int c);
	public int getCoefficient();
	
	public HashMap<String, Object> settings();
	
	public void execute(Automata state);
}
