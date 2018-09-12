
# To create a binary string in lua for using wrk to test mrpacker and mrhttp
def pbuf( b ):
  s = "string.char("
  for c in b:
    s += hex(c) + ", "
  print(s)

import mrpacker
o = { "name":"mrpacker", "awesome?":"yes" }
b = mrpacker.pack( o )
print( mrpacker.unpack(b) )

#print(b)
#f = open("test.mrp","wb")
#f.write(b)
#f.close()
#pbuf(b)
#print(len(b))

