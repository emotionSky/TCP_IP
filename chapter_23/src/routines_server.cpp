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
typedef struct
{
	sock_t hClntSock;
	char   buf[BUFFER_SIZE];
	WSABUF wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

void CALLBACK ReadCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
	LPPER_IO_DATA io_data = (LPPER_IO_DATA)(lpOverlapped->hEvent);
	sock_t client_sock = io_data->hClntSock;

	if (szRecvBytes == 0)
	{
		closesocket(client_sock);
		free(lpOverlapped->hEvent); 
		free(lpOverlapped);
		print_console(PRINT_INFOR, "Client disconnected.....");
	}
	else    // echo!
	{
		LPWSABUF data_buf = &(io_data->wsaBuf);
		data_buf->len = szRecvBytes;
		DWORD send_bytes = 0;
		WSASend(client_sock, data_buf, 1, &send_bytes, 0, lpOverlapped, WriteCompRoutine);
	}
}

void CALLBACK WriteCompRoutine(DWORD dwError, DWORD szSendBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
	LPPER_IO_DATA io_data = (LPPER_IO_DATA)(lpOverlapped->hEvent);
	sock_t client_sock = io_data->hClntSock;
	LPWSABUF data_buf = &(io_data->wsaBuf);
	DWORD recv_bytes;
	flags = 0;
	WSARecv(client_sock, data_buf, 1, &recv_bytes, &flags, lpOverlapped, ReadCompRoutine);
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

	u_long mode = 1;
	ioctlsocket(server_sock, FIONBIO, &mode);   // for non-blocking socket

	DWORD recv_bytes = 0;
	LPPER_IO_DATA io_data;
	DWORD flags = 0;
	SOCKADDR_IN client_addr;
	int client_addr_size = static_cast<int>(sizeof(client_addr));
	sock_t client_sock = invalid_sock;

	while (true)
	{
		SleepEx(100, TRUE);    // for alertable wait state
		client_sock = accept(server_sock, (SOCKADDR*)&client_addr, &client_addr_size);
		if (client_sock == invalid_sock)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			else
				print_console(PRINT_ERROR, "accept() error");
		}
		print_console(PRINT_INFOR, "Client connected.....");

		LPWSAOVERLAPPED overlapped = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
		memset(overlapped, 0, sizeof(WSAOVERLAPPED));

		io_data = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		io_data->hClntSock = (DWORD)client_sock;
		(io_data->wsaBuf).buf = io_data->buf;
		(io_data->wsaBuf).len = BUFFER_SIZE;

		overlapped->hEvent = (HANDLE)io_data;
		WSARecv(client_sock, &(io_data->wsaBuf), 1, &recv_bytes, &flags, overlapped, ReadCompRoutine);
	}

	if (client_sock != invalid_sock)
	{
		close_socket(client_sock);
	}
	close_socket(server_sock);
	
	CleanSocketEnv();
#else
	print_console(PRINT_INFOR, "not support events & WSASOCKET on linux.");
#endif
	return 0;
}


