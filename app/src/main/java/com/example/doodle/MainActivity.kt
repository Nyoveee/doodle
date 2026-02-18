package com.example.doodle

import android.os.Bundle
import android.view.MotionEvent
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.View
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.viewinterop.AndroidView

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        hideSystemUi()

        // NOTE: removed NativeBridge.init(assets) to avoid unresolved JNI dependency during debug
        setContent {
            GameSurface()
        }
    }

    override fun onResume() {
        super.onResume()
        // NOTE: removed NativeBridge.start() while debugging native renderer
    }

    override fun onPause() {
        // NOTE: removed NativeBridge.stop()
        super.onPause()
    }

    private fun hideSystemUi() {
        val decorView = window.decorView
        decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_FULLSCREEN)
    }
}

@Composable
fun GameSurface() {
    Box(modifier = Modifier.fillMaxSize()) {
        AndroidView(
            modifier = Modifier.fillMaxSize(),
            factory = { context ->
                object : SurfaceView(context) {
                    override fun performClick(): Boolean {
                        // Call through to super for accessibility events
                        return super.performClick()
                    }
                }.apply {
                    // Make the view clickable so `performClick()` is meaningful for accessibility
                    isClickable = true
                    holder.addCallback(object : SurfaceHolder.Callback {
                        override fun surfaceCreated(holder: SurfaceHolder) {
                            NativeBridge.onSurfaceCreated(holder.surface)
                        }

                        override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
                            // not implemented
                        }

                        override fun surfaceDestroyed(holder: SurfaceHolder) {
                            NativeBridge.onSurfaceDestroyed()
                        }
                    })
                    setOnTouchListener { v, event ->
                        // Call performClick for accessibility when a click/up occurs
                        if (event.action == MotionEvent.ACTION_UP) v.performClick()
                        NativeBridge.onTouchEvent(event.action, event.x, event.y)
                        true
                    }
                }
            }
        )

        // Simple Compose overlay so we can see something while native rendering is debugged
        Canvas(modifier = Modifier.fillMaxSize()) {
            // Semi-transparent red rectangle overlay
            drawRect(color = Color(0x66FF0000))
            // White debug circle in the top-left
            drawCircle(color = Color.White, radius = 80f, center = androidx.compose.ui.geometry.Offset(120f, 120f))
        }
    }
}