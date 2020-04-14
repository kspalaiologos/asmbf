package rave.construct;

import java.util.Collections;

import rave.common.Automata;
import rave.nodes.MorphableNode;
import rave.nodes.WithoutCoefficients;

public class SimpleUnbalLoop extends MorphableNode implements WithoutCoefficients {

	@Override
	public void execute(Automata state) {
		Integer delta = (Integer) settings().get("mp_delta");
		
		while(state.tape.get(state.mp.getValue()) != 0) {
			state.mp.advance(delta);
			
			while(state.tape.size() <= state.mp.getValue())
				state.tape.addAll(Collections.nCopies(delta, 0));
		}
	}

}
