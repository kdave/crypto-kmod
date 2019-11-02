Ports of cryptographic algorithms to linux crypto API. Experimental, for
testing and evaluation only.

Done:

* BLAKE2s
* BLAKE2b
  * generate assembly for SSE2, SSE4.1, AVX2

Testing:

```
$ make
$ sudo insmod blake2s.ko
$ echo 'hi' | kcapi-dgst -c blake2s --hex
```

Generators

```
$ make gen
$ make
```
