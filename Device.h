#ifndef INSTEON_DEVICE_H
#define INSTEON_DEVICE_H

#include <string>

using namespace std;

class Device {

	private:
		string _address;
		string _name;
		int _type;

	public:
		Device(string, string, int);
		
		string getName() const;
		string getAddress() const;
		int getType() const;
};

#endif
