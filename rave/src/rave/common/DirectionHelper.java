package rave.common;

import rave.construct.primitive.Minus;
import rave.construct.primitive.Plus;
import rave.construct.primitive.PtrLeft;
import rave.construct.primitive.PtrRight;

public class DirectionHelper {
	private DirectionHelper() {
		
	}
	
	public static int getRelativeDelta(INode deltaNode) {
		if(deltaNode instanceof Plus || deltaNode instanceof PtrRight) {
			return deltaNode.getCoefficient();
		} else if(deltaNode instanceof Minus || deltaNode instanceof PtrLeft) {
			return -deltaNode.getCoefficient();
		} else {
			return 0;
		}
	}
}
