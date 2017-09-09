#include <string>
#include <vector>
#include <iostream>
using namespace std;
int is_prefix(const string& pattern, int pos) {
    int m = pattern.size();
    int suffixlen = m - pos;
    for (int i=0; i < suffixlen; i++) {
        if (pattern[i] != pattern[pos+i]) return 0;
    }
    return 1;
}
int suffix_length(const string& pattern, int pos) {
    int m = pattern.size(), i = 0;
    while ((pattern[pos-i] == pattern[m-1-i]) && (i<pos)) ++i;
    return i;
}
vector<int> pre_process_good_suffix(const string& pattern) {
    int m = pattern.size();
    vector<int> good_suffix(m);
    // first loop
    int last_prefix_index = m - 1;
    for (int i = m-1; i>=0; i--) {
        if (is_prefix(pattern, i+1)) last_prefix_index = i+1;
        good_suffix[i] = last_prefix_index + (m - 1 - i);
    }
    // second loop
    for (int i=0; i < m-1; i++) {
        int slen = suffix_length(pattern, i);
        if (pattern[i - slen] != pattern[m - 1 - slen]) good_suffix[m - 1 - slen] = m - 1 - i + slen;
    }
    return good_suffix;
}
vector<int> pre_process_bad_char(const string& pattern) {
    int m = pattern.size();
    vector<int> bad_char(256, m);
    for (int i=0;i<m-1;i++) bad_char[pattern[i]] = m - 1 - i;
    return bad_char;
}
int find(const string& text, const string& pattern) {
    if (pattern.empty()) return 0;
    vector<int> bad_char = pre_process_bad_char(pattern);
    vector<int> good_suffix = pre_process_good_suffix(pattern);
    int n = text.size(), m = pattern.size(), i = m - 1;
    while(i < n) {
        int j = m - 1;
        while (j >= 0 && (text[i] == pattern[j])) { --i; --j; }
        if (j < 0) return i+1;
        i += max(bad_char[text[i]], good_suffix[j]);
    }
    return -1;
}

int main() {
    string text = "abbadcababacab";
    string pattern = "babac";
    int pos = find(text, pattern);
    printf("%d\n", pos);
    return 0;
}
