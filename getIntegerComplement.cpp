#include <iostream>
using namespace std;
int getIntegerComplement(unsigned int N) {  // eg:  N= 00000000000000000000000000010001
    if(N == 0) return 0;    
    unsigned int m = 1<<31;                 //      m= 10000000000000000000000000000000
    while((m&N) == 0)
        m >>= 1;                            // now  m= 00000000000000000000000000100000
    m <<= 1;                                // now  m= 00000000000000000000000001000000
    m -= 1;                                 // now  m= 00000000000000000000000000111111
    N ^= m;                                 // now, use XOR 1, since 1^1=0, 0^1=1.
    return N;                               // now  N= 00000000000000000000000000001110
}
int main() {
	cout<<getIntegerComplement(50);
	return 0;
}
