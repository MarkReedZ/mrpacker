import timeit
import json
import os
import sys
from collections import defaultdict
import statistics

# Import libraries - some may not be available
libraries = {}
try:
    import mrpacker
    libraries['mrpacker'] = mrpacker
except ImportError:
    print("mrpacker not available")
z="""
try:
    import orjson
    libraries['orjson'] = orjson
except ImportError:
    print("orjson not available - install with: pip install orjson")

try:
    import msgpack
    libraries['msgpack'] = msgpack
except ImportError:
    print("msgpack not available - install with: pip install msgpack")

#try:
    #import cbor2
    #libraries['cbor2'] = cbor2
#except ImportError:
    #print("cbor2 not available - install with: pip install cbor2")

"""
#try:
    #import ujson
    #libraries['ujson'] = ujson
#except ImportError:
    #print("ujson not available - install with: pip install ujson")

# Standard library JSON is always available
#libraries['json'] = json

def load_test_data():
    """Load test datasets from test_data directory"""
    test_data = {}
    test_data_dir = "test_data"
    
    if not os.path.exists(test_data_dir):
        print(f"Warning: {test_data_dir} not found, using inline data")
        # Fallback to inline test data
        test_data['inline'] = {
            'tl': [3376686588804032, 3467362509744518],
            'd': {
                3376686588804032: {'p': 0, 'txt': 'test', 'c': 3376686588804032, 'au': 'Mark', 'rtg': 1},
                3467362509744518: {'p': 0, 'txt': 'another test', 'c': 3467362509744518, 'au': 'Mark', 'rtg': 2}
            }
        }
        return test_data
    
    # Load JSON test files (limit size for reasonable benchmark times)
    json_files = [
        #('small', 's128.json'),
        #('medium', 'doc.json'), 
        #('large', 'twit.json'),
        # Skip very large files for now to keep benchmark reasonable
         ('twitter', 'twitter.json'),
         #('canada', 'canada.json'),
    ]
    
    for name, filename in json_files:
        filepath = os.path.join(test_data_dir, filename)
        if os.path.exists(filepath):
            try:
                with open(filepath, 'r') as f:
                    data = json.load(f)
                    test_data[name] = data
                    print(f"Loaded {name}: {len(json.dumps(data))} chars")
            except Exception as e:
                print(f"Failed to load {filename}: {e}")
    
    return test_data

def serialize_funcs():
    """Define serialization functions for each library"""
    funcs = {}
    
    if 'json' in libraries:
        funcs['json'] = {
            'pack': lambda obj: json.dumps(obj).encode('utf-8'),
            'unpack': lambda data: json.loads(data.decode('utf-8'))
        }
    
    if 'ujson' in libraries:
        funcs['ujson'] = {
            'pack': lambda obj: ujson.dumps(obj).encode('utf-8'),
            'unpack': lambda data: ujson.loads(data.decode('utf-8'))
        }
    
    if 'orjson' in libraries:
        funcs['orjson'] = {
            'pack': lambda obj: orjson.dumps(obj, option=orjson.OPT_NON_STR_KEYS),
            'unpack': lambda data: orjson.loads(data)
        }
    
    if 'msgpack' in libraries:
        funcs['msgpack'] = {
            'pack': lambda obj: msgpack.packb(obj, strict_types=False),
            'unpack': lambda data: msgpack.unpackb(data, strict_map_key=False)
        }
    
    if 'cbor2' in libraries:
        funcs['cbor2'] = {
            'pack': lambda obj: cbor2.dumps(obj),
            'unpack': lambda data: cbor2.loads(data)
        }
    
    if 'mrpacker' in libraries:
        funcs['mrpacker'] = {
            'pack': lambda obj: mrpacker.pack(obj),
            'unpack': lambda data: mrpacker.unpack(data)
        }
    
    return funcs

