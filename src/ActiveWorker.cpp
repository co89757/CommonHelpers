//
// Created by colin on 3/4/16.
//

#include "ActiveWorker.h"
#include <memory>
#include <cstdio>
using namespace colinli::utility;

ActiveWorker::~ActiveWorker() {
  // printf("ActiveWorker destructor ... \n");
  auto stop_token = [&]() { this->finish(); }
  ;
  this->send(stop_token);
  thd.join();
  // printf("ActiveWorker bg thread joined, put to death\n");
}

std::unique_ptr<ActiveWorker> ActiveWorker::MakeActiveWorker() {
  std::unique_ptr<ActiveWorker> pActive(new ActiveWorker());
  pActive->thd = std::thread(&ActiveWorker::run, pActive.get());
  return pActive;
}

void ActiveWorker::run() {
  while (!done) {
    Callback task;
    queue.WaitAndPop(task);
    task();
  }
}

void ActiveWorker::send(Callback task) { this->queue.Push(task); }