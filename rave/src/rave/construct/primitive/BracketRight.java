package rave.construct.primitive;

import rave.common.Automata;

public class BracketRight extends BracketNode {

	@Override
	public void execute(Automata state) {
		match(state, -1);
		state.ip.descend();
	}

}
