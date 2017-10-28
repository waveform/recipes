#include <cstdint>
#include <cstdio>
#include <cassert>
#include <vector>
#include <bitset>
using namespace std;

//TODO: 1. conv to stream style
//      2. support variable m

const uint32_t m = 8, log_m = 3;
const uint32_t char_size_in_bits = 8;
void encoder(vector<uint8_t>& input,  uint32_t in_len_in_bits,
        vector<uint8_t>& output, uint32_t& out_len_in_bits){
    out_len_in_bits = 0;
    uint32_t pos = 0;
    bitset<64> bs;
    for (uint8_t n : input) {
        uint32_t q = n / m, r = n & (m-1);
        for (uint32_t i = 0; i < q; i++) bs.set(pos++, true);
        bs.set(pos++, false);
        for (uint32_t i = 0; i < log_m; i++) {
            bs.set(pos++, r & 0x1);
            r >>= 1;
        }
        while (pos >= char_size_in_bits) {
            uint64_t t = bs.to_ullong();
            bs >>= char_size_in_bits;
            pos -= char_size_in_bits;
            out_len_in_bits += char_size_in_bits;
            output.push_back(t & 0xFF);
        }
    }
    if (pos) {
        out_len_in_bits += pos;
        assert(pos < char_size_in_bits);
        output.push_back(bs.to_ulong() & 0xFF);
    }
}

void decoder(vector<uint8_t>& input,  uint32_t in_len_in_bits,
        vector<uint8_t>& output, uint32_t& out_len_in_bits){
    out_len_in_bits = 0;
    uint32_t b = 0, pos = 0, idx = 0, mode = 1;
    uint32_t q = 0, r = 0;
    bitset<64> bs;
    while(b < in_len_in_bits) {
        if (mode) { // get q, try to get length of 1 bits
            q = 0;
            bool e = true;
            do {
                if (!pos) {
                    bs |= input[idx++];
                    pos += char_size_in_bits;
                }
                e = bs[0];
                if (e) ++q;
                bs >>= 1;
                --pos;
            } while(e);
            b += q+1;
        } else { // get r
            if (pos < log_m) {
                uint32_t v = input[idx++];
                bs |= (v << pos);
                pos += char_size_in_bits;
            }
            r = bs.to_ullong() & (m-1);
            bs >>= log_m;
            pos -= log_m;
            output.push_back( q<<log_m | r );
            b += log_m;
        }
        mode = 1 - mode;
    }
    out_len_in_bits = output.size() * char_size_in_bits;
}

void show(vector<uint8_t>& data, uint32_t len_in_bits) {
    assert(data.size() * char_size_in_bits >= len_in_bits);
    printf("[%u] ", len_in_bits);
    uint32_t i = 0;
    for (; i < len_in_bits; i+=8) printf("%02x ", data[i>>3]);
    printf("\n");
}

int main() {
    vector<uint8_t> plain;
    for (uint32_t i=0; i<256; i++) plain.push_back(i);
    uint32_t plain_len_in_bits = 8*plain.size();
    show(plain, plain_len_in_bits);

    vector<uint8_t> cipher;
    uint32_t cipher_len_in_bits = 0;
    encoder(plain, plain_len_in_bits, cipher, cipher_len_in_bits);
    show(cipher, cipher_len_in_bits);

    vector<uint8_t> text;
    uint32_t text_len_in_bits = 0;
    decoder(cipher, cipher_len_in_bits, text, text_len_in_bits);
    show(text, text_len_in_bits);

    assert(text_len_in_bits == plain_len_in_bits);
    assert(plain.size() == text.size());
    for (int i=0;i<plain.size();i++) assert(plain[i] == text[i]);
    return 0;
}
