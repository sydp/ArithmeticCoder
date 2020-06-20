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
	//string line;
	while (1) {
		string line;
		getline(cin, line);
		////cerr << line;
		int index = line.find(delimiter);
		if (index == 1 or index == 0) {
	 		symbol = line[0];
	 		code = line.substr(2);
	 		counter[symbol.c_str()[0]] = stoi(code);
	 		total += stoi(code);
	 	}
	 	else if (index == -1) {
	 		cout << "asfd";
			code = line;
	 		break;
	 	}
	 	else {
	 		//cout << "dfgd" << index;
	 		code = line.substr(0, index);
	 		//cerr << line << " " << code << " " << index << endl;
	 		break;
	 	}
	}

	//cerr << code << " " << total << endl;
	for (auto pair: counter) {
		probabilities[pair.first] = (double)pair.second / total;
	}
	
	double cum_total = 0.0;
	for (auto pair: probabilities) {
		double second = cum_total + pair.second;
		ranges[pair.first] = make_pair(cum_total, second);
		cum_total += pair.second;
	}

	mpfr_rnd_t rnd = MPFR_RNDU;
	mpfr_t kode;

	int l = total*8; //ceil(code.length()*log10(2));
	mpfr_init2(kode, l);
	mpfr_set_str(kode, code.c_str(), 10, rnd);
	//mpfr_out_str(stderr, 10, 0, kode, rnd);
	//mpfr_out_str(stdout, 10, 0, kode, rnd);
	//cout << endl;

	mpfr_t first, second;
			
	while (total) {
		//if (mpfr_zero_p(kode)) 
		//	break;

		for (auto pair: ranges) {
			mpfr_init2(first, l);
			mpfr_init2(second, l);
			mpfr_set_str(first, to_string(pair.second.first).c_str(), 10, rnd);
			mpfr_set_str(second, to_string(pair.second.second).c_str(), 10, rnd);
			// //cerr << "kode ";
			// 	mpfr_out_str(stderr, 10, 0, kode, rnd);
			// 	//cerr << endl;
			// 	mpfr_out_str(stderr, 10, 0, first, rnd);
			// 	//cerr << " ";
			// 	mpfr_out_str(stderr, 10, 0, second, rnd);
			// 	//cerr << endl;
			// find symbol whose range straddles the encoded number
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
				//cerr << "temp ";
				//mpfr_out_str(stderr, 10, 0, temp, rnd);

				// divide encoded number by range
				mpfr_div(kode, kode, temp, rnd);
				
				break;
			}

		}
		//cerr << "total " << total << endl;
		total--;
	}	
	mpfr_clears(first, second, kode, (mpfr_ptr) 0);
}
