/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-03 10:01:01
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 10:01:01
 */

#include "Buffer.h"

std::string Buffer::retrive_get(std::size_t _len){
    if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
    if(_len >= this->size()) return this->retrive_get_all();

    std::string res = std::string(this->data(), _len);
    this->offset(_len);

    return res;
}

std::string Buffer::retrive_get_all(){
    if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
    std::string total(this->data(), this->size());
    this->reset();
    return total;
}

std::string Buffer::to_string(std::size_t _len){
    if(m_thread_safe) std::lock_guard<std::mutex> g_lock(this->m_sync);
    if(_len >= this->size()) _len = this->size();
    std::string res = std::string(this->data(), _len);
    return res;
}
