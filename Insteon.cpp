#include "Insteon.h"
#include <string>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <time.h>
using namespace std;

Insteon::Insteon(string in_IP, int in_port, string in_user, string in_pass)
{
	srand(time(NULL));

	_IP = in_IP;
	_port = in_port;
	_username = in_user;
	_password = in_pass;
	_random = rand() % 100 + 1;
	_level_is_raw = false;
}

string Insteon::getSceneURL()
{
	stringstream _return;
	_return << "http://" << _username << ":" << _password << "@" << _IP << ":" << _port << "/0?" << _command << _device << "=I=0" << _random;
	return _return.str();
}

string Insteon::getDeviceURL()
{
	stringstream _return;
	_return << "http://" << _username << ":" << _password << "@" << _IP << ":" << _port;
	return _return.str();
}

string Insteon::getDimmerURL()
{
	if (_deviceType == DIMMER && _level == 0)
		_level = 100;

	stringstream _return;
	_return << "http://" << _username << ":" << _password << "@" << _IP << ":" << _port << "/3?0262" << _device << "0F" << _command << getLevel() << "=I=3" << _random;
	return _return.str();
}

string Insteon::getRelayURL()
{
	if (_command == 11)
	{
		_command = 12;
		_level = 100;
	}

	if (_command == 13)
	{
		_command = 14;
		_level = 0;
	}

	stringstream _return;
	_return << "http://" << _username << ":" << _password << "@" << _IP << ":" << _port << "/3?0262" << _device << "0F" << _command << getLevel() << "=I=3" << _random;
	return _return.str();
}

string Insteon::getThermostatURL()
{
	string _code1 = "6B";
	string _code2;

	stringstream _ss_code1;
	stringstream _ss_code2;
	stringstream _ss_checksum;
	unsigned int _d_code1;
	unsigned int _d_code2;
	unsigned int _d_checksum;

	switch (_command)
	{
		case 4:
			_code2 = "04";
			break;
		case 5:
			_code2 = "05";
			break;
		case 6:
			_code2 = "06";
			break;
		case 7:
			_code2 = "07";
			break;
		case 8:
			_code2 = "08";
			break;
		case 9:
			_code2 = "09";
			break;
		case 10:
			_code1 = "6C";
			_code2 = getTemp();
			break;
		case 11:
			_code1 = "6D";
			_code2 = getTemp();
			break;
	}

	_ss_code1 << std::hex << _code1;
	_ss_code1 >> _d_code1;

	_ss_code2 << std::hex << _code2;
	_ss_code2 >> _d_code2;

	_d_checksum = ((_d_code1 + _d_code2) * 255) % 256;

	_ss_checksum << std::setw(2) << uppercase << hex << std::setfill('0') << ((unsigned int)_d_checksum);

	stringstream _return;
	_return << "http://" << _username << ":" << _password << "@" << _IP << ":" << _port << "/3?0262" << _device << "1F" << _code1 << _code2 << "00000000000000000000000000" << _ss_checksum.str() << "=I=3" << _random;
	return _return.str();
}

string Insteon::getBufferStatusURL()
{
	stringstream _return;
	_return << "http://" << _username << ":" << _password << "@" << _IP << ":" << _port << "/buffstatus.xml";
	return _return.str();
}

string Insteon::getStatusURL()
{
	stringstream _return;
	_return << "http://" << _username << ":" << _password << "@" << _IP << ":" << _port << "/sx.xml?" << _device << "=1900";
	return _return.str();
}

string Insteon::getURL()
{
	if (_command == STATUS)
		return getStatusURL();

	switch (_deviceType)
	{
		case RELAY:
			return getRelayURL();
		case DIMMER:
			return getDimmerURL();
		case SCENE:
			return getSceneURL();
		case THERMOSTAT:
			return getThermostatURL();
		default:
			return getDeviceURL();
	}

}

void Insteon::setType(int in_type)
{
	_deviceType = in_type;
}

void Insteon::setLevel(int in_level, bool in_raw_level = false)
{
	_level = in_level;
	_level_is_raw = in_raw_level;
}

void Insteon::setCommand(int in_command)
{
	_command = in_command;
	switch (_command)
	{
		case 12:
			_level = 100;
			break;
		case 13:
		case 14:
			_level = 0;
			break;
		default:
			_level = -1;
	}
}

string Insteon::getLevel()
{
	stringstream _s_hex;
	unsigned char hex_level;

	if (_level < 0)
		return string("ZZ");

	if (_level_is_raw)
	{
		hex_level = _level;
	} else
	{
		hex_level = (_level * 255) / 100;
	}
	_s_hex << std::setw(2) << uppercase << hex << std::setfill('0') << (int)hex_level;
	return _s_hex.str();
}

string Insteon::getTemp()
{
	if (_level < 0)
		return string("ZZ");

	stringstream _s_hex;
	_s_hex << std::setw(2) << uppercase << hex << std::setfill('0') << ((int)_level*2);
	return _s_hex.str();
}

bool Insteon::setDevice(string in_device)
{
	_device = in_device;	
	return true;
}

int Insteon::type() const
{
	return _deviceType;
} 

string Insteon::typeName()
{
	switch (_deviceType)
	{
		case 90:
			return "Relay";
		case 91:
			return "Dimmer";
		case 92:
			return "Scene";
		case 93:
			return "Thermostat";
		default:
			return "Unknown Type";
	}
}
