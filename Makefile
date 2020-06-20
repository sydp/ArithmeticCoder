all:
	g++ aencode.cpp -o aencode -lmpfr -lgmp
	g++ adecode.cpp -o adecode -lmpfr -lgmp
	
clean:
	rm -f aencode
	rm -f adecode
