#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32) || defined(_WINDLL)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#ifdef _MSC_VER
#pragma  warning( disable: 4267 4996 )
#endif
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		print_console(PRINT_ERROR, "args not valid!");
		print_console(PRINT_INFOR, "Usage: %s <IP> <port>", argv[0]);
		return -1;
	}

	if (!InitSocketEnv())
	{
		print_console(PRINT_ERROR, "failed to init socket env.");
		return -1;
	}

	sock_t client_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (client_sock == invalid_sock)
	{
		print_console(PRINT_ERROR, "failed to get socket!");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to connect server!");
		return -1;
	}

#if defined(_WIN32) || defined(_WINDLL)
	// windows 并不支持这种处理
	int recv_len;
#else
	FILE* read_fp = fdopen(client_sock, "r");
	FILE* write_fp = fdopen(client_sock, "w");
#endif
	char message[BUFFER_SIZE];

	while (true)
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUFFER_SIZE - 1, stdin);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

#if defined(_WIN32) || defined(_WINDLL)
		send(client_sock, message, static_cast<int>(strlen(message)), 0);
		recv_len = recv(client_sock, message, BUFFER_SIZE - 1, 0);
		if (recv_len > 0)
			message[recv_len] = 0;
#else
		fputs(message, write_fp);
		fflush(write_fp);
		fgets(message, BUFFER_SIZE - 1, read_fp);
#endif
		print_console(PRINT_INFOR, "Message from server: %s", message);
	}

#if defined(_WIN32) || defined(_WINDLL)
	close_socket(client_sock);
#else
	fclose(write_fp);
	fclose(read_fp);
#endif

	CleanSocketEnv();
	return 0;
}
