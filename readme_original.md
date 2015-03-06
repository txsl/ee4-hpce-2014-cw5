HPCE 2015 / CW5
===========

Deadline update
---------------

I've updated the deadline to Friday 6th Mar at 23:59. This
year I've been trying to keep deadlines stable, but it
is always a bit of balancing act given people have lots of
other things going on, and I'd like people to actually be able
to spend some time on these two courseworks.

The original intention was to keep courseworks out of the
DoC exam period, which will still happen, though it will
now encroach on the revision period. So given that a large
number of people seemed happy with that in the meme issue,
I'm happy to move things back (as always, deadline changes
need to be justified, and not disadvantage anyone).

Overview
--------

This is a program for performing morphological operations in gray-scale
images, and in particular very large images. The general idea of
morphological operations can be found here:
http://homepages.inf.ed.ac.uk/rbf/HIPR2/matmorph.htm

Functionality
--------------

The program performs either open or close operations,
where an open is an erode followed by a dilate, and
a close is a dilate followed by an erode.

Our erode operation replaces each pixel with the minimum
of its Von Neumann neighbourhood, i.e. the left-right
up-down cross we have used before. Dilate does the same, but
takes the maximum. At each boundary, the neigbourhood
it truncated to exclude parts which extend beyond the
image. So for example, in the top-left corner the
neighbourhood consists of just {middle,down,right}.

Images are input and output as raw binary gray-scale
images, with no header. The processing parameters are
set on the command line as "width height [bits] [levels]":

- Width: positive integer, up to and including 2^24
- Height: positive integer, up to and including 2^24
- Bits: a binary power <=32, default=8
- Levels: number of times to erode before dilating (or vice-versa),
    - 0 <= abs(levels) <= min(width/4, height/4, 64) 
    - default=1,
    - negative values are open, positive values are close
			  
The input will contain a stream of images, followed by
an end of file. There is no upper limit on the number
of images in a stream (so don't try to store them in RAM).

A constraint is that mod(Width*bits,64)==0. There
is no constraint on image size, beyond that imposed
by the width, height, and bits parameters. To be
more specific: you may be asked to process images up
to 2^44 or so in pixel count.

Image format
---------------

Images are represented as packed binary, in little-endian
scanline order. For images with less than 8 bits per pixel, the
left-most pixel is in the MSB. For example, the 64x3 1-bit
image with packed hex representation:

    00 00 00 00 ff ff ff ff  00 00 f0 f0 00 00 0f 0f  01 02 04 08 10 20 40 80

represents the image:

    0000000000000000000000000000000011111111111111111111111111111111
    0000000000000000111100001111000000000000000000000000111100001111
    0000000100000010000001000000100000010000001000000100000010000000

You can use imagemagick to convert to and from binary representation.
For example, to convert a 512x512 image input.png to 2-bit:

    convert input.png -depth 2 gray:input.raw

and to convert output.raw back again:

    convert -size 512x512 -depth 2 gray:output.raw output.png

They can also read/write on stdin/stdout for streaming. But, it is
also easy to generate images programmatically, particularly when
dealing with large images. You can even use `/dev/zero` and
friends if you just need something large to shove through,
or want to focus on performance.

Correctness
-------------

The output images should be bit-accurate correct. Any conflict
between the operation of this program and the definition of the
image processing and image format should be seen as a bug in
this program. For example, this program cannot handle very
large images, which is a bug.

Performance and constraints
-------------------------------

The metric used to evaluate this work is throughput
in frames per second. Throughput is defined as the time between
the first pixel of the first image entering the program, and
the last pixel of the last image leaving, divided by the
total number of images. Throughput measurement does not start
until the first pixel enters the pipeline, so performance measurement
is "on-hold" till that point. However, your program
must eventually read the first pixel. Practically speaking,
if your program doesn't read a pixel within one minute,
it will be considered to have hung.

The program should support the full spectrum of input
parameters correctly, including all image sizes and
all pixel depths. However, performance is only measured
for the 1-bit and 8-bit images. The images you'll process
can have any pixel distribution, but they are often quite
sparse, meaning a large proportion of the pixels are
zero. The zero to non-zero ratio may rise to 1:1000000
for very large images, with a large amount of spatial
clustering of the non-zero pixels. That may be useful in
some cases... or not.

Execution and compilation
------------------------------

Your program will be tested on a number of machines,
and should work correctly (if not necessarily quickly)
on all of them.

Performance will be tested on the g2.2xlarge instance type
(which does contain multiple CPU cores...)

Performance is subordinate to correctness, so be careful
when allocating buffers etc. It is always better to fall back to
software if something goes wrong with OpenCL setup, rather
than crashing or having the program quit.

Submission
------------

Submission is via git, and both partners in the pair
should give each other access, and ensure that their
repositories are synchronised. The submission will be
taken as the last git commit which is identical in 
both repositories (both partners have pushed and pulled
between their repositories), and occurs before the deadline.

Your repository should contain:

- aws_bootstrap.sh : A shell script which will perform any
  necessary system setup within a new HPCE-2014-GPU-Image instance.
  
- src : contains your submitted code. You may place whatever
  you want in here. Your program will always be run with the
  "src" directory as its working directory, so you can load any
  kernel files from there, or from sub-directories of "src".

- makefile : Calling `make all` should build your executable
  into `bin/process` (whether or not you use CMake or something
  else to do the building).

- readme.txt, readme.pdf, or readme.md : a document covering:
  - What is the approach used to improve performance, in
     terms of algorithms, patterns, and optimisations.
  - A description of any testing methodology or verification.
  - A summary of how work was partitioned within the pair,
      including planning, design, and testing, as well as coding.
  This document does not need to be long, it is not a project
   report.

As well as the code and the document, feel free to include
any other interesting artefacts, such as testing code
or performance measurements. Just make sure to clean out
anything large, like test images, object files, executables.

Marking scheme
-----------------

- 33% : Performance, relative to other implementations
- 33% : Correctness
- 33% : Code review (style, appropriateness of approach, readability...)

Plagiarism
-----------

Everyone's submission will be different for this coursework,
there is little chance of seeing the same code by accident.
However, you can use whatever code you like from this file.
You may also use third-party code, but you need to attribute
it very clearly and carefully, and be able to justify why
it was used - submissions created from mostly third-party
code will be frowned upon, but only in terms of marks.

Any code transfer between pairs will be treated as plagiarism.
I would suggest you keep your code private, not least because
you are competing with the others.
