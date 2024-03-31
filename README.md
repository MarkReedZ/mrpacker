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
  b = mrpacker.pack( o )
  print( mrpacker.unpack(b) )
```

## Benchmarks

See b.py and benchmark your own real world cases as performance may vary

```
Pack then unpack
  mrpacker:
    1.669600010245631e-05
    4.354700013209367e-05
  mrjson:
    2.9059000098641263e-05
    6.217899999683141e-05
  msgpack:
    4.313099998398684e-05
    5.4524000006495044e-05
```
