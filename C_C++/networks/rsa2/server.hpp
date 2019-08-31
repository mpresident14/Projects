#ifndef SERVER_HPP
#define SERVER_HPP 1

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/thread/sync_queue.hpp>

#include <queue>
#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>

using boost::multiprecision::cpp_int;

class Server {
    public:
        Server();
        ~Server() = default;
        Server(const Server& other) = delete;
        // void receiveMessage(ushort* encryptedMsg, size_t msgArrLen);
        // void printNextMessage();
        // RSAEncrypter& sendEncrypter();
        void stop();
        void recv_msg(const std::vector<cpp_int>& encrypted_msg);
        const cpp_int& get_public_key();
        const cpp_int& get_n();

    private:    
        void process_msgs();
        // bool isRunning;
        // RSADecrypter decrypter_;
        // std::mutex queueMutex_;
        // // std::mutex printMutex_;
        // std::condition_variable cv_;
       
        cpp_int e_; // public key
        cpp_int n_; // P * Q
        cpp_int d_; // private key
        boost::sync_queue<std::vector<cpp_int>> buffer_;
        std::atomic_bool is_stopped_;
        std::thread thr_;
};

#endif