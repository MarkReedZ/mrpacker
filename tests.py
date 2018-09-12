# coding=UTF-8

import mrpacker as j
import inspect

def raises( o, f, exc,details ):
  try:
    f(o)
    if len(o) > 100: o = o[:100]
    print("ERROR ", o, " didn't raise exception")
  except Exception as e:
    if len(o) > 100: o = o[:100]
    if type(e) != exc:
      print("ERROR",o," rose wrong exception",type(e),e)
    if str(e) != details:
      print("ERROR",o," rose wrong exception details actual vs expected:\n",e,"\n",details)

def eq( a, b ):
  if a != b:
    cf = inspect.currentframe()
    print( "ERROR Line", cf.f_back.f_lineno, a, "!=", b )
    return -1
  return 0


print("Running tests...")

objs = [
[float("inf"),2],
[1,float("-inf"),2],
-1, 12, 1,
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
]

# NaN != Nan so check str rep
o = j.unpack( j.pack( float('nan') ) )
eq( str(o), 'nan' )


for o in objs:
  try:
    eq( o, j.unpack(j.pack(o)) )
  except Exception as e:
    print( "ERROR",str(e), o )


# TODO
#print("Testing Exceptions..")
#raises( "NaNd",         j.loads, ValueError, "Expecting 'NaN' at pos 0" )


print("Done")
