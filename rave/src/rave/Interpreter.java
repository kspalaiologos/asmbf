package rave;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import rave.common.ConstructBuilder;
import rave.common.INode;
import rave.common.IPass;
import rave.common.Pointer;
import rave.construct.primitive.BracketLeft;
import rave.construct.primitive.BracketRight;
import rave.construct.primitive.Comma;
import rave.construct.primitive.Dot;
import rave.construct.primitive.Minus;
import rave.construct.primitive.Plus;
import rave.construct.primitive.PtrLeft;
import rave.construct.primitive.PtrRight;
import rave.pass.ClearFinderPass;
import rave.pass.MinusMergePass;
import rave.pass.MultiplyLoopPass;
import rave.pass.PlusMergePass;
import rave.pass.PopPass;
import rave.pass.PtrLeftMergePass;
import rave.pass.PtrRightMergePass;
import rave.pass.PushPass;
import rave.pass.RCLPass;
import rave.pass.RetPass;
import rave.pass.SRVPass;

public class Interpreter {

	private ArrayList<IPass> passes;
	
	public Interpreter(byte[] file_contents) {
		ArrayList<INode> base = new ArrayList<INode>();
		ArrayList<INode> program = new ArrayList<INode>();
		
		passes = new ArrayList<IPass>();
		
		int count = 0;
		
		System.err.println("[" + PerformanceCounter.nextOP() + " ms] Rave b005 starting.");
		System.err.println("Copyright (C) 2020 by Kamila Szewczyk (a.k.a. Palaiologos/MENACE)");
		
		for(byte b : file_contents) {
			if(Character.isDigit(b)) {
				count += b - '0';
				count *= 10;
			} else if("+-<>.,[]".indexOf((char) b) != -1) {
				if(count == 0)
					count++;
				else
					count /= 10;
				
				if(b == '+')      base.add(new ConstructBuilder().type(Plus.class).coefficient(count).build());
				else if(b == '-') base.add(new ConstructBuilder().type(Minus.class).coefficient(count).build());
				else if(b == '<') base.add(new ConstructBuilder().type(PtrLeft.class).coefficient(count).build());
				else if(b == '>') base.add(new ConstructBuilder().type(PtrRight.class).coefficient(count).build());
				else if(b == '.') base.add(new ConstructBuilder().type(Dot.class).coefficient(count).build());
				else if(b == ',') base.add(new ConstructBuilder().type(Comma.class).coefficient(count).build());
				else if(b == '[') base.add(new ConstructBuilder().type(BracketLeft.class).coefficient(1).build());
				else if(b == ']') base.add(new ConstructBuilder().type(BracketRight.class).coefficient(1).build());
				
				if((b == '[' || b == ']') && count != 1) {
					throw new RuntimeException("RLE Brackets not supported.");
				}
				
				count = 0;
			}
		}
		
		System.err.println("[" + PerformanceCounter.nextOP() + " ms] Done building basic AST step.");
		
		passes.add(new PlusMergePass());
		passes.add(new MinusMergePass());
		passes.add(new PtrLeftMergePass());
		passes.add(new PtrRightMergePass());
		passes.add(new RCLPass());
		passes.add(new SRVPass());
		passes.add(new RetPass());
		passes.add(new PopPass());
		passes.add(new PushPass());
		passes.add(new MultiplyLoopPass());
		passes.add(new ClearFinderPass());
		
		for(IPass p : passes) {
			// for each pass
			
			Pointer ptr = new Pointer();
			
			while(ptr.getValue() < base.size()) {
				List<INode> area = base.subList(ptr.getValue(), base.size());
				
				int size = p.match(area);
					
				if(size > 0) {
					program.add(p.build(area));
					ptr.advance(size);
				} else {
					program.add(area.get(0));
					ptr.advance();
				}
			}
			
			base = program;
			program = new ArrayList<INode>();
		}
		
		program = base;

		// Force-free the memory for base.
		base = null;
		
		System.err.println("[" + PerformanceCounter.nextOP() + " ms] Done optimizing AST.");
		
		new CodeRunner(program).execute();
		
		System.err.println("[" + PerformanceCounter.nextOP() + " ms] Done executing.");
	}

}