def benchmark_serializer(name, pack_func, unpack_func, test_data, iterations=1000):
    """Benchmark a single serializer"""
    results = {}
    
    for data_name, data in test_data.items():
        try:
            # Test serialization
            packed_data = pack_func(data)
            size = len(packed_data)
            
            # Verify round-trip works
            unpacked_data = unpack_func(packed_data)
            
            # Time serialization
            pack_timer = timeit.Timer(lambda: pack_func(data))
            pack_times = pack_timer.repeat(repeat=5, number=iterations)
            pack_time = min(pack_times) / iterations * 1000  # ms per operation
            
            # Time deserialization  
            unpack_timer = timeit.Timer(lambda: unpack_func(packed_data))
            unpack_times = unpack_timer.repeat(repeat=5, number=iterations)
            unpack_time = min(unpack_times) / iterations * 1000  # ms per operation
            
            results[data_name] = {
                'size': size,
                'pack_time': pack_time,
                'unpack_time': unpack_time,
                'total_time': pack_time + unpack_time
            }
            
        except Exception as e:
            print(f"Error testing {name} with {data_name}: {e}")
            results[data_name] = None
    
    return results

def print_results(all_results, test_data):
    """Print benchmark results in a nice table format"""
    
    # Print size comparison
    print("\n" + "="*80)
    print("SERIALIZED SIZE COMPARISON (bytes)")
    print("="*80)
    
    for data_name in test_data.keys():
        print(f"\n{data_name.upper()} dataset:")
        print("-" * 40)
        
        sizes = []
        for lib_name, results in all_results.items():
            if results and data_name in results and results[data_name]:
                size = results[data_name]['size']
                sizes.append((lib_name, size))
        
        # Sort by size
        sizes.sort(key=lambda x: x[1])
        
        if sizes:
            smallest = sizes[0][1]
            for lib_name, size in sizes:
                ratio = size / smallest if smallest > 0 else 1.0
                print(f"  {lib_name:<12}: {size:>8,} bytes ({ratio:>5.2f}x)")
    
    # Print speed comparison  
    print("\n" + "="*80)
    print("SERIALIZATION SPEED COMPARISON (ms per operation)")
    print("="*80)
    
    for data_name in test_data.keys():
        print(f"\n{data_name.upper()} dataset:")
        print("-" * 50)
        print(f"{'Library':<12} {'Pack':<8} {'Unpack':<8} {'Total':<8}")
        print("-" * 50)
        
        speeds = []
        for lib_name, results in all_results.items():
            if results and data_name in results and results[data_name]:
                result = results[data_name]
                speeds.append((lib_name, result['pack_time'], result['unpack_time'], result['total_time']))
        
        # Sort by total time
        speeds.sort(key=lambda x: x[3])
        
        for lib_name, pack_time, unpack_time, total_time in speeds:
            print(f"{lib_name:<12} {pack_time:>7.3f} {unpack_time:>7.3f} {total_time:>7.3f}")

def main():
    print("Loading test data...")
    test_data = load_test_data()
    
    if not test_data:
        print("No test data available!")
        return
    
    print(f"Available libraries: {list(libraries.keys())}")
    print(f"Test datasets: {list(test_data.keys())}")
    
    serialize_functions = serialize_funcs()
    
    if not serialize_functions:
        print("No serialization libraries available!")
        return
    
    print(f"\nRunning benchmarks with {len(serialize_functions)} libraries...")
    
    all_results = {}
    
    for lib_name, funcs in serialize_functions.items():
        print(f"Testing {lib_name}...")
        try:
            results = benchmark_serializer(
                lib_name, 
                funcs['pack'], 
                funcs['unpack'], 
                test_data,
                iterations=50  # Reduce iterations for faster testing
            )
            all_results[lib_name] = results
        except Exception as e:
            print(f"Failed to benchmark {lib_name}: {e}")
            all_results[lib_name] = None
    
    print_results(all_results, test_data)

if __name__ == "__main__":
    main() 
