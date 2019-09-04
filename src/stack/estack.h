
#ifndef E_ESTACK
#define E_ESTACK
#include "../types.h"
namespace STACK
{
class Stack
{
  public:
    int getMax() { return max_stack; }
    Stack() {}
    Stack(int max_stack);
    ~Stack();
    int getSp()
    {
        return sp;
    };
    intdv Pop();
    void Push(intdv);

  private:
    /* data */
    int max_stack;
    intdv *stack;
    int sp = 0;
};
} // namespace STACK

#endif