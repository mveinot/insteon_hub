#include "Device.h"
#include <string>
using namespace std;

Device::Device(string in_address, string in_name, int in_type)
{
	_address = in_address;
	_name = in_name;
	_type = in_type;
}

string Device::getAddress() const
{
	return _address;
}

string Device::getName() const
{
	return _name;
}

int Device::getType() const
{
	return _type;
}
