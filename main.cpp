#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <ctime>

#include "CIoTHubDevice.h"
#include "CIoTHubMessage.h"

#include "parson/parson.h"

using namespace std;

    // typedef IOTHUBMESSAGE_DISPOSITION_RESULT (*MessageCallback)(CIoTHubDevice &iotHubDevice, CIoTHubMessage &iotHubMessage, void *userContext);
    // typedef void (*EventConfirmationCallback)(CIoTHubDevice &iotHubDevice, IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContext);
    // typedef void (*ConnectionStatusCallback)(CIoTHubDevice &iotHubDevice, IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *userContext);

void connectionStatusCallback(CIoTHubDevice &iotHubDevice, IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *userContext)
{
    cout << "Connection status result="  << result << ";reason=" << reason << endl;
}

void eventConfirmationCallback(CIoTHubDevice &iotHubDevice, IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContext)
{
    cout << "Message confirmed with " << result << " and " << *((int *)userContext) << endl;
}

IOTHUBMESSAGE_DISPOSITION_RESULT MessageCallback(CIoTHubDevice &iotHubDevice, CIoTHubMessage &iotHubMessage, void *userContext)
{
    IOTHUBMESSAGE_CONTENT_TYPE type = iotHubMessage.GetContentType();

    if (type == IOTHUBMESSAGE_STRING)
    {
        cout << "C2D String Message: " << iotHubMessage.GetString() << endl;
    }
    else if (type == IOTHUBMESSAGE_BYTEARRAY)
    {
        const uint8_t *buffer;
        size_t length;

        iotHubMessage.GetByteArray(&buffer, &length);
        char *str = new char[length + 1];
        memcpy(str, buffer, length);
        str[length] = '\0';

        cout << "C2D ByteArray Message: " << str << endl;

        delete [] str;
    }
    else
    {
        cout << "C2D Unknown Message" << endl;
    }

    return IOTHUBMESSAGE_ACCEPTED;
}

int DeviceMethodCallback(CIoTHubDevice &iotHubDevice, const unsigned char *payload, size_t size, unsigned char** response, size_t* resp_size, void* userContext)
{
    const char *fixedResp = "{ \"TestResponse\": \"Success\"}";
    char *str = new char[size + 1];
    memcpy(str, payload, size);
    str[size] = '\0';

    cout << "Test method called" << str << endl;

    *response =  new unsigned char[strlen(fixedResp) + 1];
    strcpy((char *)*response, fixedResp);
    *resp_size = strlen((char *)*response) + 1;

    return 200;
}

int UnknownDeviceMethodCallback(CIoTHubDevice &iotHubDevice, const char *methodName, const unsigned char *payload, size_t size, unsigned char** response, size_t* resp_size, void* userContext)
{
    cout << "Unknown method " << methodName << " called" << endl;
    *resp_size = 0;
    *response = NULL;

    return 404;
}

int main(int argc, char** argv)
{
	if (argc < 2 || argc > 3)
	{
		cout << "You must provide a connection string and optionally [MQTT | MQTTWS]" << endl;
		return 4;
	}
	
	char *connectionString = *(argv + 1);
	
	const char *protocolArg;
	
	if (argc == 3)
	{
		protocolArg = *(argv + 2);
		
		if (strcmp(protocolArg, "MQTT") != 0 && strcmp(protocolArg, "MQTTWS") != 0)
		{
			cout << "Protocol must be MQTT or MQTTWS" << endl;
			return 4;
		}
	}
	else
	{
		protocolArg  = "MQTT";
	}
	
    try
    {
        cout << "ConnectionString=" << connectionString << endl;
		cout << "Protocol=" << protocolArg << endl;

        CIoTHubDevice::Protocol protocol;
        
        if (strcmp(protocolArg, "MQTT") == 0)
        {
            protocol = CIoTHubDevice::Protocol::MQTT;
        }
        else if (strcmp(protocolArg, "MQTTWS") == 0)
        {
            protocol = CIoTHubDevice::Protocol::MQTT_WebSockets;
        }
        else
        {
            cerr << "Invalid protocol" << endl;
            return 4;
        }

        CIoTHubDevice *device = new CIoTHubDevice(connectionString, protocol);

        device->SetConnectionStatusCallback(connectionStatusCallback, (void *)255);
        device->SetMessageCallback(MessageCallback, NULL);
        device->SetDeviceMethodCallback("Test", DeviceMethodCallback);
        device->SetUnknownDeviceMethodCallback(UnknownDeviceMethodCallback);
		
		time_t lastSend = 0;
		time_t now = 0;
		int messageCount = 10;
		int thisMessage = 0;
		
		cout << "Sending " << messageCount << " messages" << endl;

        while (messageCount > 0)
        {
			now = time(NULL);
			
			if (now - lastSend >= 2)
			{
				//cout << "lastSend=" << lastSend << ";now=" << now << endl;
				lastSend = now;
				messageCount--;
				cout << "Sending message " << ++thisMessage << endl;
				device->SendEventAsync("{ \"test\" : 10 }", eventConfirmationCallback, &thisMessage);
			}
			
            device->DoWork();
			this_thread::sleep_for(chrono::milliseconds(50));
        }

        while (device->WaitingEvents())
        {
            device->DoWork();
        }

        delete device;
    }
    catch (exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
    }
}
