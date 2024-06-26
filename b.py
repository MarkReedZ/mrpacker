import timeit, mrpacker, mrjson
import msgpack

setup = """
import mrpacker,mrjson,msgpack
o = {'tl': [3376686588804032, 3467362509744518, 3467383293555110, 3467470637563061, 3467478106854362, 3467478107397231, 3467500195766276, 3535631493306510], 'd': {3376686588804032: {'p': 0, 'txt': 'test', 'c': 3376686588804032, 'cs': [3376907122926878, 3467281916293896], 'au': 'Mark', 'rtg': 1, 'ts': 1537224158, 'num': 3, 'tp': 3376686588804032}, 3376907122926878: {'p': 3376686588804032, 'txt': 'Well now\\n', 'c': 3376907122926878, 'cs': [3467275147554700], 'au': 'Mark', 'rtg': 1, 'ts': 1537224368, 'tp': 3376686588804032}, 3467275147554700: {'p': 3376907122926878, 'txt': '<p>new comment</p>\\n', 'c': 3467275147554700, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537310550, 'tp': 3376686588804032}, 3467281916293896: {'p': 3376686588804032, 'txt': '<p>another comment</p>\\n', 'c': 3467281916293896, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537310556, 'tp': 3376686588804032}, 3467362509744518: {'p': 0, 'txt': '<p>ttest</p>\\n', 'c': 3467362509744518, 'cs': [3467383293367650], 'au': 'Mark', 'rtg': 1, 'ts': 1537310633, 'num': 1, 'tp': 3467362509744518}, 3467383293555110: {'p': 0, 'txt': '<p>ttesttestst</p>\\n', 'c': 3467383293555110, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537310653, 'num': 0, 'tp': 3467383293555110}, 3467383293367650: {'p': 3467362509744518, 'txt': '<p>tadfas</p>\\n', 'c': 3467383293367650, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537310653, 'tp': 3467362509744518}, 3467470637563061: {'p': 0, 'txt': '<p>zz</p>\\n', 'c': 3467470637563061, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537310736, 'num': 0, 'tp': 3467470637563061}, 3467478106854362: {'p': 0, 'txt': '<p>zzfdsfdsa</p>\\n', 'c': 3467478106854362, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537310743, 'num': 0, 'tp': 3467478106854362}, 3467478107397231: {'p': 0, 'txt': '<p>2</p>\\n', 'c': 3467478107397231, 'cs': [3467500195452975], 'au': 'Mark', 'rtg': 1, 'ts': 1537310743, 'num': 1, 'tp': 3467478107397231}, 3467500195766276: {'p': 0, 'txt': '<p>3</p>\\n', 'c': 3467500195766276, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537310764, 'num': 0, 'tp': 3467500195766276}, 3467500195452975: {'p': 3467478107397231, 'txt': '<p>tat</p>\\n', 'c': 3467500195452975, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537310764, 'tp': 3467478107397231}, 3535631493306510: {'p': 0, 'txt': '<p>New comment</p>\\n', 'c': 3535631493306510, 'cs': [], 'au': 'Mark', 'rtg': 1, 'ts': 1537375739, 'num': 0, 'tp': 3535631493306510}}}

o = {"name":"MalthusianProphet","tl":2004,"dankmemes":True,"list":[1,2,3,4,5,6]}


b_mrp = mrpacker.pack(o)
b_mrj = mrjson.dumps(o)
b_msg = msgpack.packb(o)
"""

print( "mrpacker:" )
print ("  ",(min(timeit.Timer('mrpacker.pack(o)', setup=setup).repeat(10000, 5))))
print ("  ",(min(timeit.Timer('mrpacker.unpack(b_mrp)', setup=setup).repeat(10000, 5))))
print( "mrjson:" )
print ("  ",(min(timeit.Timer('mrjson.dumps(o)', setup=setup).repeat(10000, 5))))
print ("  ",(min(timeit.Timer('mrjson.loads(b_mrj)', setup=setup).repeat(10000, 5))))
print( "msgpack:" )
print ("  ",(min(timeit.Timer('msgpack.packb(o)', setup=setup).repeat(10000, 5))))
print ("  ",(min(timeit.Timer('msgpack.unpackb(b_msg,strict_map_key=False)', setup=setup).repeat(10000, 5))))

