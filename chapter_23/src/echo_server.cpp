#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <DreamSky/dream_thread.h>

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
#define READ	3
#define	WRITE	5

typedef struct    // socket info
{
	sock_t hClntSock;
	SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

typedef struct    // buffer info
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUFFER_SIZE];
	int rwMode;    // READ or WRITE
} PER_IO_DATA, * LPPER_IO_DATA;

unsigned int EchoThreadMain(void* pComPort);

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

	d_handle iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);

	DWinThreadExManager thread_manager;
	for (DWORD i = 0; i < sys_info.dwNumberOfProcessors; i++)
	{
		thread_manager.AllocNewThread(EchoThreadMain, iocp);
	}

	sock_t server_sock = WSASocket(PF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);

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

	DWORD recvBytes, flags = 0;
	SOCKADDR_IN client_addr;
	int client_addr_size = static_cast<int>(sizeof(client_addr));
	sock_t client_sock = invalid_sock;

	while (true)
	{
		client_sock = accept(server_sock, (SOCKADDR*)&client_addr, &client_addr_size);

		LPPER_HANDLE_DATA handle_data = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		handle_data->hClntSock = client_sock;
		memcpy(&(handle_data->clntAdr), &client_addr, client_addr_size);

		CreateIoCompletionPort((HANDLE)client_sock, iocp, (ULONG_PTR)handle_data, 0);

		LPPER_IO_DATA io_data = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(io_data->overlapped), 0, sizeof(OVERLAPPED));
		io_data->wsaBuf.len = BUFFER_SIZE;
		io_data->wsaBuf.buf = io_data->buffer;
		io_data->rwMode = READ;

		WSARecv(handle_data->hClntSock, &(io_data->wsaBuf),	1, &recvBytes, &flags, &(io_data->overlapped), nullptr);
	}

	close_socket(server_sock);
	CleanSocketEnv();
#else
	print_console(PRINT_INFOR, "not support iocp & WSASOCKET on linux.");
#endif
	return 0;
}

#if defined(_WIN32) || defined(_WINDLL)
unsigned int EchoThreadMain(void* iocp)
{
	DWORD bytes_trans;
	LPPER_HANDLE_DATA handle_data;
	LPPER_IO_DATA io_data;
	DWORD flags = 0;

	while (true)
	{
		GetQueuedCompletionStatus(iocp, &bytes_trans, (PULONG_PTR)&handle_data, (LPOVERLAPPED*)&io_data, INFINITE);
		sock_t sock = handle_data->hClntSock;

		if (io_data->rwMode == READ)
		{
			print_console(PRINT_INFOR, "message received!");
			if (bytes_trans == 0)
			{
				closesocket(sock);
				free(handle_data); 
				free(io_data);
				continue;
			}

			memset(&(io_data->overlapped), 0, sizeof(OVERLAPPED));
			io_data->wsaBuf.len = bytes_trans;
			io_data->rwMode = WRITE;
			WSASend(sock, &(io_data->wsaBuf), 1, nullptr, 0, &(io_data->overlapped), nullptr);

			io_data = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			memset(&(io_data->overlapped), 0, sizeof(OVERLAPPED));
			io_data->wsaBuf.len = BUFFER_SIZE;
			io_data->wsaBuf.buf = io_data->buffer;
			io_data->rwMode = READ;
			WSARecv(sock, &(io_data->wsaBuf), 1, nullptr, &flags, &(io_data->overlapped), nullptr);
		}
		else
		{
			print_console(PRINT_INFOR, "message sent!");
			free(io_data);
		}
	}
	return 0;
}
#endif
