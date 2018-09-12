import timeit, mrpacker, msgpack, mrjson

setup = u'''
import mrpacker,msgpack,mrjson, zstd
o = {'tl': [206187574518812,"dsdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"], 'd': {206187574518812: {'p': 0, 'txt': 'Masses elements hitch from parenthesis from. Decorations agree banks congress one four aptitudes. Owners pay.', 'c': 206187574518812, 'cs': [], 'au': '1287', 'up': 1, 'dn': 0, 'rtg': 1, 'ago': '1 hour ago', 'num': 0, 'tp': 206187574518812}}}
#o = { "name":"MalthusianProphet", "tl":2004, "dankmemes":True, "list":[1,2,3,4,5,6] }

bm = msgpack.packb(o)
bp = mrpacker.pack(o)
bj = mrjson.dumps(o)
'''

o = {'tl': [206187574518812], 'd': {206187574518812: {'p': 0, 'txt': 'Masses elements hitch from parenthesis from. Decorations agree banks congress one four aptitudes. Owners pay.', 'c': 206187574518812, 'cs': [], 'au': '1287', 'up': 1, 'dn': 0, 'rtg': 1, 'ago': '1 hour ago', 'num': 0, 'tp': 206187574518812}}}
o = {'tl': [206187574518812,"dsdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"], 'd': {206187574518812: {'p': 0, 'txt': 'Masses elements hitch from parenthesis from. Decorations agree banks congress one four aptitudes. Owners pay.', 'c': 206187574518812, 'cs': [], 'au': '1287', 'up': 1, 'dn': 0, 'rtg': 1, 'ago': '1 hour ago', 'num': 0, 'tp': 206187574518812}}}
#o = { "name":"MalthusianProphet", "tl":2004, "dankmemes":True, "list":[1,2,3,4,5,6] }


print ("  ",(min(timeit.Timer('mrpacker.pack(o)',setup=setup).repeat(100, 1))))
print ("  ",(min(timeit.Timer('mrpacker.unpack(bp)',setup=setup).repeat(100, 1))))
print ("  ",(min(timeit.Timer('msgpack.packb(o)',setup=setup).repeat(100, 1))))
print ("  ",(min(timeit.Timer('msgpack.unpackb(bm, encoding = "utf-8")',setup=setup).repeat(100, 1))))
print ("  ",(min(timeit.Timer('mrjson.dumps(o)',setup=setup).repeat(100, 1))))
print ("  ",(min(timeit.Timer('mrjson.loads(bj)',setup=setup).repeat(100, 1))))

if o != mrpacker.unpack( mrpacker.pack(o) ):
  print("not equal")

print (len( mrpacker.pack(o) ))
print (len( msgpack.packb(o) ))
print (len( mrjson.dumps(o) ))

import zstd
print (len( zstd.compress(mrpacker.pack(o)) ))
print (len( zstd.compress(msgpack.packb(o)) ))
print (len( zstd.compress(mrjson.dumpb(o)) ))
print ("  ",(min(timeit.Timer('zstd.compress(mrpacker.pack(o))',setup=setup).repeat(100, 1))))
print ("  ",(min(timeit.Timer('zstd.compress(msgpack.packb(o))',setup=setup).repeat(100, 1))))
print ("  ",(min(timeit.Timer('zstd.compress(mrjson.dumpb(o))',setup=setup).repeat(100, 1))))


o = { "list":["199999988888888888888888999999999999999999999999999999999999999999",1,2,3], "a":1,"b":2 }
o = { "list":9, "a":1,"b":2 }
