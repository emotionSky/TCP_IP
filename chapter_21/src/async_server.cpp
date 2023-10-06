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
void CompressSockets(sock_t arr_socks[], int idx, int total)
{
	for (int i = idx; i < total; i++)
		arr_socks[i] = arr_socks[i + 1];
}

void CompressEvents(WSAEVENT arr_events[], int idx, int total)
{
	for (int i = idx; i < total; i++)
		arr_events[i] = arr_events[i + 1];
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

	sock_t server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (server_sock == invalid_sock)
	{
		print_console(PRINT_ERROR, "failed to get socket!");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	// AF_INET  IPv4互联网协议族
	// AF_INET6 IPv6互联网协议族
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr); //使用窄字符
	//InetPton 使用宽字符
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to bind port: %s", argv[1]);
		return -1;
	}

	if (listen(server_sock, 5) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to listen port: %s", argv[1]);
		return -1;
	}

	sock_t arr_socks[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT arr_events[WSA_MAXIMUM_WAIT_EVENTS];
	WSANETWORKEVENTS net_events;

	WSAEVENT new_event = WSACreateEvent();
	if (WSAEventSelect(server_sock, new_event, FD_ACCEPT) == sock_error)
	{
		print_console(PRINT_ERROR, "WSAEventSelect() error");
		return -1;
	}

	int client_sock_count = 0;
	arr_socks[client_sock_count] = server_sock;
	arr_events[client_sock_count] = new_event;
	client_sock_count++;

	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	sock_t client_sock = invalid_sock;
	int str_len = 0;
	char msg[BUFFER_SIZE];

	while (true)
	{
		int posInfo = WSAWaitForMultipleEvents(client_sock_count, arr_events, FALSE, WSA_INFINITE, FALSE);
		int start_index = posInfo - WSA_WAIT_EVENT_0;

		for (int i = start_index; i < client_sock_count; i++)
		{
			int sig_event_index = WSAWaitForMultipleEvents(1, &arr_events[i], TRUE, 0, FALSE);
			if ((sig_event_index == WSA_WAIT_FAILED || sig_event_index == WSA_WAIT_TIMEOUT))
			{
				continue;
			}
			else
			{
				sig_event_index = i;
				WSAEnumNetworkEvents(
					arr_socks[sig_event_index], arr_events[sig_event_index], &net_events);
				if (net_events.lNetworkEvents & FD_ACCEPT)
				{
					if (net_events.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						print_console(PRINT_ERROR, "Accept Error");
						break;
					}
					client_sock = accept(arr_socks[sig_event_index], (SOCKADDR*)&client_addr, &client_addr_size);
					new_event = WSACreateEvent();
					WSAEventSelect(client_sock, new_event, FD_READ | FD_CLOSE);

					arr_events[client_sock_count] = new_event;
					arr_socks[client_sock_count] = client_sock;
					client_sock_count++;
					print_console(PRINT_INFOR, "connected new client...");
				}

				if (net_events.lNetworkEvents & FD_READ)
				{
					if (net_events.iErrorCode[FD_READ_BIT] != 0)
					{
						print_console(PRINT_ERROR, "Read Error");
						break;
					}
					str_len = recv(arr_socks[sig_event_index], msg, sizeof(msg), 0);
					send(arr_socks[sig_event_index], msg, str_len, 0);
				}

				if (net_events.lNetworkEvents & FD_CLOSE)
				{
					if (net_events.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						print_console(PRINT_ERROR, "Close Error");
						break;
					}
					WSACloseEvent(arr_events[sig_event_index]);
					closesocket(arr_socks[sig_event_index]);

					client_sock_count--;
					CompressSockets(arr_socks, sig_event_index, client_sock_count);
					CompressEvents(arr_events, sig_event_index, client_sock_count);
				}
			}
		}
	}
	WSACleanup();
#else
	print_console(PRINT_INFOR, "not support events on linux.");
#endif
	return 0;
}
