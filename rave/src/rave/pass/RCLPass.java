package rave.pass;

import java.util.List;

import rave.common.ConstructBuilder;
import rave.common.DirectionHelper;
import rave.common.INode;
import rave.common.IPass;
import rave.common.PatternParser;
import rave.common.PatternParser.MatchResult;
import rave.construct.RCL;
import rave.construct.StackPush;

public class RCLPass implements IPass {
	// 0: target cell offset, 1: source adress cell offset, 2: garbage, 3: start of memory array + 1
	private PatternParser p = new PatternParser("P[-]P[P+P+P+P-]P[P+P-]P[[2>1]+[2<1]2>1-]+[2>1]<[<[2<1]>+P+P[2>1]<-]<[2<1]>[>[2>1]<+<[2<1]>-]>[2>1]2<1[-2<1]");
	
	public RCLPass() {
		p.setPedantic(true);
	}
	
	@Override
	public int match(List<INode> input) {
		MatchResult m = p.tryMatch(input);
		
		return m.hasAny ? m.parsedTerms : 0;
	}

	@Override
	public INode build(List<INode> input) {
		MatchResult m = p.tryMatch(input);
		
		return new ConstructBuilder()
				.type(RCL.class)
				.coefficient(1)
				.setting("delta_dest", DirectionHelper.getRelativeDelta(m.variableNodes.get(0)))
				.setting("delta_src", DirectionHelper.getRelativeDelta(m.variableNodes.get(1)))
				.setting("delta_mem", DirectionHelper.getRelativeDelta(m.variableNodes.get(4)))
				.build();
	}

}
