###服务端LoginSever
服务端使用了winsock2中select模型，最多同时接受63个连接。
###客户端QtClient
客户端使用Qt图形用户接口开发，只有三个控件，分别是对话区域，输入区域和发送按钮。也是用winsock2和C++11标准中的thread做网络和多线程编程，没有使用Qt提供的Socket工具和QThread.
