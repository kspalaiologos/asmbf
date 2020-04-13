package rave.pass;

import java.util.List;

import rave.common.ConstructBuilder;
import rave.common.INode;
import rave.common.IPass;
import rave.common.PatternParser;
import rave.common.PatternParser.MatchResult;
import rave.construct.StackRV;

public class SRVPass implements IPass {

	private PatternParser p = new PatternParser("P[-]>[>>]<<-<[>+<-]>>[<<+>>-]<[>+<-]+[<<]>");
	
	public SRVPass() {
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
				.type(StackRV.class)
				.coefficient(1)
				.build();
	}

}
