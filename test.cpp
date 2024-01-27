#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <poll.h>

using namespace std;

int main(){
    string str = "ola ";
    string str2 = "mundo";

    vector<string> vec;
    string vec2;
    vec.push_back(str);
    vec.push_back(str2);

    vec2.append(str);
    vec2.push_back('1');
    vec2.push_back('1');

    cout << "+= | " << str + str2 << endl;
    for (auto i : vec){
        cout << "vec | " << i << endl;
    }


    cout << "vec2 | " << vec2 << endl;

}