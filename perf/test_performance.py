import os, sys, csv
import math
import time

# from plumbum.cmd import ls
# from plumbum import local
import envoy
from wand.image import Image


def fname_gen(width, height, bits):
    return "img/input_{0}_{1}_{2}.raw".format(width, height, bits)

bits = [2**x for x in range(0, 6)]
bits = [1,8] # Since we are only interested in 1 or 8 bit depth conversions for performance testing

length_range = [2**x for x in range(2, 24)]

pwd = os.getcwd()

try:
    test_process = sys.argv[1]
except IndexError:
    test_process = "process"

# for img in imgs:
    # path_str = pwd + "/img/" + img

    # with Image(filename=path_str) as i:
    #     width = i.width
    #     height = i.height

    # chain = local["/usr/local/bin/convert", path_str, "-depth 8", "gray:-" ]
    # print chain

# img = Image(file='big.jpg')
# img.type = 'grayscale'

# for b in reversed(bits):
#     for width in length_range:
#         for height in length_range:
#             fname = fname_gen(width, height, b)
#             if os.path.exists(fname):
#                 pass
#             else:
#             # img.resize(width, height)
#             # img.save('img/big_{0}_{1}_{2}.raw'.format(width, height, b))
#                 envoy.run('convert big.jpg -resize {0}x{1}\! -depth {2} gray:{3}'.format(width, height, b, fname))
# exit()


writer = csv.writer(sys.stdout)
writer.writerow(['Command', 'Bits', 'Width', 'Height', 'Execution Time'])

for b in bits:
    for width in length_range:
        for height in length_range:

            if width*b % 64 != 0:
                continue

            # size = width * height * b / 8
            # print "Testing with size: ", size
            fname = fname_gen(width, height, b)

            if not os.path.exists(fname):
                envoy.run('convert big.jpg -resize {0}x{1}\! -depth {2} gray:{3}'.format(width, height, b, fname))
                # print 'convert big.jpg -resize {0}x{1}\! -depth {2} gray:{3}'.format(width, height, b, fname)

            # command = "cat {0} | ../bin/{5} {1} {2} {3} {4}".format(fname, width, height, b, 8, test_process)
            command = "../bin/{0} {1} {2} {3} {4}".format(test_process, width, height, b, 8)
            
            # print command

            
            # fhandle = open(fname, 'rb')
            with open(fname, mode='rb') as file: # b is important -> binary
                file_data = file.read()
            
            tx = time.time()
            r = envoy.run(command, timeout=10000, data=file_data)

            print r.command

            t_ex = time.time() - tx

            if r.status_code:
                print "Error: ", r.std_err
                print "Command: ", r.command
                exit()
            
            else:
                writer.writerow([test_process, b, width, height, t_ex])
