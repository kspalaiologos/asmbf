package rave.construct;

import rave.common.Automata;
import rave.common.INode;

public class Clear extends WithoutSettings {

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
		state.tape.set(state.mp.getValue(), 0);
	}

}
