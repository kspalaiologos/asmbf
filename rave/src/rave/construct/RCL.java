package rave.construct;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import rave.common.Automata;
import rave.common.INode;

public class RCL implements INode {

	private int coefficient;
	private HashMap<String, Object> settings = null;

	@Override
	public void setCoefficient(int c) {
		this.coefficient = c;
	}

	@Override
	public int getCoefficient() {
		return coefficient;
	}

	@Override
	public HashMap<String, Object> settings() {
		if (settings == null)
			settings = new HashMap<String, Object>();

		return settings;
	}

	@Override
	public void execute(Automata state) {
		if(state.tape.size() <= (Integer) settings.get("delta_mem") + 15) {
			state.tape.addAll(Collections.nCopies((Integer) settings.get("delta_mem") + 15, 0));
		}
		
		List<Integer> taperam = state.tape.subList((Integer) settings.get("delta_mem") + 15, state.tape.size());
		
		Integer realSrcDelta = (Integer) settings.get("delta_src") + (Integer) settings.get("delta_dest");
		Integer realAddress = state.tape.get(state.mp.getValue() + realSrcDelta);
		Integer value = taperam.get(realAddress * 2);
		state.tape.set((Integer) settings.get("delta_dest") + state.mp.getValue(), value);
		state.mp.setValue((Integer) settings.get("delta_mem") + 12);
		
		return;
	}

}
