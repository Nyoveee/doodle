package com.example.doodle

import android.view.Surface

object NativeBridge {
    init {
        // This loads the native library when this object is used for the first time.
        // The name must match the library name in your CMakeLists.txt.
        System.loadLibrary("doodle")
    }

    /**
     * Gets a pointer to the native C++ Engine instance.
     * @return A Long representing the memory address of the native Engine. Returns 0 if not initialized.
     */
    external fun nativeGetEnginePtr(): Long

    /**
     * Pass the drawing surface to the native engine.
     * @param surface The Surface to draw on.
     */
    external fun onSurfaceCreated(surface: Surface)

    /**
     * Inform the native engine that the drawing surface is destroyed.
     */
    external fun onSurfaceDestroyed()

    /**
     * Send a touch event to the native engine.
     * @param action The touch action (e.g., MotionEvent.ACTION_DOWN).
     * @param x The x-coordinate of the touch.
     * @param y The y-coordinate of the touch.
     */
    external fun onTouchEvent(action: Int, x: Float, y: Float)
}
