package rave.construct;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import rave.common.Automata;
import rave.common.INode;

public class StackRet implements WithoutSettings {

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
		
		state.sp.descend();
		state.sp.descend();
		
		Integer i = state.sp.getValue();
		
		stackMemory.set(i, 0);
		Integer val = stackMemory.get(i + 1);
		
		state.tape.set(0, 0);
		state.tape.set(1, val);
	}

}
