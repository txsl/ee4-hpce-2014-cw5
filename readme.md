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


`txsl:` TBB?


As such, our final approach (as well as cleaning up the code to make it more object oriented), includes the following:
- Row by Row buffering, as we were out of time to try and resolve or work around the limitation of `process_buf_v2`. 
- OpenCL used of wide images.


This gives us a higher throughput compared to the original function, but unfortunately slower. We have, however, managed to fix the problem with limited memory, as our program acts as a buffer with the smallest memory footprint possible.


## Future Improvements (out of time!)

Unfortunately, there are only so many hours to the day. With an additional few days, I would have liked to explore:
- Fix `process_buf_v2` to look into the situation where the number of rows read at each turn need to be a multiple of `image_height - (levels*2)*2`. Here, either a catch for the `READ ERROR` thrown, or dynamically changing buffer windows.
- Investigate when best to switch to OpenCL, dependent on machine, bit depth, width, height and level. Furthermore, the following would have been useful for OpenCL investigation:
    - Bit Packing, to process more bits at one time. Especially useful for small images.
    - Remove branching using some neat tricks [1](https://graphics.stanford.edu/~seander/bithacks.html), [2](http://www.quora.com/Sorting-Algorithms/What-is-the-fastest-branch-free-way-to-compute-the-middle-of-three-elements)
- Use of TBB to simultaneously read, process and output. These tasks can be seperated, to improve the overall throughput. Need to be careful here to not write-before-read or read-before-write.
- Investigate the use of diamond shape for erode and dilate functions. Rather than using the cross (up, right, down, left), could look at calculating the minima over all needed spaces when levels > 2. This would save some iteration space, **but care would need to be taken to not overcomplicate this!**


Overall, in terms of priority, we should have looked into the following things (in this order):
- Optimising original C code [3 hours]
- OpenCL [6-9 hours]
- Bit Packing [6-12 hours]
- Buffering [6 hours]
- Buffering properly [12 hours]
- TBB for read/write/process [3 hours, prerequisite on buffering properly though]
- Diamond [3-6 hours]

## Testing Methodology

`txsl:` Please fill

`txsl:` include my shell file please. This saves the output of the original code, and then runs a dumb "check" on all other found versions of the code. 

`txsl:` Feel free to improve this test.

## Work Partitioning

`txsl:` Please fill