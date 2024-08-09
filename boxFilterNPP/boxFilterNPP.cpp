/* Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define WINDOWS_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#pragma warning(disable : 4819)
#endif

#include <Exceptions.h>
#include <ImageIO.h>
#include <ImagesCPU.h>
#include <ImagesNPP.h>

#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include <cuda_runtime.h>
#include <npp.h>

#include <helper_cuda.h>
#include <helper_string.h>

bool printfNPPinfo(int argc, char *argv[])
{
  const NppLibraryVersion *libVer = nppGetLibVersion();

  printf("NPP Library Version %d.%d.%d\n", libVer->major, libVer->minor,
         libVer->build);

  int driverVersion, runtimeVersion;
  cudaDriverGetVersion(&driverVersion);
  cudaRuntimeGetVersion(&runtimeVersion);

  printf("  CUDA Driver  Version: %d.%d\n", driverVersion / 1000,
         (driverVersion % 100) / 10);
  printf("  CUDA Runtime Version: %d.%d\n", runtimeVersion / 1000,
         (runtimeVersion % 100) / 10);

  // Min spec is SM 1.0 devices
  bool bVal = checkCudaCapabilities(1, 0);
  return bVal;
}

void checkCudaError(cudaError_t status, const char *msg)
{
  if (status != cudaSuccess)
  {
    std::cerr << "CUDA Error: " << msg << " - " << cudaGetErrorString(status) << std::endl;
  }
}

int main(int argc, char *argv[])
{
  printf("%s Starting...\n\n", argv[0]);

  try
  {
    std::vector<std::string> imageFilenames = {
        "Lena0.pgm", "Lena1.pgm", "Lena2.pgm", "Lena3.pgm", "Lena4.pgm",
        "Lena5.pgm", "Lena6.pgm", "Lena7.pgm", "Lena8.pgm", "Lena9.pgm"};

    findCudaDevice(argc, (const char **)argv);

    if (printfNPPinfo(argc, argv) == false)
    {
      exit(EXIT_SUCCESS);
    }

    // Print GPU information
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, 0);
    std::cout << "GPU: " << deviceProp.name << std::endl;
    std::cout << "Compute capability: " << deviceProp.major << "." << deviceProp.minor << std::endl;
    std::cout << "Total global memory: " << deviceProp.totalGlobalMem / (1024 * 1024) << " MB" << std::endl;

    for (size_t i = 0; i < imageFilenames.size(); ++i)
    {
      const auto &filename = imageFilenames[i];
      std::cout << "\n===== Processing image " << i << ": " << filename << " =====" << std::endl;

      std::string sFilename = filename;

      // Check if the input file exists
      std::ifstream infile(sFilename.data(), std::ifstream::in);
      if (!infile.good())
      {
        std::cout << "boxFilterNPP unable to open: <" << sFilename.data() << ">" << std::endl;
        continue;
      }
      infile.close();

      std::string sResultFilename = sFilename;
      std::string::size_type dot = sResultFilename.rfind('.');
      if (dot != std::string::npos)
      {
        sResultFilename = sResultFilename.substr(0, dot);
      }
      sResultFilename += "_boxFilter.pgm";

      // Perform box filter on the current image
      try
      {
        // declare a host image object for an 8-bit grayscale image
        npp::ImageCPU_8u_C1 oHostSrc;
        // load gray-scale image from disk
        npp::loadImage(sFilename, oHostSrc);

        std::cout << "Loaded image: " << sFilename << std::endl;
        std::cout << "Image dimensions: " << oHostSrc.width() << "x" << oHostSrc.height() << std::endl;

        // Print first few pixel values for debugging
        std::cout << "First 10 pixel values: ";
        for (int i = 0; i < 10 && i < oHostSrc.width() * oHostSrc.height(); ++i)
        {
          std::cout << (int)oHostSrc.data()[i] << " ";
        }
        std::cout << std::endl;

        // declare a device image and copy construct from the host image,
        // i.e., upload host to device
        npp::ImageNPP_8u_C1 oDeviceSrc(oHostSrc);

        checkCudaError(cudaGetLastError(), "After image upload");

        // Create struct with box-filter mask size
        NppiSize oMaskSize = {5, 5};

        NppiSize oSrcSize = {(int)oDeviceSrc.width(), (int)oDeviceSrc.height()};
        NppiPoint oSrcOffset = {0, 0};

        // Create struct with ROI size
        NppiSize oSizeROI = {(int)oDeviceSrc.width(), (int)oDeviceSrc.height()};
        // Allocate device image of appropriately reduced size
        npp::ImageNPP_8u_C1 oDeviceDst(oSizeROI.width, oSizeROI.height);
        // Set anchor point inside the mask to (oMaskSize.width / 2, oMaskSize.height / 2)
        // It should round down when odd
        NppiPoint oAnchor = {oMaskSize.width / 2, oMaskSize.height / 2};

        std::cout << "Mask size: " << oMaskSize.width << "x" << oMaskSize.height << std::endl;
        std::cout << "ROI size: " << oSizeROI.width << "x" << oSizeROI.height << std::endl;
        std::cout << "Anchor point: (" << oAnchor.x << ", " << oAnchor.y << ")" << std::endl;

        // Run box filter
        NppStatus nppStatus = nppiFilterBoxBorder_8u_C1R(
            oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
            oDeviceDst.data(), oDeviceDst.pitch(), oSizeROI, oMaskSize, oAnchor,
            NPP_BORDER_REPLICATE);

        if (nppStatus != NPP_SUCCESS)
        {
          std::cerr << "NPP error: " << nppStatus << std::endl;
          // Handle the error or exit the program
          continue;
        }

        checkCudaError(cudaGetLastError(), "After box filter");

        checkCudaError(cudaDeviceSynchronize(), "cudaDeviceSynchronize after box filter");

        // Declare a host image for the result
        npp::ImageCPU_8u_C1 oHostDst(oDeviceDst.size());
        // Copy the device result data into the host image
        checkCudaError(cudaMemcpy2D(oHostDst.data(), oHostDst.pitch(),
                                    oDeviceDst.data(), oDeviceDst.pitch(),
                                    oDeviceDst.width(), oDeviceDst.height(),
                                    cudaMemcpyDeviceToHost),
                       "cudaMemcpy2D");

        saveImage(sResultFilename, oHostDst);
        std::cout << "Saved image: " << sResultFilename << std::endl;

        // Free GPU memory
        nppiFree(oDeviceSrc.data());
        nppiFree(oDeviceDst.data());

        // Reset device
        checkCudaError(cudaDeviceReset(), "cudaDeviceReset");

        // Optional: add a small delay
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      catch (const std::exception &ex)
      {
        std::cerr << "Error processing image: " << sFilename << std::endl;
        std::cerr << "Error message: " << ex.what() << std::endl;
      }

      // Print memory usage after each iteration
      size_t free_byte;
      size_t total_byte;
      checkCudaError(cudaMemGetInfo(&free_byte, &total_byte), "cudaMemGetInfo");
      double free_db = (double)free_byte;
      double total_db = (double)total_byte;
      double used_db = total_db - free_db;
      printf("GPU memory usage: used = %f, free = %f MB, total = %f MB\n",
             used_db / 1024.0 / 1024.0, free_db / 1024.0 / 1024.0, total_db / 1024.0 / 1024.0);
    }

    exit(EXIT_SUCCESS);
  }
  catch (npp::Exception &rException)
  {
    std::cerr << "Program error! The following exception occurred: \n";
    std::cerr << rException << std::endl;
    std::cerr << "Aborting." << std::endl;

    exit(EXIT_FAILURE);
  }
  catch (...)
  {
    std::cerr << "Program error! An unknown type of exception occurred. \n";
    std::cerr << "Aborting." << std::endl;

    exit(EXIT_FAILURE);
    return -1;
  }

  return 0;
}