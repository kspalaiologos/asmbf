package rave.construct;

import java.util.HashMap;

import rave.common.INode;

public interface WithoutSettings extends INode {
	public default HashMap<String, Object> settings() {
		return null;
	}
}
