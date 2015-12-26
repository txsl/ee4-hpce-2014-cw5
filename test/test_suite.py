import os

# from plumbum.cmd import ls
# from plumbum import local
import envoy
from wand.image import Image


# imgs = ls["img"]()

pwd = os.getcwd()

# print type(imgs)
imgs = os.listdir("img/")

for img in imgs:
    path_str = pwd + "/img/" + img

    with Image(filename=path_str) as i:
        width = i.width
        height = i.height

    # chain = local["/usr/local/bin/convert", path_str, "-depth 8", "gray:-" ]
    # print chain
    r = envoy.run("/usr/local/bin/convert" + path_str + "-depth 8 gray:-  |  ../bin/process " + str(width)  + " " + str(height) + " 8 3")
    # | local["../bin/process", width, height, "8", "3"]
    print r.std_err
