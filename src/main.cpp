#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>


class TaskQueue{
    private:
        std::queue<int> queue;
        std::mutex mtx;
        std::condition_variable cv;
    public:
        void push(int task){
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(task);
            cv.notify_one();
        }
        int pop(){
            std::unique_lock <std::mutex> lock(mtx);

            while(queue.empty()){
                cv.wait(lock);
            }

            int ntask = queue.front();
            queue.pop();
            return ntask;
        }   

};

std::mutex coutMutex;

void worker(int id, TaskQueue& q){
    while(true){
        int task = q.pop();

        if (task == -1) break; 

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[Worker-" << id <<"] обработал задачу " << task << std::endl;

    }
}


int main(){
    TaskQueue q;
    int numWorkers = 3;
    std::vector<std::thread> workers;

    for(int i = 0; i<numWorkers; i++){
        workers.push_back(std::thread(worker, i+1, std::ref(q)));
    }

    for (int i = 1; i <= 20; i++){
        q.push(i);
    }

    for(int i = 0; i < numWorkers; i++){
        q.push(-1);
    }

    for (auto& t:workers){
        t.join();
    }
}