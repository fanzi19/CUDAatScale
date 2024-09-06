# CUDAatScale
This is a modified version for multiple images processing version.
Original Version: Only one Lena.pgm image;
Modified Version: 10 Copies of Lena.pgm images.

## Code Organization
`boxFilterNPP`:
1. 10s of large images: Lena0.pgm ~ Lena9.pgm
2. Makefile
3. Other testing files (grey-sloth.bmp, grey-sloth.png, sloth.png)

`Common`: Essential utils;

`run.sh`: Files to run Lena0.pgm ~ Lena9.pgm

## Execution
Step 1: `cd boxFilterNPP`
Step 2: `make clean build`
Step 3: `make run`

## Result of 10s of Large Images
GPU Device 0: "Ampere" with compute capability 8.6

NPP Library Version 11.3.3
  CUDA Driver  Version: 12.0
  CUDA Runtime Version: 11.3
  Device 0: <          Ampere >, Compute SM 8.6 detected
GPU: NVIDIA A10G
Compute capability: 8.6
Total global memory: 22563 MB

===== Processing image 0: Lena0.pgm =====
Loaded image: Lena0.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena0_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 1: Lena1.pgm =====
Loaded image: Lena1.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena1_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 2: Lena2.pgm =====
Loaded image: Lena2.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena2_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 3: Lena3.pgm =====
Loaded image: Lena3.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena3_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 4: Lena4.pgm =====
Loaded image: Lena4.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena4_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 5: Lena5.pgm =====
Loaded image: Lena5.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena5_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 6: Lena6.pgm =====
Loaded image: Lena6.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena6_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 7: Lena7.pgm =====
Loaded image: Lena7.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena7_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 8: Lena8.pgm =====
Loaded image: Lena8.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena8_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB

===== Processing image 9: Lena9.pgm =====
Loaded image: Lena9.pgm
Image dimensions: 512x512
First 10 pixel values: 162 162 162 161 162 156 163 160 164 160 
Mask size: 5x5
ROI size: 512x512
Anchor point: (2, 2)
Saved image: Lena9_boxFilter.pgm
GPU memory usage: used = 10572.687500, free = 11991.187500 MB, total = 22563.875000 MB
