package rave.construct;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

import rave.common.Automata;
import rave.common.Pointer;
import rave.nodes.INode;
import rave.nodes.MorphableNode;
import rave.nodes.WithoutCoefficients;

public class MultiplyLoop extends MorphableNode implements WithoutCoefficients {

	public class Tuple<X, Y> {
		public final X x;
		public final Y y;

		public Tuple(X x, Y y) {
			this.x = x;
			this.y = y;
		}
	}

	@Override
	public void execute(Automata state) {
		Integer elements = (Integer) settings.get("elements");
		ArrayList<Tuple<Integer, Integer>> deltas = new ArrayList<Tuple<Integer, Integer>>();

		IntStream.range(0, elements).forEach(
				n -> deltas.add(
						new Tuple<Integer, Integer>(
								(Integer) settings.get("delta_" + n),
								(Integer) settings.get("mp_delta_" + n)
						)
				)
		);

		final int final_coeff = state.tape.get(state.mp.getValue());
		Pointer current_mp = new Pointer();
		current_mp.setValue(state.mp.getValue());
		
		deltas.remove(deltas.size() - 1);
		
		deltas.forEach(
				x -> {
					current_mp.advance(x.x);
					state.tape.set(
						current_mp.getValue(),
						state.tape.get(current_mp.getValue()) + final_coeff * x.y
					);
				}
		);
		
		state.tape.set(state.mp.getValue(), 0);
	}

}
