#include <iostream>
#include <string>
#include "LinkedList.hpp"

using namespace std;
using namespace r64fx;

struct Item : public LinkedList<Item>::Node{
    string str;

    Item(string str) : str(str) {}
};

bool sucess = true;

void test(int num, LinkedList<Item> items, string expected)
{
    cout << "Test" << num << "\n";

    string obtained;
    for(Item* item : items )
    {
        obtained += item->str;
    }

    bool result = ( obtained == expected );
    cout << "    expected: \"" << expected << "\"\n";
    cout << "    obtained: \"" << obtained << "\"\n";
    cout << (result ? "OK" : "FAIL") << "\n\n";

    sucess = sucess && result;
}


int main()
{
    Item item1("A");
    Item item2("B");
    Item item3("C");
    Item item4("D");

    LinkedList<Item> items;
    test(1, items, "");

    items.append(&item1);
    items.append(&item2);
    items.append(&item3);
    items.append(&item4);
    test(2, items, "ABCD");

    items.remove(&item1);
    test(3, items, "BCD");

    items.append(&item1);
    test(4, items, "BCDA");

    items.clear();
    test(5, items, "");

    items.append(&item1);
    items.append(&item2);
    items.append(&item3);
    items.append(&item4);
    test(6, items, "ABCD");

    items.remove(&item2);
    test(7, items, "ACD");

    items.remove(&item4);
    test(8, items, "AC");

    items.insertAfter(&item1, &item4);
    test(9, items, "ADC");

    items.insertAfter(&item3, &item2);
    test(10, items, "ADCB");

    items.remove(&item4);
    test(11, items, "ACB");

    items.insertBefore(&item3, &item4);
    test(12, items, "ADCB");

    items.remove(&item4);
    test(13, items, "ACB");

    items.preppend(&item4);
    test(14, items, "DACB");

    return (sucess ? 0 : 1);
}
