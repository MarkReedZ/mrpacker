import msgpack

def pbuf( b ):
  s = "string.char("
  for c in b:
    s += hex(c) + ", "
  print(s)
import mrpacker
o = { "name":"MalthusianProphet", "tl":2004, "dankmemes":True, "list":[1,2,3,4,5,6] }
#print(len(o))
b = msgpack.packb( o )
print(b)
f = open("test.mrp","wb")
f.write(b)
f.close()
pbuf(b)
#print(len(b))
print( mrpacker.unpack(b) )

