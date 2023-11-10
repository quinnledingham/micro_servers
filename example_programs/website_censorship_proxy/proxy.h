struct Proxy {
	char *buffer;
	int bytes;
	struct Socket server;
	struct Socket client;
};

struct LL_Node {
	char keyword[100];
	struct LL_Node *next;
};

struct LL {
	struct LL_Node *head;
};

int get_length(const char *string)
{
    if (string == 0)
        return 0;
    
    u32 length = 0;
    const char *ptr = string;
    while(*ptr != 0)
    {
        length++;
        ptr++;
    }
    return length;
}

bool equal(const char* a, const char *b)
{
    if (a == 0 && b == 0) return true;
    if (a == 0 || b == 0) return false;
    
    int i = 0;
    do
    {
        if (a[i] != b[i])
            return false;
    } while(a[i] != 0 && b[i++] != 0);
    
    return true;
}
