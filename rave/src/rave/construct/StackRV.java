package rave.construct;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import rave.common.Automata;
import rave.common.INode;

public class StackRV extends WithoutSettings {

	private int coefficient;

	@Override
	public void setCoefficient(int c) {
		this.coefficient = c;
	}

	@Override
	public int getCoefficient() {
		return coefficient;
	}

	@Override
	public void execute(Automata state) {
		if(state.tape.size() <= 22) {
			state.tape.addAll(Collections.nCopies(20, 0));
		}
		
		List<Integer> stackMemory = state.tape.subList(21, state.tape.size());
		
		Integer i = 0;
		
		for(; i < stackMemory.size() - 1; i++) {
			if(stackMemory.get(i) != 0 || stackMemory.get(i + 1) != 0)
				break;
		}
		
		Integer old1 = stackMemory.get(i - 1);
		Integer old2 = stackMemory.get(i - 3);
		stackMemory.set(i - 3, old1);
		stackMemory.set(i - 1, old2);
	}

}
