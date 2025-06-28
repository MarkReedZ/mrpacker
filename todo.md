# mrpacker Performance Optimization TODO

## Context & Problem Analysis

### Performance Gap Identified
- **Twitter.json**: orjson unpacks 50% faster (10.2s vs 15.4s)
- **Canada.json**: mrpacker excels with float-heavy data (4.0s vs 4.6s)
- Root cause: mrpacker lacks string-specific optimizations crucial for typical JSON workloads

### Data Structure Analysis Findings
**Twitter.json characteristics:**
- 37,528 strings (avg length 29.8 chars) 
- 99,795 dictionary keys (all strings)
- 12,126 nested dictionaries
- Total string operations: ~137,323

**Canada.json characteristics:**
- 111,080 floats (coordinates)
- 56,045 arrays
- Only 4 strings
- Simple repetitive structure

### Why orjson Outperforms on String-Heavy Data

1. **String Deduplication & Caching**
   - Uses AssociativeCache with 2048-entry capacity
   - Caches frequently used dictionary keys
   - Avoids redundant Python string object creation

2. **Advanced Dictionary Handling**
   - Pre-sizes dictionaries with _PyDict_NewPresized()
   - Optimized key insertion patterns
   - Single-pass object construction

3. **Memory Layout Efficiency**
   - yyjson provides contiguous memory parsing
   - Single allocation for entire JSON structure
   - Efficient pointer arithmetic traversal

4. **Parsing Algorithm**
   - Batch processes strings during single parse pass
   - Leverages string interning
   - Optimized for JSON's inherent string-heavy nature

### mrpacker's Current Limitations

1. **String Handling Inefficiencies**
   - Each string creates separate PyUnicode_FromStringAndSize() call
   - No deduplication of repeated strings/keys
   - No caching mechanism

2. **Dictionary Construction**
   - Uses basic PyDict_New() without pre-sizing
   - Incremental construction causes hash table resizing
   - No key optimization

3. **Memory Pattern**
   - Separate allocations for each Python object
   - More memory fragmentation
   - Recursive parsing with stack overhead

## OPTIMIZATION ROADMAP

### Phase 1: String Optimization (High Impact)

#### 1.1 Implement String Deduplication Cache
- **File**: `src/unpack.c`
- **Goal**: Cache frequently used strings to avoid redundant Python object creation
- **Implementation**:
  ```c
  // Add string cache structure
  typedef struct {
      char* key;
      PyObject* value;
      uint32_t hash;
  } string_cache_entry;
  
  #define STRING_CACHE_SIZE 2048
  static string_cache_entry string_cache[STRING_CACHE_SIZE];
  static int cache_initialized = 0;
  ```
- **Functions needed**:
  - `init_string_cache()`
  - `lookup_cached_string(char* str, int len)`
  - `cache_string(char* str, int len, PyObject* py_str)`
  - `clear_string_cache()`

#### 1.2 Optimize Dictionary Key Caching
- **Target**: Lines 87-103 in `src/unpack.c` (string unpacking)
- **Strategy**: Separate cache for dictionary keys vs values
- **Implementation**: Add `is_dict_key` parameter to string functions

#### 1.3 Add String Interning for Common Keys
- **Common JSON keys**: "id", "name", "type", "data", "value", "text", etc.
- **Implementation**: Pre-populate cache with frequently used keys
- **Location**: Module initialization in `src/mrpacker.c`

### Phase 2: Dictionary Optimization (Medium Impact)

#### 2.1 Implement Dictionary Pre-sizing
- **File**: `src/unpack.c`, lines 180-220 (dict creation)
- **Current code**:
  ```c
  parents[depth] = PyDict_New();
  ```
- **Replacement**:
  ```c
  parents[depth] = _PyDict_NewPresized(l);  // l = known size
  ```
- **Benefit**: Avoids hash table resizing during construction

