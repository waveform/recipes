#!/usr/bin/python3
# -*- coding: utf-8 -*-
import re
from io import StringIO
class template(object):
    indent_dec_keyword = re.compile(r'^(elif|else|except|finally)\s+')
    def __init__(self, tmpl):
        self.text = self.code = ''
        deep, indent = 0, ' ' * 4
        w = StringIO()
        for l in StringIO(tmpl):
            if l[0] != '$':
                w.write("{0}_output.append({1}.format(**locals()))\n".format(indent * deep, repr(l[:-1])))
                continue
            l = l[1:].strip()
            if l == 'end':
                deep -= 1
                w.write(indent * deep + '#end\n')
                continue
            if template.indent_dec_keyword.match(l): deep -= 1
            w.write(indent * deep + l + '\n')
            if l[-1] == ':': deep += 1
        self.code = w.getvalue()
    def render(self, **kwargs):
        kwargs["_output"] = []
        exec(self.code, {}, kwargs)
        self.text = '\n'.join(kwargs["_output"])+'\n'
        return self.text
