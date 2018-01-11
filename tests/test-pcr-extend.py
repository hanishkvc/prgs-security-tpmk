#!/bin/python3

import os

c1=bytes.fromhex("0000000000")
c2=bytes.fromhex("0123456789")

f=open("./testdata4sha1.raw","wb+")
f.write(c1+c1+c1+c1+c2+c2+c2+c2)
f.close()

os.system("sha1sum ./testdata4sha1.raw")

a=''
for i in range(20):
  a=a+chr(0)

#
#b=''
#for i in range(10):
#  b=b+chr(i)

b=''
b+=chr(0x01)
b+=chr(0x23)
b+=chr(0x45)
b+=chr(0x67)
b+=chr(0x89)
#f.write(a+b+b+b+b)