#### 2.2 Optimize Dictionary Construction Pattern
- **Current**: Key-value pairs added incrementally with potential resizing
- **Target**: Batch insertion patterns where possible
- **Implementation**: Pre-allocate and bulk-insert when size is known

### Phase 3: Memory Layout Optimization (Medium Impact)

#### 3.1 Implement Object Pooling
- **Goal**: Reduce Python object allocation overhead
- **Strategy**: Pool commonly used objects (small integers, booleans, None)
- **Implementation**: 
  ```c
  // Pre-allocated object pools
  static PyObject* small_int_pool[256];  // 0-255
  static PyObject* cached_none;
  static PyObject* cached_true;
  static PyObject* cached_false;
  ```

#### 3.2 Optimize Buffer Management
- **Current**: Single buffer traversal with pointer arithmetic
- **Enhancement**: Add buffer validation and bounds checking
- **Location**: `decode()` function in `src/unpack.c`

### Phase 4: Parsing Algorithm Improvements (Low-Medium Impact)

#### 4.1 Add Fast Path for Common Patterns
- **Empty containers**: `{}`, `[]` 
- **Small strings**: Length < 16 characters
- **Small integers**: Values 0-255
- **Implementation**: Add special case handlers before main switch statement

#### 4.2 Improve Error Handling
- **Current**: Basic error messages
- **Enhancement**: More descriptive error context with position information
- **Benefit**: Better debugging without performance penalty

### Phase 5: Advanced Optimizations (Research Phase)

#### 5.1 SIMD String Processing
- **Research**: Vector instructions for string validation/copying
- **Target**: Long string processing optimization
- **Platform**: x86-64 SSE/AVX instructions

#### 5.2 Custom String Allocator
- **Goal**: Reduce malloc/free overhead for string objects
- **Strategy**: Arena-based allocation for temporary string processing
- **Complexity**: High - requires careful memory management

## IMPLEMENTATION PRIORITY

### Immediate (Weeks 1-2)
1. String deduplication cache (1.1)
2. Dictionary pre-sizing (2.1)

### Short-term (Weeks 3-4)  
3. Dictionary key caching (1.2)
4. Object pooling for primitives (3.1)

### Medium-term (Weeks 5-8)
5. String interning (1.3)
6. Fast paths for common patterns (4.1)

### Long-term (Research)
7. SIMD optimizations (5.1)
8. Custom allocators (5.2)

## TESTING STRATEGY

### Benchmarks to Track
1. **Twitter.json** - Primary target for string-heavy optimization
2. **Canada.json** - Ensure float performance doesn't regress
3. **Mixed workloads** - Various JSON structures
4. **Memory usage** - Track RSS during parsing

### Success Criteria
- **Goal**: Match or exceed orjson performance on string-heavy JSON
- **Twitter.json target**: Reduce unpack time from 15.4s to <12s (20% improvement)
- **Memory**: No significant memory usage increase
- **Compatibility**: Maintain existing API and format compatibility

## IMPLEMENTATION NOTES

### Code Organization
- Keep optimizations optional via compile-time flags initially
- Maintain backward compatibility
- Add comprehensive tests for each optimization

### Performance Measurement
- Use existing `bench_comprehensive.py` for validation
- Add micro-benchmarks for individual optimizations
- Profile with tools like `perf` and `valgrind`

### Risk Mitigation
- Implement optimizations incrementally
- Maintain fallback to original code paths
- Extensive testing on various JSON structures

## REFERENCES

### Key Code Locations
- String unpacking: `src/unpack.c:87-103`
- Dictionary creation: `src/unpack.c:180-220`
- Main decode loop: `src/unpack.c:76-250`
- Module init: `src/mrpacker.c:40-60`

### Performance Analysis Context
- Benchmark results from `bench_comprehensive.py`
- orjson source analysis: `orjson/src/deserialize/`
- Data structure analysis of test datasets

### Technical Background
- Python C API documentation for PyDict_* and PyUnicode_* functions
- Hash table implementation details for dictionary optimization
- Memory management patterns in high-performance C extensions 