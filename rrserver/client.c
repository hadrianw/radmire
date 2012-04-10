#include <enet/enet.h>
#include <signal.h>
#include <stdio.h>

ENetHost *client = NULL;

void cleanup()
{
	puts("rrclient: cleanup");
	enet_host_destroy(client);
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
	const char *servername = "localhost";
	if(argc > 1)
		servername = argv[1];

	if(enet_initialize()) {
		fputs("rrclient: couldn't init ENet\n", stderr);
		return EXIT_FAILURE;
	}
	atexit(cleanup);
	signal(SIGINT, sigint_handler);

	client = enet_host_create(NULL, 1, 2, 57600 / 8, 14400 / 8);
	if(!client) {
		fputs("rrclient: couldn't create client host\n", stderr);
		exit(EXIT_FAILURE);
	}

	ENetEvent event;

	ENetAddress address;
	ENetPeer *peer;
	enet_address_set_host(&address, servername);
	address.port = 44547;

	peer = enet_host_connect(client, &address, 2, 0);    
	if(!peer) {
		fprintf(stderr, "rrclient: couldn't connect to %s\n", servername);
		exit(EXIT_FAILURE);
	}

	if(enet_host_service(client, &event, 5000) > 0
	   && event.type == ENET_EVENT_TYPE_CONNECT) {
		printf("rrclient: connected to %s", servername);
	} else {
		enet_peer_reset(peer);
		fprintf(stderr, "rrclient: couldn't connect to %s", servername);
		exit(EXIT_FAILURE);
	}

	for(;;) {
		while(enet_host_service(client, &event, 1000) >= 0) {
			switch(event.type) {
			case ENET_EVENT_TYPE_RECEIVE:
				printf("rrclient: packet[%zu] %x:%u/%u: %s\n",
				       event.packet->dataLength,
				       event.peer->address.host,
				       event.peer->address.port,
				       event.channelID,
				       event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("rrclient: disconnected: %x:%u\n", 
				       event.peer->address.host,
				       event.peer->address.port);
				event.peer->data = NULL;
				exit(0);
			case ENET_EVENT_TYPE_NONE:
			default:
				break;
			}
		}
	}

	return 0;
}
