package rave.construct;

import java.util.Collections;
import java.util.List;

import rave.common.Automata;
import rave.nodes.MorphableNode;
import rave.nodes.WithoutCoefficients;

public class RCL extends MorphableNode implements WithoutCoefficients {

	@Override
	public void execute(Automata state) {
		if(state.tape.size() <= (Integer) settings.get("delta_mem") + 15) {
			state.tape.addAll(Collections.nCopies((Integer) settings.get("delta_mem") + 15, 0));
		}
		
		List<Integer> taperam = state.tape.subList((Integer) settings.get("delta_mem") + 15, state.tape.size());
		
		Integer realSrcDelta = (Integer) settings.get("delta_src") + (Integer) settings.get("delta_dest");
		Integer realAddress = state.tape.get(state.mp.getValue() + realSrcDelta);
		
		if(realAddress < 0) {
			throw new Error("Segmentation Fault: PermaGen-TAPERAM origin corruption detected.");
		}
		
		Integer value = taperam.get(realAddress * 2);
		state.tape.set((Integer) settings.get("delta_dest") + state.mp.getValue(), value);
		state.mp.setValue((Integer) settings.get("delta_mem") + 12);
		
		return;
	}

}
