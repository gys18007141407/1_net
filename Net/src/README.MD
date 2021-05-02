To use this need 3 steps:

first, include header file "src/IServer.h" then construct a class derived from IServer.h

    #include "src/IServer.h"

    class myServer: public IServer{

    private:
        TcpServer* _server;

    }

second, override 4 virtual functions, these functions do what you want to do when :a client Connect, receive new Message, send buffer has Completely sent to client , client Close

    void OnConnect(TcpConn*);
    void OnMessage(TcpConn*);
    void OnComplete(TcpConn*);
    void Onclose(TcpConn*);

third, write your run function such as follows

    void run(){

        _server = new TcpServer();
        _server->setIP("127.0.0.3");
        _server->setPort(12345);

        // or use other version
        //    explicit TcpServer();
        //    explicit TcpServer(const char* IP, const std::size_t port);
        //    explicit TcpServer(const std::string IP, const std::size_t port);
        //    explicit TcpServer(const std::size_t threads);
        //    explicit TcpServer(const char* IP, const std::size_t port, const std::size_t threads);
        //    explicit TcpServer(const std::string IP, const std::size_t port, const std::size_t threads);

        _server = new TcpServer("127.0.0.3", 12345);

        _server->setServer(this);
        _server->start();
    }


extra setting in Define.h:

    #define LISTENQUEUE 10
    #define MAXLOOPTIME 5
    #define DETAIL_LOG
    #define TIMEOUT (1000*60)
