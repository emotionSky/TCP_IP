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

#if defined(_WIN32) || defined(_WINDLL)
WSABUF data_buf;
char buf[BUFFER_SIZE];
DWORD recv_bytes = 0;

// 这个函数保留 Windows 的匈牙利命名法，着实丑陋……
void CALLBACK CompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
	if (dwError != 0)
	{
		print_console(PRINT_ERROR, "CompRoutine error");
	}
	else
	{
		recv_bytes = szRecvBytes;
		print_console(PRINT_INFOR, "Received message: %s", buf);
	}
}
#endif

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	if (argc != 2)
	{
		print_console(PRINT_ERROR, "args not valid!");
		print_console(PRINT_INFOR, "Usage: %s <port>", argv[0]);
		return -1;
	}

	if (!InitSocketEnv())
	{
		print_console(PRINT_ERROR, "failed to init socket env.");
		return -1;
	}

	sock_t server_sock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (SOCKADDR*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to bind port: %s", argv[1]);
		return -1;
	}

	if (listen(server_sock, 5) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to listen port: %s", argv[1]);
		return -1;
	}

	SOCKADDR_IN client_addr;
	int client_addr_size = static_cast<int>(sizeof(client_addr));
	sock_t client_sock = accept(server_sock, (SOCKADDR*)&client_addr, &client_addr_size);

	WSAOVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(overlapped));
	data_buf.len = BUFFER_SIZE;
	data_buf.buf = buf;
	WSAEVENT event = WSACreateEvent();     //Dummy event object 
	DWORD flags = 0;

	if (WSARecv(client_sock, &data_buf, 1, &recv_bytes, &flags, &overlapped, CompRoutine) == sock_error)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
			print_console(PRINT_INFOR, "Background data receive");
	}

	DWORD idx = WSAWaitForMultipleEvents(1, &event, FALSE, WSA_INFINITE, TRUE);
	if (idx == WAIT_IO_COMPLETION)
		print_console(PRINT_INFOR, "Overlapped I/O Completed");
	else    // If error occurred!
		print_console(PRINT_ERROR, "WSARecv() error");

	WSACloseEvent(event);
	close_socket(client_sock);
	close_socket(server_sock);
	
	CleanSocketEnv();
#else
	print_console(PRINT_INFOR, "not support events & WSASOCKET on linux.");
#endif
	return 0;
}
