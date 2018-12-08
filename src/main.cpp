#include <iostream>
#include "MD5.hpp"
using namespace std;

int main () {
    string input = "";
    string output = "";

    cout << "Input Your Message: " << endl;
    // 连空格也读进去，回车结束
    getline(cin, input);

    if (input == "") {
        cout << "null input error!" << endl;
        exit(1);
    }

    MD5 md5 = MD5();
    string Digest = md5.getMD5Digest(input);
    cout << "Your MD5 Digest:" << endl << Digest << endl;
    return 0;
}