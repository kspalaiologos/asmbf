package rave.pass;

import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

import rave.common.ConstructBuilder;
import rave.common.INode;
import rave.common.IPass;
import rave.common.PatternParser;
import rave.common.PatternParser.MatchResult;
import rave.construct.MultiplyLoop;
import rave.construct.primitive.Minus;
import rave.construct.primitive.PtrLeft;

public class MultiplyLoopPass implements IPass {

	private List<PatternParser> m = Arrays.asList(
			new PatternParser("[P+P-]"),
			new PatternParser("[P+P+P-]"),
			new PatternParser("[P+P+P+P-]"),
			new PatternParser("[P-P-]")
	);
	
	@Override
	public int match(List<INode> input) {
		List<MatchResult> matches = m.stream().map(m -> m.tryMatch(input))
				.filter(x -> x.hasAny).collect(Collectors.toList());
		
		if(matches.isEmpty())
			return 0;
		else {
			if(input.get(2 * matches.get(0).variableNodes.size()).getCoefficient() != 1
			|| matches.get(0).variableNodes.stream()
				.map(x -> (x instanceof PtrLeft ? -1 : 1) * x.getCoefficient())
				.reduce((x, y) -> x + y)
				.get()
			!= 0)
				return 0;
			return matches.get(0).parsedTerms;
		}
	}

	@Override
	public INode build(List<INode> input) {
		MatchResult match = m.stream().map(m -> m.tryMatch(input))
			.filter(x -> x.hasAny).collect(Collectors.toList()).get(0);
		
		INode element = new ConstructBuilder().type(MultiplyLoop.class).coefficient(1).build();
		
		element.settings().put("elements", match.variableNodes.size());
		
		IntStream.range(0, match.variableNodes.size()).forEach(idx -> {
			INode d1 = match.variableNodes.get(idx);
			INode d2 = input.get(2 * (idx + 1));
			
			element.settings().put("delta_" + idx, d1 instanceof PtrLeft ? -d1.getCoefficient() : d1.getCoefficient());
			element.settings().put("mp_delta_" + idx, d2 instanceof Minus ? -d2.getCoefficient() : d2.getCoefficient());
		});
		
		return element;
	}

}

