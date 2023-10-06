#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

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

	char buf[BUFFER_SIZE];
#if defined(_WIN32) || defined(_WINDLL)
#else
	FILE* read_fp = fdopen(client_sock, "r");
	FILE* write_fp = fdopen(client_sock, "w");
#endif

	while (true)
	{
#if defined(_WIN32) || defined(_WINDLL)
		int recv_len = recv(client_sock, buf, BUFFER_SIZE - 1, 0);
		if(recv_len <= 0)
			break;
		buf[recv_len] = 0;
#else
		if (fgets(buf, sizeof(buf), read_fp) == nullptr)
			break;
#endif
		fputs(buf, stdout);
		fflush(stdout);
	}

#if defined(_WIN32) || defined(_WINDLL)
	const char* res = "FROM CLIENT: Thank you! \n";
	send(client_sock, res, static_cast<int>(strlen(res)), 0);
	close_socket(client_sock);
#else
	fputs("FROM CLIENT: Thank you! \n", write_fp);
	fflush(write_fp);
	fclose(write_fp);
	fclose(read_fp);
#endif

	CleanSocketEnv();
	return 0;
}
