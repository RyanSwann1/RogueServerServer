#include "Server.h"

//https://support.holmsecurity.com/hc/en-us/articles/212963869-What-is-the-difference-between-TCP-and-UDP-
//https://stackoverflow.com/questions/3329641/how-do-multiple-clients-connect-simultaneously-to-one-port-say-80-on-a-server
//http://it-elibrary.blogspot.com/2016/01/sfml-game-development-by-example.html
//https://en.sfml-dev.org/forums/index.php?topic=943.0

//Listen all the time on one thread
//input buffer
//receives data
//stores for a bit
//spits out when neccessary

//Have list mutex locked
//mutex lock
//mutex unlock

//Mutex when adding somethihng to buffer-Contaienr - Queue
//Only one access can access the queue at one time

//std promise
//std Future 

int main()
{
	Server server("152.105.219.108", 5030);
	server.beginListenThread();
	while (server.isRunning())
	{
		server.update();
	}

	return 0;
}