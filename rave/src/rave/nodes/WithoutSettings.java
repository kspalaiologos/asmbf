package rave.nodes;

import java.util.HashMap;

public interface WithoutSettings extends INode {
	public default HashMap<String, Object> settings() {
		return null;
	}
}
