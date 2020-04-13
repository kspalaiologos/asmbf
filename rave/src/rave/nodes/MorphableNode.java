package rave.nodes;

import java.util.HashMap;

public abstract class MorphableNode implements INode {

	protected HashMap<String, Object> settings = null;

	@Override
	public HashMap<String, Object> settings() {
		if (settings == null)
			settings = new HashMap<String, Object>();

		return settings;
	}

}
