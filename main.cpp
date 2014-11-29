#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <getopt.h>
#include <vector>
#include "tinyxml2.h"
#include "Insteon.h"
#include "Device.h"
#include "version.h"

using namespace std;

#define no_argument 0
#define required_argument 1
#define optional_argument 2

void showHelp(char *in_progname);
void showVersion();

int main(int argc, char** argv)
{
	int _return = 1;
	int index;
	int iarg = 0;
	int device_type = 0;
	int command = 0;
	int pct_level = 0;

	bool verbose = false;
	bool celsius = false;
	bool raw_level = false;
	bool no_action = false;

	string IP;
	int port = 0;
	string username;
	string password;

	string device;
	string pagedata;

	tinyxml2::XMLDocument doc;

	stringstream config;

	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	config << string(homedir) << "/.insteon_hub";
	const char *configfile = config.str().c_str();
	vector<Device *> DeviceList;

	doc.LoadFile(configfile);
	if (doc.ErrorID())
	{
		cout << "Config file not found or XML format error." << endl;
	} else
	{
		const char *_c_ip = doc.FirstChildElement("hub")->FirstChildElement("ip")->GetText();
		const char *_c_port = doc.FirstChildElement("hub")->FirstChildElement("port")->GetText();
		const char *_c_username = doc.FirstChildElement("hub")->FirstChildElement("username")->GetText();
		const char *_c_password = doc.FirstChildElement("hub")->FirstChildElement("password")->GetText();

		IP = string(_c_ip);
		port = atoi(_c_port);
		username = string(_c_username);
		password = string(_c_password);

		tinyxml2::XMLElement *devices = doc.FirstChildElement("hub")->FirstChildElement("devices");
		tinyxml2::XMLElement *child = devices->FirstChildElement("device");
		while (child != NULL)
		{
			DeviceList.push_back(new Device(child->Attribute("address"), child->Attribute("name"), atoi(child->Attribute("type"))));
			child = child->NextSiblingElement();
		}
	}

	const struct option longopts[] = 
	{
		{"version",	no_argument,	NULL, 'v'},
		{"help",	no_argument,	NULL, 2},
		{"verbose",	no_argument,	NULL, 'V'},
		{"relay",	required_argument,	NULL, 'r'},
		{"dimmer",	required_argument,	NULL, 'd'},
		{"thermostat",	required_argument,	NULL, 't'},
		{"scene",	required_argument,	NULL, 's'},
		{"address",	required_argument,	NULL, 'a'},
		{"port",	required_argument,	NULL, 'p'},
		{"username",	required_argument,	NULL, 'U'},
		{"password",	required_argument,	NULL, 'P'},
		{"level",	required_argument,	NULL, 'l'},
		{"on",		no_argument,		NULL, ON},
		{"off",		no_argument,		NULL, OFF},
		{"dim",		no_argument,		NULL, DIM},
		{"bright",	no_argument,		NULL, BRIGHT},
		{"status",	no_argument,		NULL, STATUS},
		{"heat",	no_argument,		NULL, ON_HEAT},
		{"cool",	no_argument,		NULL, ON_COOL},
		{"auto",	no_argument,		NULL, ON_AUTO},
		{"fan_on",	no_argument,		NULL, ON_FAN},
		{"fan_off",	no_argument,		NULL, OFF_FAN},
		{"all_off",	no_argument,		NULL, OFF_ALL},
		{"set_cooling",	required_argument,		NULL, 'c'},
		{"set_heating",	required_argument,		NULL, 'h'},
		{"fahrenheit",	no_argument,		NULL, 212},
		{"celsius",	no_argument,		NULL, 211},
		{"byte_value", no_argument,		NULL, 210},
		{"no_action", no_argument,		NULL, 209},
		{"name", required_argument,	NULL, 208},
		{"list", no_argument, NULL, 207},
		{0,0,0,0},
	};

	while(iarg != -1)
	{
		iarg = getopt_long(argc, argv, "vVr:d:t:s:a:p:U:P:l:c:h:", longopts, &index);

		switch(iarg)
		{
			case 2:
				showHelp(argv[0]);
				return 0;
			case 'v':
				showVersion();
				return 0;
			case 'V':
				cout << "Enabling verbose operation" << endl;
				verbose = true;
				break;
			case 'a':
				IP = optarg;

				if (verbose)
					cout << "Overriding IP: " << IP << endl;
				break;
			case 'p':
				port = atoi(optarg);

				if (verbose)
					cout << "Overriding Port: " << port << endl;
				break;
			case 'U':
				username = optarg;

				if (verbose)
					cout << "Overriding User: " << username << endl;
				break;
			case 'P':
				password = optarg;

				if (verbose)
					cout << "Overriding Password." << username << endl;
				break;
			case 'r':
				device_type = RELAY;
				device = optarg;

				if (verbose)
				{
					cout << "Relay selected" << endl;
					cout << "Device ID: " << optarg << endl;
				}
				break;
			case 'd':
				device_type = DIMMER;
				device = optarg;

				if (verbose)
				{
					cout << "Dimmer selected" << endl;
					cout << "Device ID: " << optarg << endl;
				}
				break;
			case 't':
				device_type = THERMOSTAT;
				device = optarg;

				if (verbose)
				{
					cout << "Thermostat selected" << endl;
					cout << "Device ID: " << optarg << endl;
				}
				break;
			case 's':
				device_type = SCENE;
				device = optarg;

				if (verbose)
				{
					cout << "Scene selected" << endl;
					cout << "Device ID: " << optarg << endl;
				}
				break;
			case ON:
				command = ON;

				if (verbose)
					cout << "Command: ON" << endl;
				break;
			case OFF:
				command = OFF;

				if (verbose)	
					cout << "Command: OFF" << endl;
				break;
			case BRIGHT: 
				command = BRIGHT;

				if (verbose)
					cout << "Command: BRIGHT" << endl;
				break;
			case DIM:
				command = DIM;

				if (verbose)
					cout << "Command: DIM" << endl;
				break;
			case STATUS:
				command = STATUS;

				if (verbose)
					cout << "Status requested" << endl;
				break;
			case ON_HEAT:
				command = ON_HEAT;

				if (verbose)
					cout << "HVAC Command: Heat" << endl;
				break;
			case ON_COOL:
				command = ON_COOL;

				if (verbose)
					cout << "HVAC Command: Cool" << endl;
				break;
			case ON_AUTO:
				command = ON_AUTO;

				if (verbose)
					cout << "HVAC Command: Auto" << endl;
				break;
			case ON_FAN:
				command = ON_FAN;

				if (verbose)
					cout << "HVAC Command: Fan on" << endl;
				break;
			case OFF_FAN:
				command = OFF_FAN;

				if (verbose)
					cout << "HVAC Command: Fan off" << endl;
				break;
			case OFF_ALL:
				command = OFF_ALL;

				if (verbose)
					cout << "HVAC Command: All off" << endl;
				break;
			case 'c':
				command = COOL_SET;
				pct_level = atoi(optarg);

				if (verbose)
					cout << "HVAC Command: Adjust cooling setpoint to " << pct_level << endl;
				break;
			case 'h':
				command = HEAT_SET;
				pct_level = atoi(optarg);

				if (verbose)
					cout << "HVAC Command: Adjust heating setpoint to " << pct_level << endl;
				break;
			case 212:
				break;
			case 211:
				celsius = true;
				break;
			case 210:
				raw_level = true;
				break;
			case 209:
				no_action = true;
				break;
			case 208:
			{
				string tmp_name = string(optarg);

				for (int i = 0; i < DeviceList.size(); i++)
				{
					string iter_name = DeviceList[i]->getName();

					if (iter_name == tmp_name)
					{
						if (verbose)
							cout << "Found device named \"" << tmp_name << "\" with address: " << DeviceList[i]->getAddress() << endl;

						device_type = DeviceList[i]->getType();
						device = DeviceList[i]->getAddress();
					}
				}
				if (device_type == 0 && verbose)
					cout << "No device found named \"" << tmp_name << "\"" << endl;
				break;
			}
			case 207:
				cout << "The following device names are defined in the config file:" << endl;

				for (int i = 0; i < DeviceList.size(); i++)
				{
					string iter_name = DeviceList[i]->getName();
					string iter_device = DeviceList[i]->getAddress();

					cout << "\"" << iter_name << "\" is an alias for device address " << iter_device << endl;
				}

				cout << endl;
				return 0;
				break;
			case 'l':
				pct_level = atoi(optarg);

				if (verbose)
					cout << "Level: " << pct_level << endl;
				break;
		}
	}


	if (device_type == 0)
	{
		cout << "No device specified. No action taken." << endl << endl;
		showHelp(argv[0]);
		return 0;
	}

	if (command == 0)
	{
		cout << "No command selected. No action taken." << endl << endl;
		showHelp(argv[0]);
		return 0;
	}

	if (device_type == SCENE && command == STATUS)
	{
		cout << "Unable to query scene status. Query individual devices instead." << endl;
		return 0;
	}

	if (device_type == THERMOSTAT && command == STATUS)
	{
		cout << "Querying thermostat status is still a work in progress" << endl;
		return 0;
	}

	if (IP == "" || port == 0 || username == "" || password == "")
	{
		cout << "Connection details not provided. Unable to proceed." << endl << endl;
		showHelp(argv[0]);
		return 1;
	}

	if (device_type == THERMOSTAT && celsius)
	{
		pct_level = ((pct_level * 9) / 5) + 32;
	}

	Insteon insteon(IP, port, username, password); 
	insteon.setType(device_type);
	insteon.setDevice(device);
	insteon.setCommand(command);
	insteon.setLevel(pct_level, raw_level);

	if (verbose)
	{
		cout << "Interpreted device type:     " << insteon.typeName() << " (" << insteon.type() << ") " << endl;
		if (device_type == DIMMER)
			cout << "Converted dim level:         " << insteon.getLevel() << endl;
		if (device_type == THERMOSTAT)
			cout << "Converted temperature:       " << insteon.getTemp() << endl;
	}

	insteon.sendCommand();
	if (command == STATUS)
	{
		int level = insteon.getLastStatus();
		int status_pct = (level * 100) / 255;
		if (device_type == RELAY)
		{
			switch (status_pct)
			{
				case 100:
					cout << "Device is on" << endl;
					break;
				case 0:
					cout << "Device is off" << endl;
					break;
				default:
					cout << "Unknown status" << endl;
			}
		} else if (device_type == DIMMER)
		{
			if (status_pct > 0)
				cout << "Device is on at " << status_pct << "%" << endl;
			if (status_pct == 0)
				cout << "Device is off" << endl;
		}
	}

	return 0;
}

