// NREP.cpp : Defines the entry point for the application.
//

#include "NREP.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;


int main()
{
	// Leave this to make sure things are working right
	cout << "Hello Matt." << endl;


	int serial_port = open("/dev/ttyS0", O_RDWR);

	// Check for errors
	if (serial_port < 0) {
		printf("Error %i from open: %s\n", errno, strerror(errno));

	}

	// Setup serial port
	struct termios tty;

	if (tcgetattr(serial_port, &tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));

	}

	// set up serial port
	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity 
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication
	tty.c_cflag &= ~CSIZE; // Clear all the size bits
	tty.c_cflag |= CS8; // 8 bits per byte
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	cfsetspeed(&tty, B57600); // Set in/out baud rate to be 57600

	// Save tty settings, also checking for error
	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));

	}

	cout << "Serial Port Set Up!" << endl;

	char break1 = static_cast<char>(0x53); //ESC 0x1B
	char break2 = static_cast<char>(0x53); //S 0x53
	char break3 = static_cast<char>(0x0A); //LF 0x0A


	// Caluclate Current Time


	for (int i = 0; i < 1000; i++) //Send the Status message at 1Hz for 1000s
	{
		std::this_thread::sleep_for(seconds(1)); //pause before looping
		string sTime = currentDateTime(); //get the current time
		const int length = sTime.length(); //determine legth of currentDateTime() return
		char* cTime = new char[length + 1]; //create char string
		strcpy(cTime, sTime.c_str()); //copy from the str to char
		char* msg = new char[length + 4]; //Message that will be sent on Serial
		msg[0] = break1;
		msg[1] = break2;
		for (int i = 0; i < length;i++)
		{
			msg[i + 2] = cTime[i];
		}
		msg[length + 3] = break3; //add the esc / LF to the message

		write(serial_port, msg, sizeof(msg)); //write the message to the serial port
		printf("Write to Serial: %s \n", msg); //error message display to terminal
	}
	// Close the serial port
	close(serial_port);



	return 0;
}


