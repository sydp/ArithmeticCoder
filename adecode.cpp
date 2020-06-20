#include <iostream>
#include <map>
#include <limits>
#include <iomanip>
#include <gmp.h>
#include <mpfr.h>
#include <cmath>
using namespace std;

int main() {

	map<char, int> counter;
	map<char, double> probabilities;
	map<char, pair<double, double>> ranges;

	string delimiter = " ";
	string code;
	string symbol, count;
	int total = 0;
	
	// parse the lines of input,
	// the symbol model (character and frequency) is output line by line
	// followed by the AC value (and optional text that can be ignored)
	while (1) {
		string line;
		getline(cin, line);
		int index = line.find(delimiter);
		if (index == 1 or index == 0) {	// symbol
	 		symbol = line[0];
	 		code = line.substr(2);
	 		counter[symbol.c_str()[0]] = stoi(code);
	 		total += stoi(code);
	 	}
	 	else if (index == -1) { // no space - this is the AC value
	 		code = line;
	 		break;
	 	}
	 	else {	// there is optional text after the AC value
	 		code = line.substr(0, index);
	 		break;
	 	}
	}

	// compute the letter probabilities
	for (auto pair: counter) {
		probabilities[pair.first] = (double)pair.second / total;
	}
	
	// compute the letter low-high probability range
	double cum_total = 0.0;
	for (auto pair: probabilities) {
		double second = cum_total + pair.second;
		ranges[pair.first] = make_pair(cum_total, second);
		cum_total += pair.second;
	}

	mpfr_rnd_t rnd = MPFR_RNDU;
	mpfr_t kode;

	// use a precision of the number of characters * 8
	int l = total*8; 
	mpfr_init2(kode, l);
	mpfr_set_str(kode, code.c_str(), 10, rnd);
	
	mpfr_t first, second;
	mpfr_init2(first, l);
	mpfr_init2(second, l);
			
	// loop until we output characters of length "total model frequency"
	while (total) {	
		for (auto pair: ranges) {
			mpfr_set_str(first, to_string(pair.second.first).c_str(), 10, rnd);
			mpfr_set_str(second, to_string(pair.second.second).c_str(), 10, rnd);
	
			// low <= AC < high		
			if (mpfr_greaterequal_p(kode, first) && mpfr_less_p(kode, second)) {
				
				// output the symbol
				cout << pair.first;

				// range = symbol high value - symbol low value
				double x = pair.second.second - pair.second.first;

				// subtract symbol low value from encoded number
				mpfr_sub(kode, kode, first, rnd);

				mpfr_t temp;
				mpfr_init2(temp, l);
				mpfr_sub(temp, second, first, rnd);
				
				// divide encoded number by range
				mpfr_div(kode, kode, temp, rnd);
				mpfr_clear(temp);

				break;
			}

		}
		total--;
	}	
	mpfr_clears(first, second, kode, (mpfr_ptr) 0);
}
