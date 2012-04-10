#include <enet/enet.h>
#include <signal.h>
#include <stdio.h>

ENetHost *server = NULL;

void cleanup()
{
	puts("rrserver: cleanup");
	enet_host_destroy(server);
	enet_deinitialize();
}

void sigint_handler(int sig)
{
	cleanup();
	signal(SIGINT, SIG_DFL);
	kill(getpid(), SIGINT);
}

int main(int argc, char **argv)
{
	if(enet_initialize()) {
		fputs("rrserver: couldn't init ENet\n", stderr);
		return EXIT_FAILURE;
	}
	atexit(cleanup);
	signal(SIGINT, sigint_handler);

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 44547;
	server = enet_host_create(&address, 32, 2, 0, 0);
	if(!server) {
		fputs("rrserver: couldn't create server host\n", stderr);
		exit(EXIT_FAILURE);
	}

	ENetEvent event;
	for(;;) {
		while(enet_host_service(server, &event, 1000) >= 0) {
			switch(event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				printf("rrserver: connected: %x:%u\n", 
				       event.peer->address.host,
				       event.peer->address.port);
				event.peer->data = "Client information";
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				printf("rrserver: packet[%zu] %x:%u/%u: %s\n",
				       event.packet->dataLength,
				       event.peer->address.host,
				       event.peer->address.port,
				       event.channelID,
				       event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("rrserver: disconnected: %x:%u\n", 
				       event.peer->address.host,
				       event.peer->address.port);
				event.peer->data = NULL;
				break;
			case ENET_EVENT_TYPE_NONE:
				break;
			}
		}
	}

	return 0;
}
