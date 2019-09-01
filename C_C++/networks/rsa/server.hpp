#ifndef SERVER_HPP
#define SERVER_HPP 1

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/thread/sync_queue.hpp>

#include <vector>
#include <thread>

using boost::multiprecision::cpp_int;

class Server {
    public:
        Server();
        ~Server() = default;
        Server(const Server& other) = delete;
        void stop();
        void recv_msg(const std::vector<cpp_int>& encrypted_msg);
        void recv_msg(std::vector<cpp_int>&& encrypted_msg);
        const cpp_int& get_public_key();
        const cpp_int& get_n();

    private:    
        void process_msgs();

        cpp_int e_; // public key
        cpp_int n_; // P * Q
        cpp_int d_; // private key
        boost::sync_queue<std::vector<cpp_int>> buffer_;
        bool is_stopped_;
        std::thread thr_;
};

#endif