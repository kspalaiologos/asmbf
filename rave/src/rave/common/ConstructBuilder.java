package rave.common;

public class ConstructBuilder {
	
	private INode instance;
	
	public ConstructBuilder() {
		
	}
	
	public ConstructBuilder type(Class<? extends INode> c) {
		try {
			instance = c.newInstance();
		} catch (InstantiationException | IllegalAccessException e) {
			e.printStackTrace();
		}
		
		return this;
	}
	
	public ConstructBuilder coefficient(int c) {
		instance.setCoefficient(c);
		return this;
	}
	
	public ConstructBuilder setting(String key, Object value) {
		instance.settings().put(key, value);
		return this;
	}
	
	public INode build() {
		return instance;
	}
	
}
