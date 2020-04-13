package rave.construct.primitive;

import rave.common.Automata;
import rave.common.INode;
import rave.construct.WithoutSettings;

public class PtrRight extends WithoutSettings {

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
		state.mp.advance(coefficient);
		
		while(state.tape.size() <= state.mp.getValue())
			state.tape.add(0);
	}

}
