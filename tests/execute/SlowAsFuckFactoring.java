class SlowAsFuckFactoring {
	public static void main(String[] args) {
		System.out.println(new SAFF().ret(new SAFF().ret(new SAFF().factor(1260))));
	}
}
class SAFF
{
	public int ret(int foo) {
		return foo;
	}

	public int factor(int term)
	{
		int i;
		int tmp;
		i = 2;
		while(i < term && -1 < i)
		{
			tmp = slowdiv(term, i);

			if(!(tmp * i < term))
			{
				this.factor(i);
				this.factor(tmp);
				i = -2; // break;
			}
			else
			{}
			i = i + 1;
		}
		if(-1 < i)
		{
			System.out.println(term);
		}
		else {}
		return term;
	}

	public int slowdiv(int n, int div) {
		int i;
		i = 0;
		while(div < n+1)
		{
			n  = n - div;
			i = i + 1;
		}
		return i;
	}
}
