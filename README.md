## MrPacker

MrPacker is a binary object packer for python that is faster and smaller than JSON.

To install it just run Pip as usual:

```sh
    $ pip install mrpacker
```

## Usage

May be used as a replacement for json

```python
  import mrpacker
  o = { "name":"mrpacker", "awesome?":"yes" }
  b = msgpack.packb( o )
  print( mrpacker.unpack(b) )
```

## Benchmarks

See b.py and benchmark your own real world cases as performance may vary
