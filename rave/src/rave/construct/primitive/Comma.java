package rave.construct.primitive;

import java.io.IOException;

import rave.common.Automata;
import rave.common.INode;
import rave.construct.WithoutSettings;

public class Comma extends WithoutSettings {

	private int coefficient;
	
	@Override
	public void setCoefficient(int c) {
		coefficient = c;
	}

	@Override
	public int getCoefficient() {
		return coefficient;
	}

	@Override
	public void execute(Automata state) {
		int c = 0;
		
		try {
			for(int i = 0; i < coefficient; i++)
				c = System.in.read();
		} catch (IOException e) {
			c = 0;
		}
		
		if(c < 0)
			c = 0;
		
		state.tape.set(state.mp.getValue(), c);
	}

}
