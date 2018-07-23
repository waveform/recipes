#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Author: Stanley Wang

class BitStream(object):
    ELEMENT_SIZE = 8 # 8, 16, 32
    def __init__(self, preload = None):
        self.data     = preload if preload else []
        self.wpos     = len(self.data)
        self.bit_wpos = 0
        self.rpos     = 0
        self.bit_rpos = 0

    def __len__(self):
        return self.get_write_pos()

    def get_write_pos(self):
        return self.wpos * BitStream.ELEMENT_SIZE + self.bit_wpos

    def get_read_pos(self):
        return self.rpos * BitStream.ELEMENT_SIZE + self.bit_rpos

    def write(self, value, bits):
        assert bits <= 32
        #assert value >= 0
        while bits > 0:
            if self.bit_wpos == 0: self.data.append(0)
            l = min(BitStream.ELEMENT_SIZE - self.bit_wpos, bits)
            m = (1 << l) - 1
            v = (value & m) << self.bit_wpos
            self.data[self.wpos] |= v
            self.bit_wpos += l
            if self.bit_wpos == BitStream.ELEMENT_SIZE:
                self.bit_wpos = 0
                self.wpos    += 1
            value >>= l
            bits -= l

    def read(self, bits):
        assert bits <= 32
        assert self.get_read_pos() + bits <= self.get_write_pos()
        value = 0
        bit_vpos = 0
        while bits > 0:
            l = min(BitStream.ELEMENT_SIZE - self.bit_rpos, bits)
            m = (1 << l) - 1
            v = (self.data[self.rpos] >> self.bit_rpos) & m
            # print('>>', self.rpos, self.bit_rpos, l, bin(v))
            self.bit_rpos += l
            if self.bit_rpos == BitStream.ELEMENT_SIZE:
                self.bit_rpos = 0
                self.rpos    += 1
            value |= (v << bit_vpos)
            bit_vpos += l
            bits -= l
        return value

    def overwrite(self, bpos, value, bits):
        assert bits <= 32
        assert value >= 0
        assert bpos + bits <= len(self)
        MASK = (1 << BitStream.ELEMENT_SIZE) - 1
        owpos     = bpos // BitStream.ELEMENT_SIZE
        bit_owpos = bpos % BitStream.ELEMENT_SIZE
        while bits > 0:
            v = self.data[owpos]
            l = min(BitStream.ELEMENT_SIZE - bit_owpos, bits)
            m = (1 << l) - 1
            v &= MASK ^ (m << bit_owpos)
            v |= (value & m) << bit_owpos
            self.data[owpos] = v
            bit_owpos += l
            bits -= l
            value >>= l
            if bit_owpos == BitStream.ELEMENT_SIZE:
                bit_owpos = 0
                owpos += 1

    def get_data(self):
        return self.data

if __name__ == '__main__':
    import random
    random.seed(0xbadbeef)
    bits   = []
    values = []
    for i in range(400000):
        bit = random.randint(1, 32)
        bits.append(bit)
        value = random.randint(1, 0xFFFFFFFF)
        value &= (1 << bit) - 1
        values.append(value)
    bs = BitStream()
    for b,v in zip(bits, values):
        assert len(bin(v)) - 2 <= b
        bs.write(v, b)
    readback = []
    for e, b in enumerate(bits):
        v = bs.read(b)
        #print (f'#{e}', f'b{b} \t\t', values[e], '==', v)
        #print('-'*20)
        assert values[e] == v
    bs_a = BitStream()
    bs_a.write(123456, 4)
    bs_b = BitStream()
    print(len(bs_b))
