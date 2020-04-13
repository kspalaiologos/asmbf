package rave.construct;

import java.util.Collections;
import java.util.List;

import rave.common.Automata;
import rave.nodes.MorphableNode;
import rave.nodes.WithoutCoefficients;

public class StackPop extends MorphableNode implements WithoutCoefficients {

	@Override
	public void execute(Automata state) {
		if(state.tape.size() <= 22) {
			state.tape.addAll(Collections.nCopies(20, 0));
		}
		
		List<Integer> stackMemory = state.tape.subList(21, state.tape.size());
		
		state.sp.descend();
		state.sp.descend();
		
		Integer i = state.sp.getValue();
		
		stackMemory.set(i, 0);
		Integer val = stackMemory.get(i + 1);
		
		state.tape.set(state.mp.getValue() + (Integer)settings.get("target_delta"), val);
	}

}
