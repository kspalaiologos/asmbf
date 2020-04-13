package rave.pass;

import java.util.List;

import rave.common.ConstructBuilder;
import rave.common.INode;
import rave.common.IPass;
import rave.construct.primitive.PtrRight;

public class PtrRightMergePass implements IPass {

	@Override
	public int match(List<INode> input) {
		int matches = 0;
		for(INode instr : input)
			if(instr instanceof PtrRight)
				matches++;
			else
				break;
		
		return matches;
	}

	@Override
	public INode build(List<INode> input) {
		int matches = 0;
		for(INode instr : input)
			if(instr instanceof PtrRight)
				matches++;
			else
				break;
		
		return new ConstructBuilder().type(PtrRight.class).coefficient(matches).build();
	}

}
