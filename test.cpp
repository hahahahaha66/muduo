#include <iostream>
#include <string>
using namespace std;

int main() {
    int n = 0;
    string str;
    cin >> n;
    for (int i = 0; i < n; i++) {
        int size = 0;
        cin >> size;
        cin >> str;
        int count = 0;
        for(int j = 0; j < size; j++) {
            if (str[j] == '1') {
                count++;
            }
        }
        if (size == 1 && count == 1) {
            cout << 0 << endl;
        }
        else if (size == 1 && count == 0) {
            cout << 1 <<endl;
        }
        else{
            cout << (size - count) + (count * 2) << endl;
        }
    }
}
