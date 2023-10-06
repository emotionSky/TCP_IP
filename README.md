# TCP_IP

[TOC]

## 1. 项目说明

根据《TCP/IP网络编程》书籍学习，对其中的代码进行整理，并且对应做了一些优化处理，将部分通用的部分合并到`DreamSky`这个库中，然后在本工程的文件中进行调用。

### 1.1 项目特点

- 全部使用`cpp`进行重新开发，并且要求`c++11`及以上的标准。
- 在`Windows`和`Linux`平台下均可以直接编译，无需文件和平台强相关。
- 使用[`xmake`](https://xmake.io/#/)作为项目的构建工具，支持多平台的快速构建。
- `Windows`下额外提供了标准的`vs sln`解决方案，方便代码阅读和直接IDE调试。
- 统一命名规范，避免原书代码的杂乱。
- 对编程基础要求很低……真的很低……

## 2. 文件说明

### 2.1 脚本文件

#### 2.1.1 `TCP_IP.sln`

`vs`的解决方案文件，并且处理好了各种依赖关系，可以直接一键编译，不需要进行任何额外处理。

> **注意：**
>
> 这个解决方案中，只做了`x64 debug`的工程设计，其余的平台和模式的直接参考这个进行处理即可，属于重复工作量……

#### 2.1.2 `xmake.lua`

`xmake`的构建脚本，处理好了子目录的各级编译，也只需要命令行直接编译即可，但是在编译本工程之前需要先准备好[`DreamSky`](https://github.com/emotionSky/DreamSky)的`xmake`打包文件，可参考[工程说明](https://github.com/emotionSky/DreamSky/blob/main/README.md)进行编译处理。

##### 2.1.2.1 编译说明

假设在[`DreamSky`](https://github.com/emotionSky/DreamSky)中已经使用`xmake`进行了编译，并且打包到了`TCP_IP/DreamSky`目录下，那么这里既可以在`xmake.lua`中配置`DreamSky`包的目录：

```lua
-- 这是 DreamSky 的库
add_repositories("local-repo DreamSky")
add_requires("dreamcommon")
add_requires("dreamsocket")
add_requires("dreamprocess")
add_requires("dreamthread")
```

这样配置之后，可以按照如下的流程进行编译：

```shell
xmake -v
```

##### 2.1.2.2 运行说明

`xmake`编译后的产物如何运行可以参考[官方教程](https://xmake.io/#/zh-cn/?id=%e8%bf%90%e8%a1%8c%e7%9b%ae%e6%a0%87)，这里给出一个简单的示例：

在一个终端中运行：

```shell
xmake run ch1_hello_server 9000
```

在另一个终端中运行：

```shell
xmake run ch1_hello_client 127.0.0.1 9000
```

#### 2.1.3 章节说明

每一个章节对应一个`chapter_xx`的目录，所有的代码文件都在该目录下的`src`目录中，对应的`vcproj`目录中有`vs`的构建脚本。

章节文件对应图：

- chapter_01

```shell
chapter_01
├── src
│   ├── hello_client.cpp
│   └── hello_server.cpp
├── vcproj
│   ├── hello_client.vcxproj
│   ├── hello_client.vcxproj.filters
│   ├── hello_server.vcxproj
│   ├── hello_server.vcxproj.filters
│   ├── post_client.bat
│   └── post_server.bat
└── xmake.lua
```

- chapter_02

```shell
chapter_02
├── src
│   ├── tcp_client.cpp
│   └── tcp_server.cpp
├── vcproj
│   ├── post_client.bat
│   ├── post_server.bat
│   ├── tcp_client.vcxproj
│   ├── tcp_client.vcxproj.filters
│   ├── tcp_server.vcxproj
│   └── tcp_server.vcxproj.filters
└── xmake.lua
```

- chapter_03

```shell
chapter_03
├── src
│   ├── endian_conv.cpp
│   ├── inet_addr.cpp
│   ├── inet_aton.cpp
│   └── inet_ntoa.cpp
├── vcproj
│   ├── endian_conv.vcxproj
│   ├── endian_conv.vcxproj.filters
│   ├── inet_addr.vcxproj
│   ├── inet_addr.vcxproj.filters
│   ├── inet_aton.vcxproj
│   ├── inet_aton.vcxproj.filters
│   ├── inet_ntoa.vcxproj
│   ├── inet_ntoa.vcxproj.filters
│   ├── post_addr.bat
│   ├── post_aton.bat
│   ├── post_conv.bat
│   └── post_ntoa.bat
└── xmake.lua
```

- chapter_04

```shell
chapter_04
├── src
│   ├── echo_client.cpp
│   ├── echo_server.cpp
│   ├── hello_client.cpp
│   └── hello_server.cpp
├── vcproj
│   ├── echo_client.vcxproj
│   ├── echo_client.vcxproj.filters
│   ├── echo_server.vcxproj
│   ├── echo_server.vcxproj.filters
│   ├── hello_client.vcxproj
│   ├── hello_client.vcxproj.filters
│   ├── hello_server.vcxproj
│   ├── hello_server.vcxproj.filters
│   ├── post_client.bat
│   ├── post_echo_client.bat
│   ├── post_echo_server.bat
│   └── post_server.bat
└── xmake.lua
```

- chapter_05

```shell
chapter_05
├── src
│   ├── op_client.cpp
│   └── op_server.cpp
├── vcproj
│   ├── op_client.vcxproj
│   ├── op_client.vcxproj.filters
│   ├── op_server.vcxproj
│   ├── op_server.vcxproj.filters
│   ├── post_client.bat
│   └── post_server.bat
└── xmake.lua
```

- chapter_06

```shell
chapter_06
├── src
│   ├── uecho_client.cpp
│   └── uecho_server.cpp
├── vcproj
│   ├── post_client.bat
│   ├── post_server.bat
│   ├── uecho_client.vcxproj
│   ├── uecho_client.vcxproj.filters
│   ├── uecho_server.vcxproj
│   └── uecho_server.vcxproj.filters
└── xmake.lua
```

- chapter_07

```shell
chapter_07
├── file
│   └── file_client.dat
├── src
│   ├── file_client.cpp
│   └── file_server.cpp
├── vcproj
│   ├── file_client.vcxproj
│   ├── file_client.vcxproj.filters
│   ├── file_server.vcxproj
│   ├── file_server.vcxproj.filters
│   ├── post_client.bat
│   └── post_server.bat
└── xmake.lua
```

- chapter_08

```shell
chapter_08
├── src
│   ├── get_host_by_addr.cpp
│   └── get_host_by_name.cpp
├── vcproj
│   ├── get_host_by_addr.vcxproj
│   ├── get_host_by_addr.vcxproj.filters
│   ├── get_host_by_name.vcxproj
│   ├── get_host_by_name.vcxproj.filters
│   ├── post_addr.bat
│   └── post_name.bat
└── xmake.lua
```

- chapter_09

```shell
chapter_09
├── src
│   ├── get_buf.cpp
│   ├── set_buf.cpp
│   └── sock_type.cpp
├── vcproj
│   ├── get_buf.vcxproj
│   ├── get_buf.vcxproj.filters
│   ├── post_get.bat
│   ├── post_set.bat
│   ├── post_type.bat
│   ├── set_buf.vcxproj
│   ├── set_buf.vcxproj.filters
│   ├── sock_type.vcxproj
│   └── sock_type.vcxproj.filters
└── xmake.lua
```

- chapter_10

```shell
chapter_10
├── src
│   ├── echo_mpclient.cpp
│   ├── echo_mpserv.cpp
│   ├── fork.cpp
│   ├── remove_zombie.cpp
│   ├── sigaction.cpp
│   ├── signal.cpp
│   ├── wait.cpp
│   └── wait_pid.cpp
├── vcproj
│   ├── fork.vcxproj
│   ├── fork.vcxproj.filters
│   ├── mp_client.vcxproj
│   ├── mp_client.vcxproj.filters
│   ├── mp_server.vcxproj
│   ├── mp_server.vcxproj.filters
│   ├── post_client.bat
│   ├── post_fork.bat
│   ├── post_pid.bat
│   ├── post_rmzombie.bat
│   ├── post_server.bat
│   ├── post_sigaction.bat
│   ├── post_signal.bat
│   ├── post_wait.bat
│   ├── rmzombie.vcxproj
│   ├── rmzombie.vcxproj.filters
│   ├── sigaction.vcxproj
│   ├── sigaction.vcxproj.filters
│   ├── signal.vcxproj
│   ├── signal.vcxproj.filters
│   ├── wait_pid.vcxproj
│   ├── wait_pid.vcxproj.filters
│   ├── wait.vcxproj
│   └── wait.vcxproj.filters
└── xmake.lua
```

- chapter_11

```shell
chapter_11
├── src
│   ├── pipe1.cpp
│   ├── pipe2.cpp
│   └── pipe3.cpp
├── vcproj
│   ├── pipe1.vcxproj
│   ├── pipe1.vcxproj.filters
│   ├── pipe2.vcxproj
│   ├── pipe2.vcxproj.filters
│   ├── pipe3.vcxproj
│   ├── pipe3.vcxproj.filters
│   ├── post_pipe1.bat
│   ├── post_pipe2.bat
│   └── post_pipe3.bat
└── xmake.lua
```

- chapter_12

```shell
chapter_12
├── src
│   ├── echo_client.cpp
│   ├── echo_server.cpp
│   └── select.cpp
├── vcproj
│   ├── echo_client.vcxproj
│   ├── echo_client.vcxproj.filters
│   ├── echo_server.vcxproj
│   ├── echo_server.vcxproj.filters
│   ├── post_client.bat
│   ├── post_select.bat
│   ├── post_server.bat
│   ├── select.vcxproj
│   └── select.vcxproj.filters
└── xmake.lua
```

- chapter_13

```shell
chapter_13
├── src
│   ├── oob_recv.cpp
│   ├── oob_send.cpp
│   ├── peek_recv.cpp
│   ├── peek_send.cpp
│   ├── readv.cpp
│   └── writev.cpp
├── vcproj
│   ├── oob_recv.vcxproj
│   ├── oob_recv.vcxproj.filters
│   ├── oob_send.vcxproj
│   ├── oob_send.vcxproj.filters
│   ├── peek_recv.vcxproj
│   ├── peek_recv.vcxproj.filters
│   ├── peek_send.vcxproj
│   ├── peek_send.vcxproj.filters
│   ├── post_oob_recv.bat
│   ├── post_oob_send.bat
│   ├── post_peek_recv.bat
│   ├── post_peek_send.bat
│   ├── post_readv.bat
│   ├── post_writev.bat
│   ├── readv.vcxproj
│   ├── readv.vcxproj.filters
│   ├── writev.vcxproj
│   └── writev.vcxproj.filters
└── xmake.lua
```

- chapter_14

```shell
chapter_14
├── src
│   ├── broad_receiver.cpp
│   ├── broad_sender.cpp
│   ├── news_receiver.cpp
│   └── news_sender.cpp
├── vcproj
│   ├── broad_receiver.vcxproj
│   ├── broad_receiver.vcxproj.filters
│   ├── broad_sender.vcxproj
│   ├── broad_sender.vcxproj.filters
│   ├── news_receiver.vcxproj
│   ├── news_receiver.vcxproj.filters
│   ├── news_sender.vcxproj
│   ├── news_sender.vcxproj.filters
│   ├── news.txt
│   ├── post_broad_recver.bat
│   ├── post_broad_sender.bat
│   ├── post_recver.bat
│   └── post_sender.bat
└── xmake.lua
```

- chapter_15

```shell
chapter_15
├── src
│   ├── desto.cpp
│   ├── std_client.cpp
│   ├── stdcpy.cpp
│   ├── std_server.cpp
│   ├── syscpy.cpp
│   └── todes.cpp
├── vcproj
│   ├── desto.vcxproj
│   ├── desto.vcxproj.filters
│   ├── news.txt
│   ├── post_client.bat
│   ├── post_desto.bat
│   ├── post_server.bat
│   ├── post_stdcpy.bat
│   ├── post_syscpy.bat
│   ├── post_todes.bat
│   ├── std_client.vcxproj
│   ├── std_client.vcxproj.filters
│   ├── stdcpy.vcxproj
│   ├── stdcpy.vcxproj.filters
│   ├── std_server.vcxproj
│   ├── std_server.vcxproj.filters
│   ├── syscpy.vcxproj
│   ├── syscpy.vcxproj.filters
│   ├── todes.vcxproj
│   └── todes.vcxproj.filters
└── xmake.lua
```

- chapter_16

```shell
chapter_16
├── src
│   ├── dup.cpp
│   ├── sep_client.cpp
│   ├── sep_server1.cpp
│   └── sep_server2.cpp
├── vcproj
│   ├── dup.vcxproj
│   ├── dup.vcxproj.filters
│   ├── post_client.bat
│   ├── post_dup.bat
│   ├── post_server1.bat
│   ├── post_server2.bat
│   ├── sep_client.vcxproj
│   ├── sep_client.vcxproj.filters
│   ├── sep_server1.vcxproj
│   ├── sep_server1.vcxproj.filters
│   ├── sep_server2.vcxproj
│   └── sep_server2.vcxproj.filters
└── xmake.lua
```

- chapter_17

```shell
chapter_17
├── src
│   ├── echo_client.cpp
│   ├── echo_epet_server.cpp
│   ├── echo_eplt_server.cpp
│   └── echo_epoll_server.cpp
├── vcproj
│   ├── echo_client.vcxproj
│   ├── echo_client.vcxproj.filters
│   ├── echo_epet_server.vcxproj
│   ├── echo_epet_server.vcxproj.filters
│   ├── echo_eplt_server.vcxproj
│   ├── echo_eplt_server.vcxproj.filters
│   ├── echo_epoll_server.vcxproj
│   ├── echo_epoll_server.vcxproj.filters
│   ├── post_echo_client.bat
│   ├── post_epet_server.bat
│   ├── post_eplt_server.bat
│   └── post_epoll_server.bat
└── xmake.lua
```

- chapter_18

```shell
chapter_18
├── src
│   ├── chat_client.cpp
│   ├── chat_server.cpp
│   ├── mutex.cpp
│   ├── semaphore.cpp
│   └── thread.cpp
├── vcproj
│   ├── chat_client.vcxproj
│   ├── chat_client.vcxproj.filters
│   ├── chat_server.vcxproj
│   ├── chat_server.vcxproj.filters
│   ├── mutex.vcxproj
│   ├── mutex.vcxproj.filters
│   ├── post_client.bat
│   ├── post_mutex.bat
│   ├── post_semaphore.bat
│   ├── post_server.bat
│   ├── post_thread.bat
│   ├── semaphore.vcxproj
│   ├── semaphore.vcxproj.filters
│   ├── thread.vcxproj
│   └── thread.vcxproj.filters
└── xmake.lua
```

- chapter_19

```shell
chapter_19
├── src
│   ├── thread1.cpp
│   ├── thread2.cpp
│   └── thread3.cpp
├── vcproj
│   ├── post_thread1.bat
│   ├── post_thread2.bat
│   ├── post_thread3.bat
│   ├── thread1.vcxproj
│   ├── thread1.vcxproj.filters
│   ├── thread2.vcxproj
│   ├── thread2.vcxproj.filters
│   ├── thread3.vcxproj
│   └── thread3.vcxproj.filters
└── xmake.lua
```

- chapter_20

```shell
chapter_20
├── src
│   ├── chat_client.cpp
│   ├── chat_server.cpp
│   ├── sync_cs.cpp
│   ├── sync_event.cpp
│   ├── sync_mutex.cpp
│   └── sync_sem.cpp
├── vcproj
│   ├── chat_client.vcxproj
│   ├── chat_client.vcxproj.filters
│   ├── chat_server.vcxproj
│   ├── chat_server.vcxproj.filters
│   ├── post_client.bat
│   ├── post_cs.bat
│   ├── post_event.bat
│   ├── post_mutex.bat
│   ├── post_sem.bat
│   ├── post_server.bat
│   ├── sync_cs.vcxproj
│   ├── sync_cs.vcxproj.filters
│   ├── sync_event.vcxproj
│   ├── sync_event.vcxproj.filters
│   ├── sync_mutex.vcxproj
│   ├── sync_mutex.vcxproj.filters
│   ├── sync_sem.vcxproj
│   └── sync_sem.vcxproj.filters
└── xmake.lua
```

- chapter_21

```shell
chapter_21
├── src
│   ├── async_server.cpp
│   └── echo_client.cpp
├── vcproj
│   ├── async_server.vcxproj
│   ├── async_server.vcxproj.filters
│   ├── echo_client.vcxproj
│   ├── echo_client.vcxproj.filters
│   ├── post_echo_client.bat
│   └── post_server.bat
└── xmake.lua
```

- chapter_22

```shell
chapter_22
├── src
│   ├── cmpl_routines_recv.cpp
│   ├── overlapped_recv.cpp
│   └── overlapped_send.cpp
├── vcproj
│   ├── cmpl_routines_recv.vcxproj
│   ├── cmpl_routines_recv.vcxproj.filters
│   ├── overlapped_recv.vcxproj
│   ├── overlapped_recv.vcxproj.filters
│   ├── overlapped_send.vcxproj
│   ├── overlapped_send.vcxproj.filters
│   ├── post_recv.bat
│   ├── post_routines_recv.bat
│   └── post_send.bat
└── xmake.lua
```

- chapter_23

```shell
chapter_23
├── src
│   ├── echo_client.cpp
│   ├── echo_server.cpp
│   └── routines_server.cpp
├── vcproj
│   ├── echo_client.vcxproj
│   ├── echo_client.vcxproj.filters
│   ├── echo_server.vcxproj
│   ├── echo_server.vcxproj.filters
│   ├── post_client.bat
│   ├── post_routines_server.bat
│   ├── post_server.bat
│   ├── routines_server.vcxproj
│   └── routines_server.vcxproj.filters
└── xmake.lua
```

- chapter_24

```shell
chapter_24
├── src
│   └── web_server.cpp
├── vcproj
│   ├── post_server.bat
│   ├── web_server.vcxproj
│   └── web_server.vcxproj.filters
├── web
│   └── index.html
└── xmake.lua
```

