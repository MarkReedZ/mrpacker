
# To create a binary string in lua for using wrk to test mrpacker and mrhttp
def pbuf( b ):
  s = "string.char("
  for c in b:
    s += hex(c) + ", "
  print(s)

import mrpacker as mrp
import time
#import ujson, mrjson

o = 23
print( o == mrp.unpack(mrp.pack(o)) )
exit()

f = open("../mrjson/bench/json/twitter.json","r")
s = f.read()
f.close()
o = mrjson.loads(s)

#print(o)

#o = [True]*5000
o = {"name":"MalthusianProphet","tl":2004,"dankmemes":True,"list":[1,2,3,4,5,6]}
#o = {"name":55,"tl":2004,"dankmemes":True,"list":[1,2,3,4,5,6]}
o = [ "fadsfds", 213 , 123, 1, 2, 3, "lists are cool", [1,2,3,] ]

st = time.time()
b = mrp.pack( o )
print( "took ", time.time()-st)
st = time.time()
o = mrp.unpack( b )
print( "took ", time.time()-st)
exit(1)

print("mrjson:")
st = time.time()
b = mrjson.dumps( o )
print( "took ", time.time()-st)
st = time.time()
o = mrjson.loads( b )
print( "took ", time.time()-st)
#print(b)
print("ujson:")
st = time.time()
b = ujson.dumps( o )
print( "took ", time.time()-st)
st = time.time()
o = ujson.loads( b )
print( "took ", time.time()-st)
#print(b)


#print(b)
#f = open("test.mrp","wb")
#f.write(b)
#f.close()
#pbuf(b)
#print(len(b))

