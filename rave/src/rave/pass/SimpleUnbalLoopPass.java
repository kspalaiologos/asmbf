package rave.pass;

import java.util.List;

import rave.common.ConstructBuilder;
import rave.common.DirectionHelper;
import rave.common.PatternParser;
import rave.construct.SimpleUnbalLoop;
import rave.nodes.INode;

public class SimpleUnbalLoopPass implements IPass {

	private PatternParser pm = new PatternParser("[P]");
	
	@Override
	public int match(List<INode> input) {
		return pm.tryMatch(input).hasAny ? 3 : 0;
	}

	@Override
	public INode build(List<INode> input) {
		return new ConstructBuilder()
				.type(SimpleUnbalLoop.class)
				.coefficient(1)
				.setting("mp_delta", DirectionHelper.getRelativeDelta(pm.tryMatch(input).variableNodes.get(0)))
				.build();
	}

}
