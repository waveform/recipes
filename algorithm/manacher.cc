#include <string>
#include <vector>
#include <iostream>
using namespace std;

// return longest palindrome string
string manacher(const string& s) {
    int n = s.size();
    // build string from "CAAB" to "^#C#A#A#B#$"
    string st(n*2+1+2, '#');
    vector<int> len(n*2+1+2);
    st.front() = '^';
    st.back() = '$';
    for (int i=0;i<n;i++) st[2*i+2] = s[i];
    // [i, right) half open half close iternval
    int right = 0, mid = 0, longest_len = 0, longest_mid = 0;
    for (int i=1; i<st.size()-1; i++) {
        if (right > i) len[i] = min(right-i, len[2*mid-i]);
        else len[i] = 1;
        while (st[i+len[i]] == st[i-len[i]]) len[i]++;
        if (len[i]+i > right) {
            right = len[i]+i;
            mid = i;
        }
        if (len[i] > longest_len) {
            longest_len = len[i];
            longest_mid = i;
        }
    }
    // convert coord from st to t
    int start = (longest_mid - longest_len + 1 - 1) / 2;
    int length = longest_len - 1;
    return s.substr(start, length);
}

int main() {
    string src = "CAABAADAABCBAE";
    printf("%s\n", manacher(src).c_str());
    return 0;
}
