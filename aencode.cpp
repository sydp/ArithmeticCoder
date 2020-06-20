#include <iostream>
#include <map>
#include <limits>
#include <iomanip>
#include <gmp.h>
#include <mpfr.h>
#include <cmath>
using namespace std;

// TODO: edge case - 0.0 
void print_out(mpfr_t &low, mpfr_t &high, int l) {

	char low_buffer[4096] = { 0 }; //new char[4096];
	char high_buffer[4096] = { 0 }; // = new char[4096];
	//cerr << l;
	int pr = 4096; //ceil(l*2);
	string fmt = "%." + to_string(pr) + "Rf";
 	mpfr_sprintf(low_buffer, fmt.c_str(), low);
	mpfr_sprintf(high_buffer, fmt.c_str(), high);
	int offset = 0;
	for (int i = 2; i < 2048; i++) {
		if (low_buffer[i] != high_buffer[i]) {
			offset = i;
			break;
		}
	}
	//cerr << offset << endl;
	//cout << offset << endl;
	//low_buffer[offset + 1] = 0x00;
	//high_buffer[offset + 1] = 0x00;
	mpfr_t final_low, final_high;
	mpfr_init2(final_low, l);
	mpfr_init2(final_high, l);

	mpfr_set_str(final_low, string(low_buffer).substr(0, offset + 2).c_str(), 10, MPFR_RNDU);

	mpfr_set_str(final_high, string(high_buffer).substr(0, offset + 2).c_str(), 10, MPFR_RNDU);
	
	if (low_buffer[offset + 4] != '0') {
		mpfr_t r;
		mpfr_init2(r, l);
		string f;
		f = string("1.0e-") + to_string(offset);
		mpfr_set_str(r, f.c_str(), 10, MPFR_RNDU);
		mpfr_add(final_low, final_low, r, MPFR_RNDU);
		mpfr_clear(r);
	}
	if (high_buffer[offset + 4] != '0') {
		mpfr_t r;
		mpfr_init2(r, l);
		string f;
		f = string("1.0e-") + to_string(offset);
		mpfr_set_str(r, f.c_str(), 10, MPFR_RNDU);
		mpfr_add(final_high, final_high, r, MPFR_RNDU);
		mpfr_clear(r);
	}
	mpfr_sprintf(low_buffer, fmt.c_str(), final_low);
	mpfr_sprintf(high_buffer, fmt.c_str(), final_high);
	
	mpfr_clear(final_low);
	mpfr_clear(final_high);
	mpfr_free_cache();
	//mpfr_out_str(stdout, 10, 0, final_low, rnd);
	//cout << endl;
	//mpfr_out_str(stdout, 10, 0, final_high, rnd);
	//cout << endl;
	//printf("%s %s\n", low_buffer, high_buffer);
	cout << string(low_buffer).substr(0, offset + 3) << " ";
	cout << string(high_buffer).substr(0, offset + 3) << endl;

}

