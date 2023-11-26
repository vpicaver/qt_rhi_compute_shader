# RHI Compute Shader Example

This project demonstrates the use of the Qt Rendering Hardware Interface (RHI) to perform compute operations using shaders. It's a basic example that showcases how to set up a compute shader pipeline and perform operations on buffers.

## Features

- Initialization of the Qt RHI for different platforms (Direct3D 12 on Windows, Metal on MacOS/iOS, Vulkan on supported platforms).
- Creation and manipulation of buffers using QRhiBuffer.
- Setting up a compute shader pipeline.
- Performing simple add between two buffers and reading back the results.

The correct results should be:
```
i: 0 value: 12
i: 1 value: 14
i: 2 value: 16
i: 3 value: 18
i: 4 value: 20
i: 5 value: 22
i: 6 value: 24
i: 7 value: 26
i: 8 value: 28
i: 9 value: 30
```

## Prerequisites

To run this example, you need:

- Qt 6.6 or later, with shadertool installed

## Know issues

- returns -1 for all elements on Metal on non-apple silicon, see https://bugreports.qt.io/browse/QTBUG-119447

## Building and Running

1. Clone or download this repository.
2. Open the project in Qt Creator or your preferred IDE.
3. Build the project using cmake.
4. Run the executable.

## Code Structure

The main function in the `main.cpp` file demonstrates the following:

- Setting up the QGuiApplication.
- Initializing the RHI with platform-specific parameters.
- Creating buffers and a compute pipeline.
- Loading and setting up the compute shader (a simple add).
- Dispatching the compute command and reading back the results.

## Notes

- This example assumes the presence of a compute shader in SPIR-V format. This example uses cmake and shadertools to convert GLSL computer into SPIR-V format.
- Error handling is minimal for brevity, but in a production environment, you should handle potential errors more robustly.
- This is a minimal example intended for educational purposes.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
