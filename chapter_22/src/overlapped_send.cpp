#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>

#if defined(_WIN32) || defined(_WINDLL)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
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

	sock_t client_sock = WSASocket(PF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(client_sock, (SOCKADDR*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to connect server!");
		return -1;
	}

	WSAEVENT event = WSACreateEvent();
	WSAOVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = event;
	WSABUF data_buf;
	char msg[] = "Network is Computer!";	
	data_buf.len = strlen(msg) + 1;
	data_buf.buf = msg;

	DWORD send_bytes = 0;
	if (WSASend(client_sock, &data_buf, 1, &send_bytes, 0, &overlapped, nullptr) == sock_error)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			print_console(PRINT_INFOR, "Background data send");
			WSAWaitForMultipleEvents(1, &event, TRUE, WSA_INFINITE, FALSE);
			WSAGetOverlappedResult(client_sock, &overlapped, &send_bytes, FALSE, nullptr);
		}
		else
		{
			print_console(PRINT_ERROR, "WSASend() error");
		}
	}

	print_console(PRINT_INFOR, "Send data size: %d", send_bytes);
	WSACloseEvent(event);
	close_socket(client_sock);
	
	CleanSocketEnv();
#else
	print_console(PRINT_INFOR, "not support events & WSASOCKET on linux.");
#endif
	return 0;
}
