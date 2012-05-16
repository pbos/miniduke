class ThisInMain{
	public static void main(String[] args)
	{
		int i;
		// ThisInMain.java:6: non-static variable this cannot be referenced from a static context
		i = new Foo().foo(this);
	}
}

class Foo {
	public int foo(ThisInMain foo)
	{
		return 5;
	}
}
