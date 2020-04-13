package rave.construct.primitive;

import rave.common.Automata;

public class BracketRight extends BracketNode {

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
		match(state, -1);
		state.ip.descend();
	}

}
