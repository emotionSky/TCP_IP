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
#include <sys/wait.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;

#if defined(_WIN32) || defined(_WINDLL)
class Work
{
public:
	Work(sock_t sock) : m_serverSock(sock) {}

	void MainWork()
	{
		print_console(PRINT_INFOR, "main thread");
		m_thread.WaitThread(-1);
	}

	void ThreadWork()
	{
		struct sockaddr_in client_addr;
		socklen_t client_addr_size = sizeof(client_addr);
		while (true)
		{
			int str_len = 0;
			sock_t client_sock = accept(m_serverSock, (struct sockaddr*)&client_addr, &client_addr_size);

			if(client_sock == invalid_sock)
				continue;

			while ((str_len = recv(client_sock, m_buf, BUFFER_SIZE - 1, 0)) != 0)
				send(client_sock, m_buf, str_len, 0);
		}
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

private:
	sock_t m_serverSock;
	char m_buf[BUFFER_SIZE];
	DWinThread m_thread;
};
#else
void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG);
	print_console(PRINT_INFOR, "removed proc id: %d", pid);
}
#endif

int main(int argc, char *argv[])
{
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

#if defined(_WIN32) || defined(_WINDLL)
#else
	struct sigaction act;
	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	int state = sigaction(SIGCHLD, &act, 0);

	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
#endif

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

#if defined(_WIN32) || defined(_WINDLL)
	Work w(server_sock);
	if (w.RunThread())
	{
		w.MainWork();
	}
#else
	int str_len = 0;
	char buf[BUFFER_SIZE];

	while (1)
	{
		sock_t client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
		if (client_sock == -1)
			continue;
		else
			print_console(PRINT_INFOR, "new client connected...");

		pid_t pid = fork();
		if (pid == -1)
		{
			close_socket(client_sock);
			continue;
		}
		if (pid == 0)
		{
			close_socket(server_sock);
			while ((str_len = recv(client_sock, buf, BUFFER_SIZE - 1, 0)) != 0)
				send(client_sock, buf, str_len, 0);

			close_socket(client_sock);
			print_console(PRINT_INFOR, "client disconnected...");
			return 0;
		}
		else
			close_socket(client_sock);
	}
	close_socket(server_sock);
#endif

	CleanSocketEnv();
	return 0;
}


