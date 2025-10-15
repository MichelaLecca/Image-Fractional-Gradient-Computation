# Image-Fractional-Gradient-Computation
C++ code (based on OpenCV library) for computing the fractional gradient of an image exploiting the Atangana-Baleanu integral operator. 


Given a RGB or a gray-level image, the code computes the fractional gradient of the image luminance. 
The user parameters are:
1) A value <gsigma> for smoothing the luminance before computing the horizontal and vertical derivatives: this operation aims at decreasing possible image noise; when <gsigma?> is set to -1 no smoothing is performed. The smoothing filter is Gaussian;
2) The order <sigma> of the derivative.

The code computes the horizontal and vertical fractional gradient of the luminance and shows the fractional gradient magnitude computed by the L-infty norm.

The fractional derivatives are computed using the kernels described in [1] (see Section 3.4, "Mask MP2").

Fractional gradients are widely used for contrast enhancement and the fractional gradient implmeneted here has been used in [2] to enhance backlight images.

[1] Ghanbari, Behzad, and Abdon Atangana. "Some new edge detecting techniques based on fractional derivatives with non-local and non-singular kernels." Advances in Difference Equations 2020.1 (2020): 435.

[2] Lecca, Michela, and Raul Paolo Serapioni. "Non-uniformly lighted image enhancement exploiting the Atangana–Baleanu fractional integral and the Sobel filter." Journal of the Optical Society of America A 42.2 (2025): 183-192.