void showHelp(char *in_progname)
{
	cout << "Usage: " << in_progname << " [OPTION]... " << endl;
	cout << "Send commands and get status of Insteon devices through an Insteon Hub" << endl;
	cout << "connected to the local network." << endl << endl;
	cout << "  -v, --version			show version information" << endl;
	cout << "      --help			show this help message" << endl;
	cout << "  -V, --verbose			enable extra output" << endl;
	cout << "  -r, --relay=ID		specify relay device with its address" << endl;
	cout << "  -d, --dimmer=ID		specify dimmer device with its address" << endl;
	cout << "  -t, --thermostat=ID		specify thermostat device with its address" << endl;
	cout << "  -s, --scene=ID		specify scene with its identifier" << endl;
	cout << "      --name=ALIAS		specify device using a defined alias" << endl;
	cout << "      --list			list all defined device aliases" << endl;
	cout << "  -a, --address=IP		override Hub IP address" << endl;
	cout << "  -p, --port=PORT		override Hub port number" << endl;
	cout << "  -U, --username=USER		override Hub username" << endl;
	cout << "  -P, --password=PASS		override Hub password" << endl;
	cout << "  -l, --level=LEVEL		set dim/bright percent" << endl;
	cout << "      --byte_value		interpret LEVEL as byte value (0-255)" << endl;
	cout << "      --heat			set HVAC system to heating mode" << endl;
 	cout << "      --cool			set HVAC system to cooling mode" << endl;
	cout << "      --auto			set HVAC system to auto mode" << endl;
	cout << "      --fan_on			set HVAC system fan on" << endl;
	cout << "      --fan_off			set HVAC system fan off" << endl;
	cout << "      --all_off			set all HVAC system components off" << endl;
	cout << "      --fahrenheit		interpret TEMP values as degrees Fahrenheit (default)" << endl;
	cout << "      --celsius			interpret TEMP values as degrees Celsius" << endl;
	cout << "  -c, --set_cooling=TEMP	set HVAC cool point to TEMP" << endl;
	cout << "  -h, --set_heating=TEMP	set HVAC heat point to TEMP" << endl;
	cout << "      --on			send On command to specified device" << endl;
	cout << "      --off			send Off command to specified device" << endl;
	cout << "      --dim			send Dim command to specified device" << endl;
	cout << "      --bright			send Bright command to specified device" << endl;
	cout << "      --no_action		Display the URL that is generated, but don't send it to the Hub" << endl;
	cout << "      --status			retrieve status of specified device" << endl << endl;
	cout << "Exit status:" << endl;
	cout << " Returns HTTP status code from hub if no other errors, or 1 otherwise." << endl;
}

void showVersion()
{
	cout << "Insteon Hub Command-line Control Utility v" << __HUB_VERSION__ << endl;
}
