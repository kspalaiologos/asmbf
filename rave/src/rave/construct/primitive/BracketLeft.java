package rave.construct.primitive;

import rave.common.Automata;

public class BracketLeft extends BracketNode {

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
		if(state.tape.get(state.mp.getValue()) == 0)
			match(state, 1);
	}

}
