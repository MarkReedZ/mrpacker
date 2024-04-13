# coding=UTF-8

import mrpacker as j
import json
import inspect

def raises( o, f, exc,details ):
  try:
    f(o)
    cf = inspect.currentframe()
    print("ERROR line",cf.f_back.f_lineno,"didn't raise exception",str(exc))
  except Exception as e:
    cf = inspect.currentframe()
    if type(e) != exc:
      print("ERROR line",cf.f_back.f_lineno,"rose the wrong exception",str(e))
    if str(e) != details:
      print("ERROR line",cf.f_back.f_lineno,"raised an exception with the wrong details\nAct: ",str(e),"\nExp: ",details)

def eq( a, b ):
  if a != b:
    cf = inspect.currentframe()
    print( "ERROR Line", cf.f_back.f_lineno, a, "!=", b )
    return -1
  return 0



# Pack and unpack a bunch of objects

print("Running tests...")

objs = [
[float("inf"),2],
[1,float("-inf"),2],
-1, 0, 1,255,256,
-132123123123,
1.002, -1.31,
-312312312312.31,
2**31, 2**32, 2**32-1,
1337E40, 1.337E40, 3937e+43, 7e+3, 
7e-3, 1e-1, 1e-4,
[1, 2, 3, 4],
[1, 2, 3, [4,5,6,7,8]],
{"k1": 1, "k2": 2, "k3": 3, "k4": 4},
'\u273f\u2661\u273f',  # ✿♡✿
{},
"\x00", "\x19", 
"afsd \x00 fdasf",
"\xe6\x97\xa5\xd1\x88",
"\xf0\x90\x8d\x86",
"\xf3\xbf\xbf\xbffadfadsfas",
"fadsfasd\xe6\x97\xa5\xd1\x88",
[[[[]]]] * 20,
[31337.31337, 31337.31337, 31337.31337, 31337.31337] * 10,
{'a': -12345678901234.56789012},
"fadfasd \\ / \r \t \n \b \fxx fadsfas",
"这是unicode吧你喜欢吗？我的中文最漂亮",
["别","停","believing", -1,True,"zero",False, 1.0],
None, True,False,float('inf'),
[18446744073709551615, 18446744073709551615, 18446744073709551615],
{ "A":[{},[],{}], "b": { "1":1,"2":2,"3":3 } },
]
o = {}
for x in range(100000):
  o[x] = x
objs.append(o)

# NaN != Nan so check str rep
o = j.unpack( j.pack( float('nan') ) )
eq( str(o), 'nan' )


for o in objs:
  try:
    eq( o, j.unpack(j.pack(o)) )
  except Exception as e:
    print( "ERROR",str(e), o )

from os import walk
for (dirpath, dirnames, filenames) in walk("test_data"):
  for fn in filenames:
    f = open( "test_data/"+fn,"rb")
    o = json.load(f)
    try:
      eq( o, j.unpack(j.pack(o)) )
    except Exception as e:
      print( "ERROR",str(e), o )



print("Testing Exceptions..")
raises( 99999999999999999999999999999999, j.pack, OverflowError, "The number is out of range for a long long" )
raises( -99999999999999999999999999999999, j.pack, OverflowError, "The number is out of range for a long long" )


print("Done")






