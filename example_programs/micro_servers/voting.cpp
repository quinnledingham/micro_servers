#include "basic.h"

#include "buffer.h"

struct Candidate
{
	const char *name;
	int id;
	int votes;
};
global Candidate candidates[4] = {
	{ "Louella Conway", 5238, 10 },
	{ "Jordan Larson",  6483, 12 },
	{ "Jean Banks",     3233, 15 },
	{ "Coleen Green",   6327, 19 },
};

struct IP_List_Node
{
	bool32 head;
	const char *ip;
	IP_List_Node *next;
};

internal bool32
already_voted(IP_List_Node *head, const char *ip)
{
	printf("already_voted() ip: %s\n", ip);
	IP_List_Node *ptr = head;
	while (ptr != 0) {
		printf("ptr %p\n", ptr);
		if (!ptr->head) {
			printf("compare %s == %s\n", ptr->ip, ip);
			if (equal(ptr->ip, ip)) return true;
		}

		ptr = ptr->next;
	}

	return false;
}

internal void
add_ip(IP_List_Node *head, const char *ip)
{
	printf("add_ip() ip: %s\n", ip);
	IP_List_Node *ptr = head;
	while (ptr->next != 0) {
		ptr = ptr->next;
	}

	ptr->next = (IP_List_Node *)malloc(sizeof(IP_List_Node));
	memset((void*)ptr->next, 0, sizeof(IP_List_Node));

	u32 ip_length = get_length(ip);
	ptr->next->ip = (char *)malloc(ip_length + 1);
	memset((void*)ptr->next->ip, 0, ip_length + 1);
	memcpy((void*)ptr->next->ip, (void*)ip, ip_length);
}

internal char*
scuffed_concat(char *left, const char *right)
{
	char *new_text = string_malloc_concat(left, right);
	free(left);
	return new_text;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "run voting like this: ./ms_voting <port>\n");
		return 1;
	} 

	qsock_init_qsock();

	IP_List_Node ip_list = {};
	ip_list.head = true;

	// Save the time when the micro service was started.
    struct timespec time;
    struct tm *timeinfo;
    timespec_get(&time, TIME_UTC);
    time_t time_voting_opened = time.tv_sec;
    int voting_length = 15; // in seconds

	char *port = argv[1];
	QSock_Socket server = {};
	qsock_server(&server, port, UDP);

	while(1) {
		QSock_Socket client = {};
		Message received = recv_message(server, &client);

		// The time of the most request.
        timespec_get(&time, TIME_UTC);
        time_t time_now = time.tv_sec;
        fprintf(stderr, "timeNow: %ld votingStart: %ld\n", (long)time_now, (long)time_voting_opened);
        int time_since_opened = (int)time_now - (int)time_voting_opened; // calculate how long it has been since the micro server started

		// Using the micro service again
		if (equal(received.system, "Connecting")) {
			printf("Start Message Sending\n");
			
			Message m = {};
			m.prompt = string_malloc("Enter your vote: ");
			m.text = string_malloc("Connected to voting service.\nType quit to leave voting.\nCandidates:\n");
			for (u32 i = 0; i < ARRAY_COUNT(candidates); i++) {
				m.text = scuffed_concat(m.text, candidates[i].name);
				m.text = scuffed_concat(m.text, " ");
				char id[6];
				sprintf(id, "%d\n", candidates[i].id);
				id[5] = 0;
				m.text = scuffed_concat(m.text, id);
			}

			m.key = KEY;
			m.system = string_malloc("clear");
			
			send_message(server, &client, m);
			free_message(m);
		}
		else if (equal(received.system, "Summary")) {
			if (time_since_opened <= voting_length) {
				// voting is not done yet
				Message m = {};

				time_t closing_time = time_voting_opened + voting_length;
				timeinfo = localtime(&closing_time);

				m.text = (char *)malloc(100);
				sprintf(m.text, "\nVoting ends at %sAfter that the results can be viewed\n\n", asctime(timeinfo));
				m.prompt = string_malloc("Command: ");
				send_message(server, &client, m);
				free_message(m);
			}
			else {
				Message m = {};

				m.text = string_malloc("\nVoting Summary:\n");

				for (u32 i = 0; i < ARRAY_COUNT(candidates); i++) {
					char y[100];
					memset(y, 0, 100);
					sprintf(y, "%s %d: %d\n", candidates[i].name, candidates[i].id, candidates[i].votes);
					m.text = scuffed_concat(m.text, y);
				}

				m.text = scuffed_concat(m.text, "\n");
				m.prompt = string_malloc("Command: ");
				send_message(server, &client, m);
				free_message(m);
			}
		}
		else {
			// Voting
			if (time_since_opened > voting_length) {
				Message m = {};
				m.text = string_malloc("The time to vote is over.\nType quit to stop using the voting micro server.\n");
				m.prompt = string_malloc("Enter your vote: ");
				send_message(server, &client, m);
				free_message(m);
			}
			else if (already_voted(&ip_list, qsock_get_ip(client.info))) {
				printf("already_voted\n");
				Message m = {};
				m.text = string_malloc("You have already voted.\nType quit to stop using the voting micro server.\n");
				m.prompt = string_malloc("Enter your vote: ");
				send_message(server, &client, m);
				free_message(m);
			}
			else {
				int encrypted = atoi(received.text);
				int unencrypted = encrypted / KEY;
				bool32 valid_id = false;
				printf("encrypted vote %d\n", encrypted);

				for (u32 i = 0; i < ARRAY_COUNT(candidates); i++) {
					if (unencrypted == candidates[i].id) {
						valid_id = true;
						candidates[i].votes++;
					}
				}

				Message m = {};
				if (valid_id) {
					add_ip(&ip_list, qsock_get_ip(client.info));
					m.text = string_malloc("Vote successful.\nType quit to leave voting.\n");
					m.prompt = string_malloc("Enter your vote: ");
				}
				else {
					m.text = string_malloc("Not a valid id.\n");
					m.prompt = string_malloc("Enter your vote: ");
					m.key = KEY;
				}
				send_message(server, &client, m);
				free_message(m);
			}
		}

		free_message(received);
	}

	qsock_free_socket(server);
	return 0;
}