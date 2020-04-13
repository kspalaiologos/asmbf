package rave.construct.primitive;

import rave.common.Automata;
import rave.nodes.INode;
import rave.nodes.WithoutCoefficients;
import rave.nodes.WithoutSettings;

public abstract class BracketNode implements WithoutSettings, WithoutCoefficients {
	public void match(Automata instance, int dir) {
		int level = 1;
		INode ipvalue;
		
		do {
			instance.ip.advance(dir);
			
			if(instance.ip.getValue() < 0 ||
			   instance.ip.getValue() >= instance.program.size())
				throw new RuntimeException("Unbalanced brackets!");
			
			ipvalue = instance.program.get(instance.ip.getValue());
			
			if(ipvalue instanceof BracketLeft)
				level += dir;
			
			if(ipvalue instanceof BracketRight)
				level -= dir;
		} while(level > 0);
	}
}
