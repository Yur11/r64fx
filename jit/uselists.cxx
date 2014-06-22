#include <iostream>
#include <iomanip>
#include "jit.h"
#include "shared_sources/LinkedItem.h"

using namespace std;
using namespace r64fx;

typedef void (*Fun)(void);


struct Item : public LinkedItem<Item>{
    long int num;
    
    Item(long int num) : num(num) {}
};

const int buffer_size = 4;
long int buffer[buffer_size];


/** @brief Insert b after a. 
 
    Broken!
 */
void insert_item(Assembler &as, GPR64 a_addr, GPR64 b_addr)
{
    Disp8 prev(0);
    Disp8 next(8);
    
    Base a(a_addr);
    Base b(b_addr);
    
    /* rax = a.next */
    as.mov(rax, a, next);
    
    /* a.next = b */
    as.mov(a, next, b_addr);
    
    /* b.prev = a */
    as.mov(b, prev, a_addr);
    
    /* rax.prev = b */
    as.mov(Base(rax), prev, b_addr);
}


int main()
{
    Item* a = new Item(1);
    Item* b = new Item(20);
    Item* c = new Item(300);
    Item* d = new Item(4000);
    Item* e = new Item(50000);
    
    a->insert(b);
    b->insert(c);
    c->insert(d);
    
    CodeBuffer cb;
    Assembler as(cb);

    as.mov(rax, Imm64(a));
    as.mov(rcx, Imm64(e));
    insert_item(as, rax, rcx);
    
    as.mov(rax, 0);
    
    as.mov(rbx, Imm64(a));
    auto mark = Mem8(as.ip());
        as.add(rax, Base(rbx), Disp8(16));
        as.mov(rbx, Base(rbx), Disp8(8));
        as.cmp(rbx, 0);
        as.jne(mark);
        
    as.mov(Mem64(buffer), rax);
    
    as.ret();
    
    Fun fun = (Fun) as.getFun();
    
    cout << "---------\n";
    
    fun();

    for(int i=0; i<buffer_size; i++)
    {
        cout << buffer[i] << "\n";
    }
    
    return 0;
}