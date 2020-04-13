package rave.construct;

import rave.common.Automata;
import rave.nodes.INode;
import rave.nodes.WithoutCoefficients;
import rave.nodes.WithoutSettings;

public class Clear implements WithoutSettings, WithoutCoefficients {
	
	@Override
	public void execute(Automata state) {
		state.tape.set(state.mp.getValue(), 0);
	}

}
