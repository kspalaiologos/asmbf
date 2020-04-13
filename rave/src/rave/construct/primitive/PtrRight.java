package rave.construct.primitive;

import rave.common.Automata;
import rave.nodes.CoefficientalNode;
import rave.nodes.INode;
import rave.nodes.WithoutSettings;

public class PtrRight extends CoefficientalNode implements WithoutSettings {

	@Override
	public void execute(Automata state) {
		state.mp.advance(getCoefficient());
		
		while(state.tape.size() <= state.mp.getValue())
			state.tape.add(0);
	}

}
