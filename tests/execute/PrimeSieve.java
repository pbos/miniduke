class PrimeSieve {
	public static void main(String[] args) {
		int[] primes;
		int[] sieve;
		int prime_len;
		int i;
		int j;

		sieve = new int[100];

		i = 2;
		while(i < 100){
			if(sieve[i] == 0){
				System.out.println(i);
				j = 2 * i;
				while(j < 100) {
					sieve[j] = 1;
					j = j + i;
				}
			}
			i = i + 1;
		}
	}
}
