#include "Insteon.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
using namespace std;

Insteon::Insteon(string in_IP)
{
	srand(time(NULL));

	_IP = in_IP;
	_tcp_port = 9761;
	_random = rand() % 100 + 1;
	_level_is_raw = false;
}

string Insteon::getSceneCommand()
{
	char hex_string[3];
	bzero(hex_string, 3);
	int2hex(atoi(_device.c_str()), hex_string);

	stringstream _return;
	_return << "0261" << hex_string << _command << "00";
	return _return.str();
}

string Insteon::getDimmerCommand()
{
	if (_deviceType == DIMMER && _level == 0)
		_level = 100;

	stringstream _return;
	_return << "0262" << _device << "0F" << _command << getLevel();
	return _return.str();
}

string Insteon::getRelayCommand()
{
	if (_command == 11)
	{
//		_command = 12;
		_level = 100;
	}

	if (_command == 13)
	{
//		_command = 14;
		_level = 0;
	}

	stringstream _return;
	_return << "0262" << _device << "0F" << _command << getLevel();
	return _return.str();
}

string Insteon::getThermostatCommand()
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
	_return << "0262" << _device << "1F" << _code1 << _code2 << "00000000000000000000000000" << _ss_checksum.str();
	return _return.str();

}

string Insteon::getStatusCommand()
{
	stringstream _return;
	_return << "0262" << _device << "0F1900";
	return _return.str();
}

string Insteon::getCommandString()
{
	if (_command == STATUS)
		return getStatusCommand();

	switch (_deviceType)
	{
		case RELAY:
			return getRelayCommand();
		case DIMMER:
			return getDimmerCommand();
		case THERMOSTAT:
			return getThermostatCommand();
		case SCENE:
			return getSceneCommand();
		default:
			return getStatusCommand();
	}
}

void Insteon::int2hex(const int val, char* target)
{
	sprintf(target, "%02X", val);
}

void Insteon::hex2bin(const char* src, unsigned char* target)
{
	size_t count = 0;

	while(*src && src[1])
	{
		sscanf(src, "%2hhx", &target[count]);
		count++;
		src+=2;
	}
}

bool Insteon::sendCommand()
{
	string commandStr = getCommandString();
	const char *command = commandStr.c_str();
	size_t length;
	
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	unsigned char buffer[256];
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		return false;

	server = gethostbyname(_IP.c_str());
	if (server == NULL)
		return false;

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(_tcp_port);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		return false;

	bzero(buffer,256);
	
	//printf("%s\n", command);
	hex2bin(command, buffer);
	switch (_deviceType)
	{
		case RELAY:
		case DIMMER:
			length = 8;
			break;
		case SCENE:
			length = 5;
			break;
		default:
			length = 4;
	}

	if (_command == STATUS)
		length = 8;

	//printf("%s\n", buffer);
	n = write(sockfd,buffer,length);
	if (n < 0)
		return false;

//	printf("Sent %d bytes\n", n);

	bzero(buffer, 256);
	n = read(sockfd,buffer,256);
	if (n < 0)
		return false;

/*
	printf("Result: (%d) %s\n", n, buffer);
	int i;
	for (i = 0; i < n; i++)
	{
		printf("%02X", buffer[i]);
	}
	printf("\n");
*/

	if (_command == STATUS)
	{
		sleep(2);

	do {
		bzero(buffer, 256);
		n = read(sockfd,buffer,256);
		if (n < 0)
			return false;
	} while (n != 11);

	_return_value = buffer[10];
/*
	printf("Result: (%d) %s\n", n, buffer);
	for (int i = 10; i < n; i++)
	{
		printf("%02X", buffer[10]);
	}
	printf("\n");
*/
	}

	close(sockfd);
	return true;
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

int Insteon::getLastStatus()
{
//	cout << ": " << static_cast<int>(_return_value);
	return (int)_return_value;
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
