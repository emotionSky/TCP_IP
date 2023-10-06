#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32) || defined(_WINDLL)
#include <DreamSky/dream_thread.h>
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
class Work
{
public:
	Work(sock_t sock) : m_sock(sock) {}

	void MainWork()
	{
		read_routine(m_sock, m_mainBuf);
		m_thread.WaitThread(-1);
	}

	void ThreadWork()
	{
		write_routine(m_sock, m_thdBuf);
	}

	bool RunThread()
	{
		if (!m_thread.CreateOneThread(std::bind(&Work::ThreadWork, this)))
		{
			print_console(PRINT_ERROR, "failed to create thread: %s", m_thread.GetErrorString().c_str());
			return false;
		}
		return true;
	}

	void read_routine(sock_t sock, char* buf)
	{
		while (true)
		{
			int str_len = recv(sock, buf, BUFFER_SIZE - 1, 0);
			if (str_len == 0)
				return;

			buf[str_len] = 0;
			print_console(PRINT_INFOR, "Message from server: %s", buf);
		}
	}

	void write_routine(sock_t sock, char* buf)
	{
		while (true)
		{
			fgets(buf, BUFFER_SIZE - 1, stdin);
			if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
			{
#if defined(_WIN32) || defined(_WINDLL)
				shutdown(sock, SD_SEND); //win:  SD_RECEIVE  SD_SEND  SD_BOTH
#else
				shutdown(sock, SHUT_WR);
#endif
				return;
			}
			send(sock, buf, static_cast<int>(strlen(buf)), 0);
		}
	}

private:
	sock_t     m_sock;
	char       m_mainBuf[BUFFER_SIZE];
	char       m_thdBuf[BUFFER_SIZE];
	DWinThread m_thread;
};

#else
void read_routine(sock_t sock, char* buf)
{
	while (true)
	{
		int str_len = recv(sock, buf, BUFFER_SIZE - 1, 0);
		if (str_len == 0)
			return;

		buf[str_len] = 0;
		print_console(PRINT_INFOR, "Message from server: %s", buf);
	}
}

void write_routine(sock_t sock, char* buf)
{
	while (true)
	{
		fgets(buf, BUFFER_SIZE, stdin);
		if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
		{
			shutdown(sock, SHUT_WR);
			return;
		}
		send(sock, buf, static_cast<int>(strlen(buf)), 0);
	}
}
#endif

int main(int argc, char *argv[])
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
	// AF_INET  IPv4互联网协议族
	// AF_INET6 IPv6互联网协议族
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr); //使用窄字符
	//InetPton 使用宽字符
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to connect server!");
		return -1;
	}
	
#if defined(_WIN32) || defined(_WINDLL)
	{
		Work w(client_sock);
		if (w.RunThread())
		{
			w.MainWork();
		}
	}
#else
	char buf[BUFFER_SIZE];
	pid_t pid = fork();
	if (pid == 0)
		write_routine(client_sock, buf);
	else
		read_routine(client_sock, buf);
#endif

	close_socket(client_sock);
	CleanSocketEnv();
	return 0;
}
