#include "server.hpp"
#include "rsa_utils.hpp"
#include "keygen.hpp"

#define P "35201546659608842026088328007565866231962578784643756647773109869245232364730066609837018108561065242031153677"
#define Q "499490918065850301921197603564081112780623690273420984342968690594064612108591217229304461006005170865294466527166368851"

using namespace std;

Server::Server()
{
   cpp_int p{P};
   cpp_int q{Q};
   n_ = p * q;
   array<cpp_int, 2> keys = generate_keys((p - 1) * (q - 1));
   d_ = move(keys[0]);
   e_ = move(keys[1]);
}

void Server::recv_msg(const vector<cpp_int>& encrypted_msg)
{
    cout << "Server received: " << decrypt_msg(encrypted_msg, d_, n_) << endl;;
}

const cpp_int& Server::get_public_key()
{
    return e_;
}

const cpp_int& Server::get_n()
{
    return n_;
}

// void Server::receiveMessage(ushort* encryptedMsg, size_t msgArrLen)
// {
//     // lock the mutex
//     unique_lock<mutex> lck(queueMutex_);
    
//     // Could optionally put a capacity on the queue here using cond_wait
//     q_.push(make_pair(encryptedMsg, msgArrLen));
   
//     cv_.notify_all();
//     // mutex unlocked upon destruction of unique_lock
// }

// void Server::printNextMessage()
// {
//     while (1) {   
//         // lock the mutex     
//         unique_lock<mutex> lck(queueMutex_);

//         // Wait for a message (unless the server has been stopped, then we just process the 
//         // messages until the queue is empty)

//         // The following is equivalent to:
//         // while (q_.empty() && isRunning) {
//         //    cv_.wait(lck);
//         // }
//         cv_.wait(lck, [this]()->bool {return !q_.empty() || !isRunning;} );
    
//         // This check must be here, not in the while statement, because this thread might be 
//         // waiting when we call stop();
//         if (q_.empty() && !isRunning) {
//             break;
//         }

//         pair<ushort*, size_t> nextPair = q_.front();
//         ushort* encryptedMsg = nextPair.first;
//         size_t msgArrLen = nextPair.second;
//         q_.pop();
//         // lck.unlock();

//         char* msg = decrypter_.decryptMessage(encryptedMsg, msgArrLen);

//         // unique_lock<mutex> printLck(printMutex_);
//         cout << "MESSAGE RECEIVED: " << msg << endl;
        
//         delete[] msg;
//         // mutex unlocked upon destruction of unique_lock
//     }   
// }

// RSAEncrypter& Server::sendEncrypter()
// {
//     return decrypter_.getEncrypter();
// }

// void Server::stop(vector<thread>& threads)
// {
//     // Wait for all user threads to put their message in the queue.
//     for (auto& thr : threads) {
//         thr.join();
//     } 

//     // Server may be stuck in conditional loop because queue is empty, so notify it and set 
//     // isRunning to false to break the loop.
//     cv_.notify_all();
//     isRunning = false;
//     printThread_.join();
// }
