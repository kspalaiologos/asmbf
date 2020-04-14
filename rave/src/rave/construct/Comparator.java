package rave.construct;

import java.util.function.BinaryOperator;

import rave.common.Automata;
import rave.nodes.MorphableNode;
import rave.nodes.WithoutCoefficients;

public class Comparator extends MorphableNode implements WithoutCoefficients {

	@Override
	public void execute(Automata state) {
		BinaryOperator<Integer> comparator = (BinaryOperator<Integer>) settings().get("comparator");
		Integer d1 = (Integer) settings.get("delta_1"), d2 = (Integer) settings.get("delta_2");
		Integer abs_mp = (Integer) settings.get("abs_mp");
		
		Integer result = comparator.apply(state.tape.get(state.mp.getValue() + d2 + d1), state.tape.get(state.mp.getValue() + d1));
		
		state.tape.set(state.mp.getValue() + d1, result);
		state.mp.setValue(abs_mp);
	}

}
