package rave.construct.primitive;

import rave.common.Automata;
import rave.common.INode;
import rave.construct.WithoutSettings;

public class Minus extends WithoutSettings {

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
		state.tape.set(state.mp.getValue(), state.tape.get(state.mp.getValue()) - coefficient);
	}

}
