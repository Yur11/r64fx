#include "StringUtils.hpp"
#include <iostream>

using namespace std;
using namespace r64fx;


void test(string::iterator &it, const string::iterator &end_it, const std::string &expected)
{
    auto path = next_file_path_from_uri_list(it, end_it);
    if(path != expected)
    {
        cout << "Expected:\n" << expected << "\nGot:\n" << path;
        cout << "Failed!\n";
        exit(1);
    }
}


int main()
{
    {
        string uri_list =
            "#This is a comment\r\n"
            "file:///home/user/sausage/files/cat/and_pigs\r\n"
            "#This is another comment\r\n"
            "file:///bla/bla/good\r\n"
        ;

        auto it = uri_list.begin();

        test(it, uri_list.end(), "/home/user/sausage/files/cat/and_pigs");
        test(it, uri_list.end(), "/bla/bla/good");
        test(it, uri_list.end(), "");
    }

    {
        string uri_list =
            "#This is a comment\n"
            "file:///home/user/sausage/files/cat/and_pigs\r\n"
            "#This is another comment\n"
            "file:///bla/bla/good\n"
        ;

        auto it = uri_list.begin();

        test(it, uri_list.end(), "/home/user/sausage/files/cat/and_pigs");
        test(it, uri_list.end(), "/bla/bla/good");
        test(it, uri_list.end(), "");
    }

    {
        string uri_list =
            "#This is a comment\r\n"
            "file:///home/user/sausage/files/cat/and_pigs\r\n"
            "#This is another comment\r\n"
            "bla:///bla/bla/good\r\n"
        ;

        auto it = uri_list.begin();

        test(it, uri_list.end(), "/home/user/sausage/files/cat/and_pigs");
        test(it, uri_list.end(), "");
    }

    cout << "OK\n";
    return 0;
}