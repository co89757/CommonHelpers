//
// Created by colin on 3/4/16.
//

#ifndef PROJECT_ACTIVEWORKER_H
#define PROJECT_ACTIVEWORKER_H

#include <thread>
#include <memory>
#include <functional>
#include "BlockQueue.h"

namespace colinli {
namespace utility {
typedef std::function<void()> Callback;

class ActiveWorker {
 private:
  std::thread thd;
  ActiveWorker() {}
  ActiveWorker(const ActiveWorker&) = delete;
  ActiveWorker& operator=(const ActiveWorker&) = delete;
  bool done = false;
  void run();
  // token action to finish the work
  void finish() { done = true; }
  BlockQueue<Callback> queue;

 public:
  virtual ~ActiveWorker();
  void send(Callback task);

  static std::unique_ptr<ActiveWorker> MakeActiveWorker();

};

}
}

#endif  //PROJECT_ACTIVEWORKER_H
