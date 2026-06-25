package com.example.matrixrain

object MatrixNativeBridge {
    // This tells Kotlin that the actual code for this function is written in C++
    external fun nativeUpdateAndRender(pixels: IntArray, width: Int, height: Int)
}
