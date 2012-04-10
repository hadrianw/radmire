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

static void evconnect(ENetEvent *ev)
{
	if(!ev)
		return;

	printf("rrclient: connected: %x:%u\n", 
	       ev->peer->address.host,
	       ev->peer->address.port);
	ev->peer->data = "Client information";
}

static void evreceive(ENetEvent *ev)
{
	if(!ev)
		return;

	printf("rrclient: packet[%zu] %x:%u/%u: %s\n",
	       ev->packet->dataLength,
	       ev->peer->address.host,
	       ev->peer->address.port,
	       ev->channelID,
	       ev->packet->data);
	enet_packet_destroy(ev->packet);
}

static void evdisconnect(ENetEvent *ev)
{
	if(!ev)
		return;

	printf("rrclient: disconnected: %x:%u\n", 
	       ev->peer->address.host,
	       ev->peer->address.port);
	ev->peer->data = NULL;
}

static void (*handler[ENET_EVENT_TYPE_COUNT])(ENetEvent *) = {
	[ENET_EVENT_TYPE_CONNECT] = evconnect,
	[ENET_EVENT_TYPE_RECEIVE] = evreceive,
	[ENET_EVENT_TYPE_DISCONNECT] = evdisconnect
};

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

	ENetEvent ev;

	ENetAddress address;
	ENetPeer *peer;
	enet_address_set_host(&address, servername);
	address.port = 44547;

	peer = enet_host_connect(client, &address, 2, 0);    
	if(!peer) {
		fprintf(stderr, "rrclient: couldn't connect to %s\n", servername);
		exit(EXIT_FAILURE);
	}

	if(enet_host_service(client, &ev, 3000) > 0
	   && ev.type == ENET_EVENT_TYPE_CONNECT) {
		evconnect(&ev);
	} else {
		enet_peer_reset(peer);
		fprintf(stderr, "rrclient: couldn't connect to %s\n", servername);
		exit(EXIT_FAILURE);
	}

	for(;;) {
		while(enet_host_service(client, &ev, 1000) >= 0) {
			if(handler[ev.type])
				handler[ev.type](&ev);
		}
	}

	return 0;
}
