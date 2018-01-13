import os
import sys

if len(sys.argv) == 1:
    cmd = bytes.fromhex("80010000000a00000181")
else:
    cmd = bytes.fromhex(sys.argv[1])


os.system("mknod /dev/tpm0 c 200 0")
f=os.open("/dev/tpm0", os.O_RDWR)
os.write(f,cmd)
resp=os.read(f,4096)
os.close(f)

print("Cmd:[{}]\nResp:[{}]".format(cmd, resp))

