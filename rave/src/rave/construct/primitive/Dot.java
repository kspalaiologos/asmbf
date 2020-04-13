package rave.construct.primitive;

import rave.common.Automata;
import rave.common.INode;
import rave.construct.WithoutSettings;

public class Dot implements WithoutSettings {

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
		for(int i = 0; i < coefficient; i++)
			System.out.print((char) state.tape.get(state.mp.getValue()).intValue());
	}

}
