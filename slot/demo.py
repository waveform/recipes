#!/usr/bin/python3
# -*- coding: utf-8 -*-

from slot import template

if __name__ == "__main__":
    class Country: pass

    Afghanistan = Country();
    Afghanistan.name = "Afghanistan"
    Afghanistan.cont = "Asia"
    Afghanistan.area = 652230
    Afghanistan.pop = 25500100
    Afghanistan.gdp = 20343000

    Albania = Country();
    Albania.name = "Albania"
    Albania.cont = "Europe"
    Albania.area = 28748
    Albania.pop = 2831741
    Albania.gdp = 12960000

    Algeria = Country();
    Algeria.name = "Algeria"
    Algeria.cont = "Africa"
    Algeria.area = 2381741
    Algeria.pop = 37100000
    Algeria.gdp = 188681000

    Andorra = Country();
    Andorra.name = "Andorra"
    Andorra.cont = "Europe"
    Andorra.area = 468
    Andorra.pop = 78115
    Andorra.gdp = 3712000

    Angola  = Country();
    Angola.name = "Angola"
    Angola.cont = "Africa"
    Angola.area = 1246700
    Angola.pop = 20609294
    Angola.gdp = 100990000

    tmpl_str = """\
    +----------------+--------------+-------------+--------------+-------------+
    | name           | continent    | area        | population   | gdp         |
    +----------------+--------------+-------------+--------------+-------------+
$for c in contries:
    | {c.name:<12}   | {c.cont:<12} | {c.area:<12}| {c.pop:<11}  | {c.gdp:<11} |
$end
    +----------------+--------------+-------------+--------------+-------------+
"""
    tmpl = template(tmpl_str)
    for lineno, line in enumerate(tmpl.code.split('\n')):
        print('#{0:03}: {1}'.format(lineno, line))
    s = tmpl.render(contries = [Afghanistan, Albania, Algeria, Andorra, Angola])
    print(tmpl.text, end='')
