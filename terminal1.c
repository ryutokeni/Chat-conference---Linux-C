#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

int main()
{
	system("gnome-terminal");
	sleep(1);
 	fstream chatScreen;
	chatScreen.open("/dev/pts/1", ios::out);
	chatScreen << "Thank you" << endl;
	chatScreen.close();
}
