#ifndef SERVER_HPP
#define SERVER_HPP 1

#include "rsa_decrypter.hpp"
#include <queue>
#include <vector>
#include <thread>
#include <condition_variable>

class Server {
    public:
        Server();
        ~Server() = default;
        Server(const Server& other) = default;
        void receiveMessage(ushort* encryptedMsg, size_t msgArrLen);
        void printNextMessage();
        RSAEncrypter& sendEncrypter();
        void stop(std::vector<std::thread>& threads);

    private:    
        bool isRunning;
        RSADecrypter decrypter_;
        std::queue<std::pair<ushort*, size_t>> q_; // Each pair maps msg to length of array
        std::mutex queueMutex_;
        // std::mutex printMutex_;
        std::condition_variable cv_;
        std::thread printThread_;
};

#endif