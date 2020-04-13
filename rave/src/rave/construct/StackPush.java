package rave.construct;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import rave.common.Automata;
import rave.nodes.INode;
import rave.nodes.WithoutCoefficients;

public class StackPush implements WithoutCoefficients {

	private HashMap<String, Object> settings = null;

	@Override
	public HashMap<String, Object> settings() {
		if (settings == null)
			settings = new HashMap<String, Object>();

		return settings;
	}

	@Override
	public void execute(Automata state) {
		if(state.tape.size() <= 22) {
			state.tape.addAll(Collections.nCopies(20, 0));
		}
		
		List<Integer> stackMemory = state.tape.subList(21, state.tape.size());
		
		Integer i = state.sp.getValue();
		
		stackMemory.set(i, 1);
		stackMemory.set(i + 1, state.tape.get(state.mp.getValue() + (Integer)settings.get("target_delta")));
		
		state.sp.advance(2);
	}

}
