package rave.construct.primitive;

import rave.Rave;
import rave.common.Automata;
import rave.nodes.CoefficientalNode;
import rave.nodes.WithoutSettings;

public class Dot extends CoefficientalNode implements WithoutSettings {

	@Override
	public void execute(Automata state) {
		for(int i = 0; i < getCoefficient(); i++)
			if(!Rave.integer_io)
				System.out.print((char) state.tape.get(state.mp.getValue()).intValue());
			else
				System.out.print((int) state.tape.get(state.mp.getValue()).intValue());
	}

}
