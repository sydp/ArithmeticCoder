#include <iostream>
#include <map>
#include <limits>
#include <iomanip>
#include <gmp.h>
#include <mpfr.h>
#include <cmath>

using namespace std;

mpfr_t low, high;

// Output the arithmetic coded low and high values, rounding them to the 
// minimum precision needed to decode them 
void print_out(int l) {

	int buf_size = 4096;
	char low_buffer[4096] = { 0 }; 
	char high_buffer[4096] = { 0 };

	string fmt = "%." + to_string(buf_size) + "Rf";
 	mpfr_sprintf(low_buffer, fmt.c_str(), low);
	mpfr_sprintf(high_buffer, fmt.c_str(), high);

	mpfr_clear(low);
	mpfr_clear(high);

	// find the offset (digit) where the high and low differs
	int offset = 0;
	for (int i = 2; i < buf_size; i++) {
		if (low_buffer[i] != high_buffer[i]) {
			offset = i;
			break;
		}
	}
	
	// convert back to an mpfr_t
	mpfr_t final_low, final_high;
	mpfr_init2(final_low, l);
	mpfr_init2(final_high, l);

	mpfr_set_str(final_low, string(low_buffer).substr(0, offset + 2).c_str(), 10, MPFR_RNDU);
	mpfr_set_str(final_high, string(high_buffer).substr(0, offset + 2).c_str(), 10, MPFR_RNDU);
	
	// "round-up" the low value if needed
	if (low_buffer[offset + 4] != '0') {
		mpfr_t r;
		mpfr_init2(r, l);
		string f;
		f = string("1.0e-") + to_string(offset);
		mpfr_set_str(r, f.c_str(), 10, MPFR_RNDU);
		mpfr_add(final_low, final_low, r, MPFR_RNDU);
		mpfr_clear(r);
	}

	// "round-up" the high value if needed
	if (high_buffer[offset + 4] != '0') {
		mpfr_t r;
		mpfr_init2(r, l);
		string f;
		f = string("1.0e-") + to_string(offset);
		mpfr_set_str(r, f.c_str(), 10, MPFR_RNDU);
		mpfr_add(final_high, final_high, r, MPFR_RNDU);
		mpfr_clear(r);
	}

	// convert back to ascii decimal again..
	mpfr_sprintf(low_buffer, fmt.c_str(), final_low);
	mpfr_sprintf(high_buffer, fmt.c_str(), final_high);
	
	mpfr_clear(final_low);
	mpfr_clear(final_high);
	mpfr_free_cache();

	// and output to stdout, separated by a space
	cout << string(low_buffer).substr(0, offset + 2) << " ";
	cout << string(high_buffer).substr(0, offset + 2) << endl;

}

// encode the line of "visible ASCII" characters into an arithmetic coded
// high and low value.  Return the precision (# bits) used to encode
int encode(const string &line) {
	map<char, int> counter;
	map<char, double> probabilities;
	map<char, pair<double, double>> ranges;
	
	// Get letter frequencies
	for (int i = 0; i < line.length(); i++) {
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
		ranges[pair.first] = make_pair(total, total + pair.second);
		total += pair.second;
	}

	// This mode specifies round-to-nearest
  	mpfr_rnd_t rnd = MPFR_RNDN;
	mpfr_t delta;
	
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
		
		//high = low +fmt rangePair.second*delta;
		mpfr_set_str(second, to_string(rangePair.second).c_str(), 10, rnd);
		mpfr_mul(high, delta, second, MPFR_RNDU);
		mpfr_add(high, high, low, MPFR_RNDU);

		//low = low + rangePair.first*delta;
		mpfr_set_str(first, to_string(rangePair.first).c_str(), 10, rnd);
		mpfr_mul(temp, delta, first, MPFR_RNDU);
		mpfr_add(low, low, temp, MPFR_RNDU);
	
		mpfr_clear(temp);
		mpfr_clear(first);
		mpfr_clear(second);
	}

	mpfr_clear(delta);

	mpfr_free_cache();

	return l;
}

int main() {

	string line;
	getline(cin, line);
	
	int precision = encode(line);
	print_out(precision);

	return 0;
}
