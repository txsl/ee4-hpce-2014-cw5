HPCE 2015 / CW5
===========

Submission for
- Dario Magliocchetti (dm1911)
- Thomas Lim (txl11)

### Overview

In the fifth coursework for HPCE, we look at applying open and close functions on images. Opening (or closing) consists of two operations; Eroding followed by a dilate. The details of this operations can be found in detail here: [Wiki](http://en.wikipedia.org/wiki/Mathematical_morphology), [Web](http://homepages.inf.ed.ac.uk/rbf/HIPR2/matmorph.htm)


The objective of this coursework is to implement an optimised solution that is:
- high throughput
- low latency
- can handle infinitely long images (no out of memory issues!)


## Approach

Explored Options:
- Use of OpenCL to calculate erode and dilate functions. This is particularly useful for wide images, as the overhead in copying data over is offset by the speed of processing.
    - `Process()` function was made standalone and then moved to a kernel
    - Use of `std::swap()` and `setArg()` to avoid unnecessary copying of data to openGL
    - Particularly helpful for wide images, and operations with a high `level`.
- Partitioning input data (buffering). Two approaches were looked into here:
    - Reading row-by-row into a cyclic buffer, to then perform the `erode`/`dilate` functions. This allows for a better throughput and ability to read infinite streams. However, note that this is slower than the original code!
    - To counter this, we can read multiple rows at a time (increase the buffer size). This is implemented in `process_buf_v2`. Multiple rows are read at one time, with an additional buffer to the top and to the bottom to correctly calculate the output pixels. **Note Limitation: The number of rows that are used in the buffer must be a factor of `image_height - (levels*2)*2`**. 


As such, our final approach (as well as cleaning up the code to make it more object oriented), includes the following:
- Row by Row buffering, as we were out of time to try and resolve or work around the limitation of `process_buf_v2`. 
- OpenCL used of wide images.


This gives us a higher throughput compared to the original function, but unfortunately slower. We have, however, managed to fix the problem with limited memory, as our program acts as a buffer with the smallest memory footprint possible.


## Future Improvements (out of time!)

Unfortunately, there are only so many hours to the day. With an additional few days, I would have liked to explore:
- Fix `process_buf_v2` to look into the situation where the number of rows read at each turn need to be a multiple of `image_height - (levels*2)*2`. Here, either a catch for the `READ ERROR` thrown, or dynamically changing buffer windows.
- Investigate when best to switch to OpenCL, dependent on machine, bit depth, width, height and level. Furthermore, the following would have been useful for OpenCL investigation:
    - Bit Packing, to process more bits at one time. Especially useful for small images.
    - Remove branching using some neat tricks [1](https://graphics.stanford.edu/~seander/bithacks.html), [2](http://www.quora.com/Sorting-Algorithms/What-is-the-fastest-branch-free-way-to-compute-the-middle-of-three-elements), [3](http://users.utcluj.ro/~raluca/ip_2014/ipl_07e.pdf)
- Use of TBB to simultaneously read, process and output. These tasks can be seperated, to improve the overall throughput. Need to be careful here to not write-before-read or read-before-write.
- Investigate the use of diamond shape for erode and dilate functions. Rather than using the cross (up, right, down, left), could look at calculating the minima over all needed spaces when levels > 2. This would save some iteration space, **but care would need to be taken to not overcomplicate this!**
- Improve Algorithm: [1](http://www.cs.technion.ac.il/~ron/PAPERS/EfficientMorphology_KimmelGil_PAMI_2002.pdf), [2](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.98.1196&rep=rep1&type=pdf). Not read properly, but some pointers perhaps to be found here: [3](http://internationalmathematicasymposium.org/IMS2006/IMS2006_CD/articles/Jankowski.pdf), [4](http://www.ijetae.com/files/Volume3Issue7/IJETAE_0713_48.pdf).
- Further performance testing with differing width and height images, and smaller gaps between image sizes for better plots.


Overall, in terms of priority, we should have looked into the following things (in this order):
- Optimising original C code [3 hours]
- OpenCL [6-9 hours]
- Bit Packing [6-12 hours]
- Buffering [6 hours]
- Buffering properly [12 hours]
- TBB for read/write/process [3 hours, prerequisite on buffering properly though]
- Diamond [3-6 hours]

## Testing Methodology

Testing is conducted in two flavours: functionality testing (that it meets the spec of the project), and performance testing (that it functions quickly).

Functionality testing was mainly conducted by the script `simple_compare` within the `test` directory. It runs an image through imagemagick converting it to a raw binary format. It then is passed through the original `process` command (now called `process_old`). This is used as a benchmark, with the output of `process_old` assumed to be correct. The output of all the other commands in `bin` called `process*` are then called with the same input. The output is compared using `diff`.

Since `process_old` could not handle files which were too large, it is assumed that if code can handle a smaller image correctly, it would also be able to handle a larger image correctly (which are are unable to test using `process_old`).

These tests were conducted locally, as well as on the AWS g2.2xlarge instance type (with HPCE-2014-GPU-Image) in order to ensure it runs appropriately on the target environment (or at least one of the target environments.).

Performance testing was conducted in the `perf` directory. The script `performance_tester.sh` was painstakingly built after many failed attempts at accurately timing the commands. The issue mainly arises from the number of parameters passed to the function, as well as the piping of `/dev/zero` to the command itself. The shell script written writes the output formatted in csv style for further anaylsis. 

The image size goes up in a square (equal widths and heights), designed to test across a large range of input sizes (all within spec). The image `output.png` in `perf` shows the results. Tests were run twice on both `process_old` and `process`. The two runs from each show little difference between each run. The low sized images show a startup time, but with `process_old` this rapidly increases (although there is an uncharacteristic dip). This then drops as `process_old` cannot read such a large file into memory. We see a more expected characteristic curve in `process` as the image grows in size (remember the total number of pixels are increasing in a square dimension, as it's legnth * width (which for this test is equal))).

## Work Partitioning

* dm1911: the `process` codebase, functional tests and initial Makefile
* txl11: performance testing, AWS testing and Linux compatible Makefile


