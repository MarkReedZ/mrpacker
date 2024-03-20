

# To create a binary string in lua for using wrk to test mrpacker and mrhttp
def pbuf( b ):
  s = "string.char("
  for c in b:
    s += hex(c) + ", "
  print(s)

import mrpacker
import time
import ujson, mrjson


n = 0x100000000000
n = 99999999999999999999999
b = mrpacker.pack(n)
for c in b:
  print(hex(c))
print(mrpacker.unpack(b))
exit()

#f = open("../mrjson/bench/json/twitter.json","r")
#s = f.read()
#f.close()
#o = mrjson.loads(s)

#print(o)

#o = [True]*5000
o = {"name":"MalthusianProphet","tl":2004,"dankmemes":True,"list":[1,2,3,4,5,6]}
#o = {"name":55,"tl":2004,"dankmemes":True,"list":[1,2,3,4,5,6]}
#o = [ "fadsfds", 213 , 123, 1, 2, 3, "lists are cool", [1,2,3,] ]

for x in range(1):
  if 0:
    #st = time.time()
    b = mrp.pack( o )
    #print( "took ", time.time()-st)
    #st = time.time()
    #o = mrp.unpack( b )
    #print( "took ", time.time()-st)
    #exit(1)
  
  if 0:
    #st = time.time()
    b = mrjson.dumps( o )
    #print( "took ", time.time()-st)
    #st = time.time()
    #o = mrjson.loads( b )
    #print( "took ", time.time()-st)
  
  if 0:
    #print(b)
    print("ujson:")
    st = time.time()
    b = ujson.dumps( o )
    print( "took ", time.time()-st)
    st = time.time()
    o = ujson.loads( b )
    print( "took ", time.time()-st)
    #print(b)
  
