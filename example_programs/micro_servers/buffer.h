#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>

#define BUFFER_SIZE 1000

struct Message
{
	char *prompt; // malloc
	char *system; // malloc
	char *text;   // malloc

	int prompt_length;
	int system_length;
	int text_length;
	int key;
};

global char global_buffer[BUFFER_SIZE];

#define KEY 2387

inline char*
string_malloc_concat(const char *string, const char *add)
{
	if (string == 0 || add == 0) return 0;
	u32 string_length = get_length(string);
	u32 add_length = get_length(add);
	char *result = (char *)malloc(string_length + add_length + 1);

	u32 result_index = 0;
	for (u32 i = 0; i < string_length; i++) result[result_index++] = string[i];
	for (u32 i = 0; i < add_length;    i++) result[result_index++] = add[i];

	result[string_length + add_length] = 0;
    return result;
}

inline const char*
copy_string(const char *buffer, const char *string, int string_length)
{
	if (string_length == 0) return buffer;

	memcpy((void*)buffer, (void*)string, string_length);
	//free((void*)string);

	return buffer + string_length;
}

inline const char*
copy_string(const char *buffer, char **string, int string_length)
{
	if (string_length == 0) return buffer;

	*string = (char *)malloc(string_length + 1);
	memset((void*)*string, 0, string_length + 1);
	memcpy((void*)*string, (void*)buffer, string_length);
	return buffer + string_length;
}

internal void
copy_message_to_buffer(Message m, const char *buffer)
{
	m.prompt_length = get_length(m.prompt);
	m.system_length = get_length(m.system);
	m.text_length   = get_length(m.text);

	memcpy((void*)buffer, (void*)&m, sizeof(Message));
	buffer += sizeof(Message);

	buffer = copy_string(buffer, m.prompt, m.prompt_length);
	buffer = copy_string(buffer, m.system, m.system_length);
	buffer = copy_string(buffer, m.text,   m.text_length);
}

internal Message
copy_buffer_to_message(const char *buffer)
{
	Message m = {};

	memcpy((void*)&m, (void*)buffer, sizeof(Message));
	buffer += sizeof(Message);

	buffer = copy_string(buffer, &m.prompt, m.prompt_length);
	buffer = copy_string(buffer, &m.system, m.system_length);
	buffer = copy_string(buffer, &m.text,   m.text_length);

	return m;
}

inline void
send_message(QSock_Socket socket, QSock_Socket *connected, Message m)
{
	memset(global_buffer, 0, BUFFER_SIZE);
	copy_message_to_buffer(m, global_buffer);
	qsock_send(socket, connected, global_buffer, BUFFER_SIZE);
}

inline Message
recv_message(QSock_Socket socket, QSock_Socket *connected)
{
	Message m = {};
	int bytes = qsock_recv(socket, connected, global_buffer, BUFFER_SIZE);
	if (bytes == 0) {
		printf("Zero bytes received\n");
		return m;
	}
	m = copy_buffer_to_message(global_buffer);
	return m;
}

inline void
free_message(Message m)
{
	if (m.prompt != 0) free(m.prompt);
	if (m.system != 0) free(m.system);
	if (m.text   != 0) free(m.text);
}