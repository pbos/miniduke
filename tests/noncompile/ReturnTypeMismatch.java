class ReturnTypeMismatch {
	public static void main(String[] args) {
		System.out.println(new Foo().bar());
	}
}

class Foo {
	public int bar() {
		// ReturnTypeMismatch.java:10: incompatible types
		return 5 < 2;
	}
}
