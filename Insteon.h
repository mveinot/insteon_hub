#ifndef INSTEON_H
#define INSTEON_H

#define STATUS 1

#define RELAY 90
#define DIMMER 91
#define SCENE 92
#define THERMOSTAT 93

#define ON_HEAT 4
#define ON_COOL 5
#define ON_AUTO 6
#define ON_FAN 7
#define OFF_FAN 8
#define OFF_ALL 9
#define COOL_SET 10
#define HEAT_SET 11

#define ON 11
#define FAST_ON 12
#define OFF 13
#define FAST_OFF 14
#define BRIGHT 15
#define DIM 16

#include <string>

using namespace std;

class Insteon {

	private:
		string _IP;
		int _tcp_port;
		string _device;
		int _deviceType;
		int _command;
		int _level;
		int _random;
		bool _level_is_raw;
		unsigned char _return_value;

		string getRelayCommand();
		string getDimmerCommand();
		string getSceneCommand();
		string getThermostatCommand();
		string getStatusCommand();
		string getCommandString();
		void int2hex(const int, char*);
		void hex2bin(const char*, unsigned char*);

	public:
		Insteon(string);
		void setType(int);
		bool setDevice(string);
		void setLevel(int, bool);
		void setCommand(int);
		bool sendCommand();
		int getLastStatus();
		
		int type() const;
		string typeName();
		string getLevel();
		string getTemp();
};

#endif
