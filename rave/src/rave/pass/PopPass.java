package rave.pass;

import java.util.List;

import rave.common.ConstructBuilder;
import rave.common.DirectionHelper;
import rave.common.PatternParser;
import rave.common.PatternParser.MatchResult;
import rave.construct.StackPop;
import rave.nodes.INode;

public class PopPass implements IPass {

	private PatternParser p = new PatternParser("P[-]P[-]>[>>]<<->[<<<[<<]>+>[>>]>-]<<<[<<]>[P+P-]");
	
	public PopPass() {
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
				.type(StackPop.class)
				.coefficient(1)
				.setting("target_delta", DirectionHelper.getRelativeDelta(m.variableNodes.get(0)))
				.build();
	}

}
