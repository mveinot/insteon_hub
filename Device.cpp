#include "Device.h"
#include <string>
using namespace std;

Device::Device(string in_address, string in_name, int in_type)
{
	_address = in_address;
	_name = in_name;
	_type = in_type;
}

string Device::getAddress()
{
	return _address;
}

string Device::getName()
{
	return _name;
}

int Device::getType()
{
	return _type;
}