int main() {
	map<char, int> counter;
	map<char, double> probabilities;
	map<char, pair<double, double>> ranges;
	string line;
	
	getline(cin, line);
	
	// Get letter frequencies
	for (int i = 0; i < line.length(); i++) {
	//	cout << line[i] << endl;
		counter[line[i]]++;
	}

	// compute letter probabilities
	for (auto pair: counter) {
		probabilities[pair.first] = (double)pair.second / line.length();
		cout << pair.first << " " << pair.second << endl;
	}

	// compute the letter low-high probability range
	double total = 0.0;
	for (auto pair: probabilities) {
		//cout << pair.first << " " << pair.second <<  endl;
		ranges[pair.first] = make_pair(total, total + pair.second);
		total += pair.second;
	}

	// This mode specifies round-to-nearest
  	mpfr_rnd_t rnd = MPFR_RNDN;
	mpfr_t low, high, delta;
	
	// 
	int l = ceil(line.length() * 8);
	mpfr_init2(low, l);
	mpfr_init2(high, l);
	mpfr_init2(delta, l);
	
	mpfr_set_str(low, "0", 10, rnd);
	mpfr_set_str(high, "1", 10, rnd);
	mpfr_set_str(delta, "0.0", 10, rnd);
	
	for (int i = 0; i < line.length(); i++) {
		mpfr_t temp, first, second;	
		mpfr_init2(first, l);
		mpfr_init2(second, l);
		mpfr_init2(temp, l);
		mpfr_set_str(temp, "0", 10, rnd);


		auto rangePair = ranges[line[i]];
		
		//delta = high - low;
		mpfr_sub(delta, high, low, rnd);
		
		mpfr_set_str(second, to_string(rangePair.second).c_str(), 10, rnd);

		//high = low + rangePair.second*delta;
		mpfr_mul(high, delta, second, MPFR_RNDU);
		mpfr_add(high, high, low, MPFR_RNDU);

		//low = low + rangePair.first*delta;
		mpfr_set_str(first, to_string(rangePair.first).c_str(), 10, rnd);

		mpfr_mul(temp, delta, first, MPFR_RNDU);
		mpfr_add(low, low, temp, MPFR_RNDU);
		//mpfr_out_str(stderr, 10, 0, high, rnd);
		//cerr << endl;
		//mpfr_out_str(stderr, 10, 0, low, rnd);
		//cerr << endl;
			mpfr_clear(temp);
		mpfr_clear(first);
		mpfr_clear(second);
	}

	mpfr_clear(delta);

	mpfr_free_cache();

	print_out(low, high, l);
	// char low_buffer[2048] = { 0 }; //new char[4096];
	// char high_buffer[2048] = { 0 }; // = new char[4096];
	// int pr = ceil(l*log10(2));
	// string fmt = "%." + to_string(l) + "Rf";
 
	// mpfr_sprintf(low_buffer, fmt.c_str(), low);
	// mpfr_sprintf(high_buffer, fmt.c_str(), high);

	// mpfr_clear(low);
	// mpfr_clear(high);
	// mpfr_free_cache();
	
	// int offset = 0;
	// for (int i = 2; i < 2048; i++) {
	// 	if (low_buffer[i] != high_buffer[i]) {
	// 		offset = i;
	// 		break;
	// 	}
	// }
	// //cerr << offset << endl;
	// //cout << offset << endl;
	// //low_buffer[offset + 1] = 0x00;
	// //high_buffer[offset + 1] = 0x00;
	// mpfr_t final_low, final_high;
	// mpfr_init2(final_low, l);
	// mpfr_init2(final_high, l);

	// mpfr_set_str(final_low, string(low_buffer).substr(0, offset + 2).c_str(), 10, MPFR_RNDU);

	// mpfr_set_str(final_high, string(high_buffer).substr(0, offset + 2).c_str(), 10, MPFR_RNDU);
	
	// if (low_buffer[offset + 4] != '0') {
	// 	mpfr_t r;
	// 	mpfr_init2(r, l);
	// 	string f;
	// 	f = string("1.0e-") + to_string(offset);
	// 	mpfr_set_str(r, f.c_str(), 10, MPFR_RNDU);
	// 	mpfr_add(final_low, final_low, r, MPFR_RNDU);
	// 	mpfr_clear(r);
	// }
	// mpfr_sprintf(low_buffer, fmt.c_str(), final_low);
	// mpfr_sprintf(high_buffer, fmt.c_str(), final_high);
	
	// mpfr_clear(final_low);
	// mpfr_clear(final_high);
	// mpfr_free_cache();
	// //mpfr_out_str(stdout, 10, 0, final_low, rnd);
	// //cout << endl;
	// //mpfr_out_str(stdout, 10, 0, final_high, rnd);
	// //cout << endl;
	// printf("%s %s\n", low_buffer, high_buffer);
	//cout << string(low_buffer).substr(0, offset + 3) << " ";
	//cout << string(high_buffer).substr(0, offset + 3) << endl;
	
	// what if the final number is zero?
	return 0;
}
