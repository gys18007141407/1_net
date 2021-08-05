/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-03 11:30:41
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 11:30:41
 */

#include "Helper.h"

std::size_t get_thread_id(){
    std::thread::id _ = std::this_thread::get_id();
    return *reinterpret_cast<__gthread_t*>(&_);
}

void setFdReuse(std::size_t fd){
    std::size_t reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(std::size_t));  //param2: SOL_SOCKET、IPPROTO_TCP、IPPROTO_IP、IPPROTO_IPV6。
}