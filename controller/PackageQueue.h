/**
   @file PackageQueue.h

   Class declaration for Packages Queues
*/


#ifndef PackageQueue_h
#define PackageQueue_h

#include "Package.h"
#define PACKAGE_QUEUE_SIZE 15


//#define NULL nullptr



class PackageQueue {
  private:
    Package queue[PACKAGE_QUEUE_SIZE];
    int front, rear;
  public:

    PackageQueue() {
      front = -1;
      rear = -1;
    }

    void clear(){
      front = -1;
      rear = -1;
    }

    bool isFull() {
      if (front == 0 && rear == PACKAGE_QUEUE_SIZE - 1)
        return true;
      else
        return false;
    }

    Package* deQueue() {
      //Package *package;
      char frontold = front;
      if (!isEmpty()) {
        //*package = queue[front];
        if (front >= rear) {
          front = -1;
          rear = -1;
        } else {
          front++;
        }
        return &(queue[frontold]);
      } else {
        return NULL;
      }
    }

    Package* getFront() {
      if (!isEmpty())
        return &(queue[front]);
      else
        return NULL;
    }

    bool isEmpty() {
      if (front == -1)
        return true;
      else
        return false;
    }

    bool enQueue(Package * pac) {
      
      if (!isFull()) {
        if (front == -1) front = 0;
        rear++;
        queue[rear] = *pac;
        return true;
      } else
        return false;
    }



};

#endif
