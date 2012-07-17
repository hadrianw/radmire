#include <enet/enet.h>
#include <signal.h>
#include <stdio.h>

static ENetHost *server = NULL;

static void cleanup()
{
	puts("rrserver: cleanup");
	enet_host_destroy(server);
	enet_deinitialize();
}

static void sigint_handler(int sig)
{
	cleanup();
	signal(SIGINT, SIG_DFL);
	kill(getpid(), SIGINT);
}

static void evconnect(ENetEvent *ev)
{
	if(!ev)
		return;

	printf("rrserver: connected: %x:%u\n", 
	       ev->peer->address.host,
	       ev->peer->address.port);
	ev->peer->data = "Client information";
}

static void evreceive(ENetEvent *ev)
{
	if(!ev)
		return;

	printf("rrserver: packet[%zu] %x:%u/%u: %s\n",
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

	printf("rrserver: disconnected: %x:%u\n", 
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

	ENetEvent ev;
	for(;;) {
		while(enet_host_service(server, &ev, 1000) >= 0)
			if(handler[ev.type])
				handler[ev.type](&ev);
	}

	return 0;
}
