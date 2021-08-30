all: geoip

geoip:
	g++ -L/usr/local/lib/ -I/opt/halon/include/ -I/usr/include/x86_64-linux-gnu/ -fPIC -shared geoip.cpp -lmaxminddb -o geoip.so