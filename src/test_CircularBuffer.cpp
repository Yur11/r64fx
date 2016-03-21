#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "CircularBuffer.hpp"

using namespace std;
using namespace r64fx;

int main()
{
    {
        CircularBuffer<int> cb(4);

        for(int j=0; j<10; j++)
        {
            for(int i=1; i<=4; i++)
            {
                if(cb.write(&i, 1) != 1)
                {
                    cout << "Failed to write " << i << "!\n";
                    return 1;
                }
            }

            for(int i=5; i<8; i++)
            {
                if(cb.write(&i, 1) == 1)
                {
                    cout << "Fail! Should not have written " << i << "!\n";
                    return 1;
                }
            }

            for(int i=1; i<=4; i++)
            {
                int num = 0;
                if(cb.read(&num, 1) != 1)
                {
                    cout << "Failed to read " << i << "!\n";
                    return 1;
                }

                if(num != i)
                {
                    cout << "Fail! Should have read " << i << ", but got " << num << "!\n";
                    return 1;
                }
            }

            for(int i=1; i<=4; i++)
            {
                int num = 0;
                if(cb.read(&num, 1) != 0)
                {
                    cout << "Fail! Should not have read a value!\n";
                    return 1;
                }
            }

            {
                int buff[3] = {1, 2, 3};
                if(cb.write(buff, 3) != 3)
                {
                    cout << "Failed to write " << 3 << " items!\n";
                    return 1;
                }

                if(cb.write(buff, 3) != 1)
                {
                    cout << "Fail! Should have written only " << 1 << " item!\n";
                    return 1;
                }
            }

            {
                int buff[3] = {0, 0, 0};
                if(cb.read(buff, 2) != 2)
                {
                    cout << "Failed to read " << 2 << " items!\n";
                    return 1;
                }

                if(buff[0] != 1)
                {
                    cout << "Fail! buff[0] should have been " << 1 << "!\n";
                    return 1;
                }

                if(buff[1] != 2)
                {
                    cout << "Fail! buff[1] should have been " << 2 << "!\n";
                    return 1;
                }

                if(cb.read(buff, 3) != 2)
                {
                    cout << "Fail! Should have only read " << 2 << " items!\n";
                    return 1;
                }
            }
        }
        cout << "OK!\n";
    }

    {
        const int sleep_time = 600;
        CircularBuffer<int> cb(2);

        pthread_t thread1;
        if(pthread_create(&thread1, nullptr, [](void* arg) -> void* {
            auto cbp = (CircularBuffer<int>*) arg;
            for(int i=0; i<20; i++)
            {
                while(cbp->write(&i, 1) != 1)
                {
                    cout << "Failed to write " << i << "!\n";
                    usleep(sleep_time / 2);
                }
                usleep(sleep_time);
            }

            return nullptr;
        }, &cb) != 0)
        {
            cerr << "Failed to create a thread!";
            return 2;
        }
        usleep(sleep_time);

        for(int i=0; i<20; i++)
        {
            int num = -1;
            if(cb.read(&num, 1) != 1)
            {
                cout << "Failed to read " << i << "!\n";
            }
            else
            {
                cout << i << "\n";
            }

            usleep(sleep_time);
        }

        pthread_join(thread1, nullptr);
        cout << "Done!\n";
    }

    return 0;
}