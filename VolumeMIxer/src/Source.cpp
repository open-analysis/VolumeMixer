#include <iostream>
#include "web/webClient.h"

void testWeb()
{

	webClient client;

	client.getQueue();
}

int main(int argc, CHAR* argv[])
{
	testWeb();

	printf("\nDone\n");

	return 0;
}