Ports of cryptographic algorithms to linux crypto API. Experimental, for
testing and evaluation only.

Done:

* BLAKE2s
* BLAKE2b

Todo:

* BLAKE2p? modes
* self-tests

Testing:

```
$ make
$ sudo insmod blake2s.ko
$ echo 'hi' | kcapi-dgst -c blake2s --hex
```
