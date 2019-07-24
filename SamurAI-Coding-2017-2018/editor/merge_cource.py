#!/usr/bin/env python
import json
import sys
results = {}
isFirst = True
for fn in sys.argv[1:]:
    with open(fn) as f:
        js = json.load(f)
        if isFirst:
            results = js
            isFirst = False
            continue
        offset = results['length']
        results['stepLimit'] += js['stepLimit']
        results['thinkTime'] += js['thinkTime']
        results['length'] += js['length']
        results['vision'] = max(results['vision'], js['vision'])
        if results['width'] != js['width']:
            raise 'cannot merge courses whose widths are different'
        for o in js['obstacles']:
            newo = [{'y' : p['y'] + offset, 'x' : p['x']} for p in o]
            results['obstacles'].append(newo)
print(json.dumps(results, separators=(',', ':')))
