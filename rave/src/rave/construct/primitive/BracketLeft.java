package rave.construct.primitive;

import rave.common.Automata;

public class BracketLeft extends BracketNode {

	@Override
	public void execute(Automata state) {
		if(state.tape.get(state.mp.getValue()) == 0)
			match(state, 1);
	}

}
