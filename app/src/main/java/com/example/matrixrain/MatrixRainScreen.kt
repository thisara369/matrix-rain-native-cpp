package com.example.matrixrain

import android.graphics.Bitmap
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.runtime.withFrameNanos
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.FilterQuality
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.unit.IntSize
import kotlinx.coroutines.isActive

@Composable
fun MatrixRainScreen(modifier: Modifier = Modifier) {
    var containerSize by remember { mutableStateOf(IntSize.Zero) }

    Box(
        modifier = modifier
            .fillMaxSize()
            .onSizeChanged { containerSize = it }
    ) {
        if (containerSize.width > 0 && containerSize.height > 0) {
            // Render scale: 3x downscaled buffer for maximum pixel drawing performance.
            // Stretches via nearest-neighbor filtering for sharp retro-blocky cyberpunk aesthetics.
            val scale = 3
            val bufferWidth = ((containerSize.width / scale) / 8).coerceAtLeast(1) * 8
            val bufferHeight = ((containerSize.height / scale) / 8).coerceAtLeast(1) * 8

            MatrixRainRenderer(
                bufferWidth = bufferWidth,
                bufferHeight = bufferHeight,
                parentWidth = containerSize.width,
                parentHeight = containerSize.height
            )
        }
    }
}

@Composable
private fun MatrixRainRenderer(
    bufferWidth: Int,
    bufferHeight: Int,
    parentWidth: Int,
    parentHeight: Int
) {
    // Recomposable state counter to trigger Canvas redraws at the screen's refresh rate
    var frameTrigger by remember { mutableIntStateOf(0) }

    // Retained Bitmap and pixel IntArray buffer across frames
    val bitmap = remember(bufferWidth, bufferHeight) {
        Bitmap.createBitmap(bufferWidth, bufferHeight, Bitmap.Config.ARGB_8888)
    }
    val pixels = remember(bufferWidth, bufferHeight) {
        IntArray(bufferWidth * bufferHeight)
    }

    // Active Choreographer frame loop
    LaunchedEffect(bufferWidth, bufferHeight) {
        while (isActive) {
            withFrameNanos { _ ->
                // Call native JNI engine to compute physics state and draw characters into pixels buffer
                MatrixNativeBridge.nativeUpdateAndRender(pixels, bufferWidth, bufferHeight)

                // Copy raw pixel array into the Android Bitmap
                bitmap.setPixels(pixels, 0, bufferWidth, 0, 0, bufferWidth, bufferHeight)

                // Increment state to force Compose Canvas to redraw the updated Bitmap
                frameTrigger++
            }
        }
    }

    Canvas(modifier = Modifier.fillMaxSize()) {
        // Read state inside drawing scope to ensure recomposition/redraw dependency
        val t = frameTrigger

        // Draw the low-resolution bitmap upscaled to fill the screen dimensions.
        // FilterQuality.None implements nearest-neighbor upscaling for clean pixel-art styling.
        drawImage(
            image = bitmap.asImageBitmap(),
            dstSize = IntSize(parentWidth, parentHeight),
            filterQuality = FilterQuality.None
        )
    }
}
