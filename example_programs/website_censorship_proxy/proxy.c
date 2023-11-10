#include "../../qsock.h"
#include "proxy.h"
#include "stdlib.h"

void add_node(struct LL *list, const char *keyword) {
	if (list->head == 0) {
		list->head = (struct LL_Node*)malloc(sizeof(struct LL_Node));
		memset(list->head , 0, sizeof(struct LL_Node));
		memcpy(list->head ->keyword, keyword, get_length(keyword));
		return;
	}

	struct LL_Node *ptr = list->head;
	while(ptr->next != 0) {
		ptr = ptr->next;
	}

	ptr->next = (struct LL_Node*)malloc(sizeof(struct LL_Node));
	memset(ptr->next, 0, sizeof(struct LL_Node));
	memcpy(ptr->next->keyword, keyword, get_length(keyword));
}

void print_list(struct LL *list) {
	struct LL_Node *ptr = list->head;
	while(ptr != 0) {
		printf("%s\n", ptr->keyword);

		ptr = ptr->next;
	}
}

int process_http(char *h) {
	char *ptr = h;

	char left[200];
	char right[200];

	int content_length = 0;

	while(*ptr != '\n') { ptr++; }
	ptr++;

	while (!equal(left, "Content-Type")) {
		memset(left, 0, 200);
		memset(right, 0, 200);

		int count = 0;
		while(*ptr != ':') {
			left[count++] = *ptr;
			ptr++;
		}
		ptr += 2; // skip : and ' '
		count = 0;
		while(*ptr != '\n' && *ptr != 13) {
			right[count++] = *ptr;
			ptr++;
		}
		ptr += 2;

		printf("left: %s, right: %s\n", left, right);

		if (equal(left, "Content-Length")) {
			content_length = atoi(right);
		}
	}
	printf("end of header (%ld)\n", ptr - h);
	return content_length;
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		fprintf(stderr, "Incorrect usage, should be: ./wcp_proxy <PORT>");
		return 1;
	}

	struct LL keywords = {};
	add_node(&keywords, "Floppy");
	add_node(&keywords, "Spongebob");
	print_list(&keywords);

	struct Proxy proxy;
	proxy.server = qsock_server(argv[1], TCP);
	proxy.buffer = (char *)malloc(TCP_BUFFER_SIZE);

	while(1) {
		qsock_accept(&proxy.server);

		proxy.bytes = qsock_general_recv(&proxy.server, proxy.buffer, TCP_BUFFER_SIZE);
		printf("(recv)%s(end)\n", proxy.buffer);

		int address_index = 0;
		char address[100];
		memset((void*)address, 0, 100);

		{
		int buffer_index = 0;
		while(buffer_index < proxy.bytes) {
			if (proxy.buffer[buffer_index]     == 'H' &&
				proxy.buffer[buffer_index + 1] == 'o' &&
				proxy.buffer[buffer_index + 2] == 's' &&
				proxy.buffer[buffer_index + 3] == 't' &&
				proxy.buffer[buffer_index + 4] == ':') {
				buffer_index += 6;
				//printf("HERE %s\n", proxy.buffer + buffer_index);
				while(proxy.buffer[buffer_index] != '\n' && proxy.buffer[buffer_index] != 13) {
					//printf("char %d\n", proxy.buffer[buffer_index]);
					address[address_index++] = proxy.buffer[buffer_index];
					buffer_index++;
				}
			}
			buffer_index++;
		}
		}

		const char *ip = qsock_get_ip_from_web(address);
		proxy.client = qsock_client(ip, "80", TCP);
		free((void*)ip);
		qsock_general_send(proxy.client, proxy.buffer, proxy.bytes);
		proxy.bytes = qsock_general_recv(&proxy.client, proxy.buffer, TCP_BUFFER_SIZE);

		int content_length = process_http(proxy.buffer);
		//fprintf(stderr, "(website bytes: %d, content_length: %d)\n", proxy.bytes, content_length);

		int bytes_sent = qsock_general_send(proxy.server, proxy.buffer, proxy.bytes);
		//printf("bytes_sent: %d\n", bytes_sent);

		proxy.bytes = qsock_general_recv(&proxy.client, proxy.buffer, TCP_BUFFER_SIZE);
		bytes_sent = qsock_general_send(proxy.server, proxy.buffer, proxy.bytes);
	}

	free((void*)proxy.buffer);

	return 0;
}