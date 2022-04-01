# Ray tracer

My implementation of a Monte-Carlo path tracer, made whilst following Shirley's fantastic 
[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) book. 
This renderer is also multi-threaded for reduced render times!

![out](https://user-images.githubusercontent.com/11539931/161174533-5f433d28-0623-4200-a71c-d1ce88f97618.png)

*1280x720 at 32 samples per-pixel, up to 8 bounces. This frame took 50 seconds to render on my Ryzen 1700X.*

## Future plans

Other things I want to implement:

- BVH
- Light sources
- Other styles of ray tracing (classic Whitted-style ray tracing, distributed ray tracing)
- Denoising
- Interactivity, with a GUI to change the render options
- GPU acceleration?
- General performance improvements
