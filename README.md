# Native C++ Matrix Digital Rain Simulation

A high-performance, retro-inspired graphics rendering engine for Android. This project showcases the integration of low-level native C++ systems with the modern declarative UI capabilities of Jetpack Compose using the Android Native Development Kit (NDK) and Java Native Interface (JNI).

## 🛠️ Tech Stack & Engine Architecture

* **Native Core:** Written in pure C++ using standard libraries (`std::chrono`, arrays) to handle physics calculations and fading trail decays.
* **UI & Presentation:** Configured entirely with **Jetpack Compose**, running an active frame-rate locked animation rendering loop.
* **Bridge Interface:** Leverages **JNI (Java Native Interface)** to pass direct, zero-allocation memory references between Kotlin and C++.
* **Compilation Optimization:** Powered by **KSP (Kotlin Symbol Processing)** and CMake toolchains, bypassing old, slow Java compilation pipelines.

## 🚀 Performance & Memory Engineering

* **Zero-Allocation Pixel Blitting:** To eliminate runtime lag spikes caused by the Android Garbage Collector, the application allocates a single, fixed `IntArray` buffer on launch. The C++ engine mutates the raw pixel colors directly inside this existing RAM block.
* **Chunky Render Scaling:** To maintain a locked 60/120 FPS on high-density smartphone displays, the native loop updates a downscaled virtual matrix (e.g., 360x800). 
* **Nearest-Neighbor Upscaling:** The resulting pixel array is flushed into an Android Bitmap and drawn on a Compose `Canvas` using `FilterQuality.None`, stretching the image cleanly to preserve crisp, blocky arcade-style retro pixel aesthetics.
* **Delta-Time Motion:** Physics steps are managed via `std::chrono::steady_clock`, ensuring the digital stream drops at an identical velocity across all screen refresh rates.
