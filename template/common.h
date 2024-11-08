// Template, IGAD version 3
// IGAD/NHTV/UU - Jacco Bikker - 2006-2022

// common.h is to be included in host and device code and stores
// global settings and defines.


#ifdef _DEBUG
#define LOCATION  std::format(" FILE {:32.32} | LINE {}\n", __FILE__, __LINE__)
#define LOGMESSAGE(x)	std::cerr << "MESSAGE    |" << LOCATION << x << '\n' << std::endl
#define LOGWARNING(x)	std::cerr << "WARNING !  |" << LOCATION << x << '\n' << std::endl
#define LOGERROR(x)		std::cerr << "ERROR   !! |" << LOCATION << x << '\n' << std::endl
#else
#define LOGMESSAGE(x)
#define LOGWARNING(x)
#define LOGERROR(x)
#endif

// default screen resolution
#define SCRWIDTH	1920
#define SCRHEIGHT	1080
#define FULLSCREEN

// constants
#define PI		3.14159265358979323846264f
#define INVPI		0.31830988618379067153777f
#define INV2PI		0.15915494309189533576888f
#define TWOPI		6.28318530717958647692528f
#define SQRT_PI_INV	0.56418958355f
#define LARGE_FLOAT	1e34f

// IMPORTANT NOTE ON OPENCL COMPATIBILITY ON OLDER LAPTOPS:
// Without a GPU, a laptop needs at least a 'Broadwell' Intel CPU (5th gen, 2015):
// Intel's OpenCL implementation 'NEO' is not available on older devices.
// Same is true for Vulkan, OpenGL 4.0 and beyond, as well as DX11 and DX12.