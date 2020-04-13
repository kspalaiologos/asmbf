package rave.construct.primitive;

import rave.common.Automata;
import rave.nodes.CoefficientalNode;
import rave.nodes.INode;
import rave.nodes.WithoutSettings;

public class Plus extends CoefficientalNode implements WithoutSettings {

	@Override
	public void execute(Automata state) {
		state.tape.set(state.mp.getValue(), state.tape.get(state.mp.getValue()) + getCoefficient());
	}

}
