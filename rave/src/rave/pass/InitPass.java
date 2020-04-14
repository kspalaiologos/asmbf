package rave.pass;

import java.util.List;

import rave.common.PatternParser;
import rave.nodes.INode;

public class InitPass implements IPass {

	public static Integer gotInit = 0;
	private PatternParser p = new PatternParser("+>+[");
	
	public InitPass() {
		p.setPedantic(true);
	}
	
	@Override
	public int match(List<INode> input) {
		if(gotInit != 0)
			return 0;
		
		if(p.tryMatch(input).hasAny)
			gotInit = 1;
		else {
			throw new Error("Error: This code doesn't seem to be produced with asm2bf toolkit. Aborting.");
		}
		
		return 0;
	}

	@Override
	public INode build(List<INode> input) {
		// TODO Auto-generated method stub
		return null;
	}

}
