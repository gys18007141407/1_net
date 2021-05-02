#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <atomic>
#include <iostream>
#include <thread>

std::atomic<bool> _async;

void threadFUNC(std::size_t fd){

    char buffer[2048];
    std::size_t readbytes = 1;

    while(readbytes && !_async.load(std::memory_order_acquire)){
        
        readbytes = read(fd, buffer, 2040);

        buffer[readbytes] = '\0';

        std::cout << "receive mesg: " << buffer << std::endl;
    }
}

int main(){

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if(fd <= 0) {
        std::cout << strerror(errno) << std::endl;
    }else{

        sockaddr_in addr;
        addr.sin_family  =AF_INET;
        addr.sin_port = htons(8888);
        
        if(-1 == inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) || -1 == connect(fd, (sockaddr*)&addr, sizeof(sockaddr))){
            std::cout << strerror(errno) << std::endl;
            close(fd);
        }else{

            std::cout << "connect success ... " << std::endl;

            _async.store(false, std::memory_order_seq_cst);

            std::thread readFUNC(threadFUNC, fd);

            std::string msg;
            std::size_t sendbytes;
            while(std::getline(std::cin, msg), msg != "quit"){

                sendbytes = write(fd, msg.c_str(), msg.size());

                if(sendbytes <= 0){
                    std::cout << strerror(errno) << std::endl;
                    break;
                }

            }

            close(fd);

            _async.store(true, std::memory_order_release);

            if(readFUNC.joinable()) readFUNC.join();
        }

    }

    return 0;

}