package rave.construct;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import rave.common.Automata;
import rave.nodes.INode;
import rave.nodes.WithoutCoefficients;
import rave.nodes.WithoutSettings;

public class StackRV implements WithoutSettings, WithoutCoefficients {

	@Override
	public void execute(Automata state) {
		if(state.tape.size() <= 22) {
			state.tape.addAll(Collections.nCopies(20, 0));
		}
		
		List<Integer> stackMemory = state.tape.subList(21, state.tape.size());
		
		Integer i = state.sp.getValue() - 2;
		
		if(i < 4) {
			throw new Error("Stack underflow: PermaGen corruption detected.");
		}
		
		Integer old1 = stackMemory.get(i);
		Integer old2 = stackMemory.get(i - 2);
		stackMemory.set(i - 2, old1);
		stackMemory.set(i, old2);
	}

}
