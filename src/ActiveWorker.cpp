//
// Created by colin on 3/4/16.
//

#include "ActiveWorker.h"
#include <memory>
using namespace colinli::utility;

ActiveWorker::~ActiveWorker(){
    auto stop_token = [&]() {
        this->finish();
    };
    this->send(stop_token);
    thd.join();
}


std::unique_ptr<ActiveWorker> ActiveWorker::MakeActiveWorker(){
    std::unique_ptr<ActiveWorker> pActive (new ActiveWorker());
    pActive->thd = std::thread(&ActiveWorker::run, pActive.get());
    return pActive;
}

void ActiveWorker::run()
{
    while(!done) {
        Callback task;
        queue.WaitAndPop(task);
        task();
    }
}

void ActiveWorker::send(Callback task)
{
    this->queue.Push(task);
}