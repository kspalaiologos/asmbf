package rave.construct;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import rave.common.Automata;
import rave.nodes.INode;
import rave.nodes.WithoutCoefficients;
import rave.nodes.WithoutSettings;

public class StackRet implements WithoutSettings, WithoutCoefficients {

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
		
		state.tape.set(0, 0);
		state.tape.set(1, val);
	}

}
