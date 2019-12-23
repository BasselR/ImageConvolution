# ImageConvolution
For information: https://en.wikipedia.org/wiki/Kernel_(image_processing)

This C program performs a convolution on a given image file, based on a provided convolution kernel.

To run the program, first execute the makefile using `make`

Then you can run the generated program "filter" like so: `./filter input.ppm kernel output.ppm`

This will use the kernel provided in the file "kernel" to perform an image convolution on the provided PPM file, and output the result in another PPM file.

For example: `./filter sample.ppm kernel sampleOut.ppm`

A sample picture file has been provided (in both its JPG and PPM formats), but you can use your own JPG pictures as long as you convert them to PPM format before using them as input. This can be done using converters online. Alternatively, if you have ImageMagick installed, like so:

JPG to PPM: `convert -compress none sample.jpg sample.ppm`

PPM to JPG: `convert sample.ppm sample.jpg`

The text file "kernel" is formatted like so:

```
n
scale
n rows of n numbers each
```

For example:

```
3
9
1 1 1
1 1 1
1 1 1
```

In the above example, the kernel would be a normalized box blur, using a kernel of all 1s and a scale of 9.
