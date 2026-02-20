package com.example.doodle

import android.os.Bundle
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout
import androidx.activity.compose.setContent
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.ComposeView
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.google.androidgamesdk.GameActivity

//Match game activity states with screen states.
enum class ScreenState {
    START_MENU,
    PLAYING,
    GAME_OVER
}



class MainActivity : GameActivity() {

    // We use a mutable state defined outside onCreate so the JNI method can access the setter logic indirectly
    private val currentScore = mutableIntStateOf(0)
    private val currentScreen = mutableStateOf(ScreenState.START_MENU)


    companion object {
        init {
            System.loadLibrary("doodle")
        }
    }



    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        Log.i("DoodleUI", "MainActivity Created!") // Standard log
        if (hasFocus) {
            hideSystemUi()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 1. Create a ComposeView manually
        val composeView = ComposeView(this).apply {
            setContent {

                Box(modifier = Modifier.fillMaxSize()) {

                    // 3. Switch Content based on State
                    when (currentScreen.value) {
                        ScreenState.START_MENU -> StartMenuOverlay()
                        ScreenState.PLAYING -> InGameOverlay()
                        ScreenState.GAME_OVER -> GameOverOverlay()
                    }
                }

            }
        }

        //Create a layer above game activity surface
        // 2. Add the ComposeView on top of the existing GameActivity surface
        val params = FrameLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.MATCH_PARENT
        )
        addContentView(composeView, params)
    }


//    private fun restartGame() {
//        // Simple restart by recreating activity for now
//        finish()
//        startActivity(intent)
//    }


    //----Engine to UI functions----
    fun setScore(score: Int) {
        // C++ runs on a background thread. UI updates must happen on Main Thread.
        runOnUiThread {
            currentScore.intValue = score
        }
    }

    fun gameOver(finalScore: Int) {
        runOnUiThread {
            currentScore.intValue = finalScore
            currentScreen.value = ScreenState.GAME_OVER
        }
    }

    //---UI to Engine functions---

    external fun startGameNative()

    fun startGame() {
        currentScore.intValue = 0
        currentScreen.value = ScreenState.PLAYING
        // We reuse the native restart function to reset the C++ state
        startGameNative()
    }

    public fun backToMenu() {
        currentScreen.value = ScreenState.START_MENU
    }


    external fun restartGameNative()

    public fun restartGame() {
        // 2. Reset UI State
        currentScore.intValue = 0

        // 3. Call C++
        restartGameNative();
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

    @Composable
    fun StartMenuOverlay() {
        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF87CEEB)) // Sky blue opaque background to hide game
                .clickable(enabled = false) {},
            contentAlignment = Alignment.Center
        ) {
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Text(
                    text = "DEMON MAN JUMP",
                    fontSize = 40.sp,
                    fontWeight = FontWeight.Bold,
                    color = Color.White
                )
                Spacer(modifier = Modifier.height(32.dp))
                Button(
                    onClick = {  startGame() },
                    colors = ButtonDefaults.buttonColors(containerColor = Color(0xFF4CAF50))
                ) {
                    Text("PLAY GAME", fontSize = 20.sp, modifier = Modifier.padding(8.dp))
                }
            }
        }
    }

    @Composable
    fun InGameOverlay() {
        // Transparent overlay, just shows score
        Box(modifier = Modifier.fillMaxSize().padding(16.dp)) {
            Column(modifier = Modifier.align(Alignment.TopStart)) {
                Text(
                    text = "Score:",
                    color = Color.Black,
                    fontSize = 14.sp,
                    fontWeight = FontWeight.Bold
                )
                Text(
                    text = "${currentScore.intValue}",
                    color = Color.Black,
                    fontSize = 24.sp,
                    fontFamily = FontFamily.Monospace
                )
            }
        }
    }

    @Composable
    fun GameOverOverlay() {
        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xAA000000)) // Semi-transparent black
                .clickable(enabled = false) {}, // Block clicks
            contentAlignment = Alignment.Center
        ) {
            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                modifier = Modifier
                    .background(Color.White, shape = RoundedCornerShape(16.dp))
                    .padding(32.dp)
            ) {
                Text(
                    "GAME OVER",
                    color = Color.Red,
                    fontSize = 32.sp,
                    fontWeight = FontWeight.Bold
                )
                Spacer(modifier = Modifier.height(16.dp))
                Text("Final Score", color = Color.Black, fontSize = 18.sp)
                Text(
                    text = "${currentScore.intValue}",
                    color = Color.Black,
                    fontSize = 48.sp,
                    fontWeight = FontWeight.Bold
                )
                Spacer(modifier = Modifier.height(24.dp))

                // Restart Button
                Button(
                    onClick = { startGame() },
                    colors = ButtonDefaults.buttonColors(containerColor = Color.Blue)
                ) {
                    Text("Try Again")
                }

                Spacer(modifier = Modifier.height(8.dp))

                // Back to Menu Button
                Button(
                    onClick = { backToMenu() },
                    colors = ButtonDefaults.buttonColors(containerColor = Color.Gray)
                ) {
                    Text("Main Menu")
                }
            }
        }
    }
}





