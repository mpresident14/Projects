#include "server.hpp"

using namespace std;

Server::Server()
    : isRunning{true}, printThread_{&Server::printNextMessage, this}
{
   
}

void Server::receiveMessage(ushort* encryptedMsg, size_t msgArrLen)
{
    // lock the mutex
    unique_lock<mutex> lck(queueMutex_);
    
    // Could optionally put a capacity on the queue here using cond_wait
    q_.push(make_pair(encryptedMsg, msgArrLen));
   
    cv_.notify_all();
    // mutex unlocked upon destruction of unique_lock
}

void Server::printNextMessage()
{
    while (1) {   
        // lock the mutex     
        unique_lock<mutex> lck(queueMutex_);

        // Wait for a message (unless the server has been stopped, then we just process the 
        // messages until the queue is empty)

        // The following is equivalent to:
        // while (q_.empty() && isRunning) {
        //    cv_.wait(lck);
        // }
        cv_.wait(lck, [this]()->bool {return !q_.empty() || !isRunning;} );
    
        // This check must be here, not in the while statement, because this thread might be 
        // waiting when we call stop();
        if (q_.empty() && !isRunning) {
            break;
        }

        pair<ushort*, size_t> nextPair = q_.front();
        ushort* encryptedMsg = nextPair.first;
        size_t msgArrLen = nextPair.second;
        q_.pop();
        // lck.unlock();

        char* msg = decrypter_.decryptMessage(encryptedMsg, msgArrLen);

        // unique_lock<mutex> printLck(printMutex_);
        cout << "MESSAGE RECEIVED: " << msg << endl;
        
        delete[] msg;
        // mutex unlocked upon destruction of unique_lock
    }   
}

RSAEncrypter& Server::sendEncrypter()
{
    return decrypter_.getEncrypter();
}

void Server::stop(vector<thread>& threads)
{
    // Wait for all user threads to put their message in the queue.
    for (auto& thr : threads) {
        thr.join();
    } 

    // Server may be stuck in conditional loop because queue is empty, so notify it and set 
    // isRunning to false to break the loop.
    cv_.notify_all();
    isRunning = false;
    printThread_.join();
}
