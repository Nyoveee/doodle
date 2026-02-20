package com.example.doodle

import android.os.Bundle
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout
import androidx.activity.compose.setContent
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
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
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.remember
import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.tween
import androidx.compose.animation.fadeIn
import androidx.compose.animation.slideInVertically
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.compose.runtime.remember
import com.google.androidgamesdk.GameActivity
import kotlin.times


//Match game activity states with screen states.
enum class ScreenState {
    START_MENU,
    PLAYING,
    GAME_OVER,
    HIGH_SCORES
}



class MainActivity : GameActivity() {

    // We use a mutable state defined outside onCreate so the JNI method can access the setter logic indirectly
    private val currentScore = mutableIntStateOf(0)
    private val currentScreen = mutableStateOf(ScreenState.START_MENU)

    // Theme Colors derived from your screenshot
    private val ThemeRed = Color(0xFFD32F2F) // Red for platforms/buttons
    private val ThemeBlack = Color.Black
    private val ThemeWhite = Color.White

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
                        ScreenState.HIGH_SCORES -> HighScoresOverlay()
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


    private fun openHighScores() {
        currentScreen.value = ScreenState.HIGH_SCORES
    }

    external fun restartGameNative()

    public fun restartGame() {
        // 2. Reset UI State
        currentScore.intValue = 0
        currentScreen.value = ScreenState.PLAYING
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

        var isVisible by remember { mutableStateOf(false) }

        // Trigger the animation immediately when this Composable enters the screen
        LaunchedEffect(Unit) {
            isVisible = true
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(ThemeBlack), // Match game background
            contentAlignment = Alignment.Center
        ) {
            Column(horizontalAlignment = Alignment.CenterHorizontally) {

                // Title Animation: Slides down from top
                AnimatedVisibility(
                    visible = isVisible,
                    enter = slideInVertically(
                        initialOffsetY = { -100 }, // Start 100 pixels above
                        animationSpec = tween(durationMillis = 800)
                    ) + fadeIn(animationSpec = tween(durationMillis = 800))
                ) {
                    Column(horizontalAlignment = Alignment.CenterHorizontally) {
                        // Title
                        Text(
                            text = "Demon Jump", // Or "DOODLE JUMP"
                            color = ThemeRed,
                            fontSize = 50.sp,
                            fontWeight = FontWeight.Bold,
                            fontFamily = FontFamily.Monospace,
                            modifier = Modifier.padding(bottom = 8.dp)
                        )

                        Text(
                            text = "SURVIVE THE CLIMB",
                            color = Color.Gray,
                            fontSize = 16.sp,
                            fontFamily = FontFamily.Monospace,
                            modifier = Modifier.padding(bottom = 60.dp)
                        )
                    }
                }



                AnimatedVisibility(
                    visible = isVisible,
                    enter = slideInVertically(
                        initialOffsetY = { 200 }, // Start 200 pixels below
                        animationSpec = tween(durationMillis = 800, delayMillis = 300) // 300ms delay for potential stagger effect
                    ) + fadeIn(animationSpec = tween(durationMillis = 800, delayMillis = 300))
                ) {
                    Column(horizontalAlignment = Alignment.CenterHorizontally) {
                        // 1. Play Button
                        MenuButton(text = "PLAY", onClick = { startGame() })

                        Spacer(modifier = Modifier.height(20.dp))

                        // 2. High Scores Button
                        MenuButton(text = "HIGH SCORES", onClick = { openHighScores() })

                        Spacer(modifier = Modifier.height(20.dp))

                        // 3. Quit Button
                        // Uses a hollow style to distinguish it slightly, or same style
                        Button(
                            onClick = { finish() },
                            colors = ButtonDefaults.buttonColors(containerColor = Color.Transparent),
                            border = BorderStroke(2.dp, ThemeRed),
                            shape = RoundedCornerShape(8.dp),
                            modifier = Modifier.width(220.dp).height(60.dp)
                        ) {
                            Text(
                                "QUIT",
                                fontSize = 20.sp,
                                color = ThemeRed,
                                fontFamily = FontFamily.Monospace
                            )
                        }
                    }
                }
            }
        }
    }

    @Composable
    fun MenuButton(text: String, onClick: () -> Unit) {
        Button(
            onClick = onClick,
            colors = ButtonDefaults.buttonColors(containerColor = ThemeRed), // Red platforms
            shape = RoundedCornerShape(8.dp), // Slightly blocky like platforms
            modifier = Modifier
                .width(220.dp)
                .height(60.dp)
        ) {
            Text(
                text = text,
                fontSize = 20.sp,
                fontWeight = FontWeight.Bold,
                color = ThemeWhite,
                fontFamily = FontFamily.Monospace
            )
        }
    }

    @Composable
    fun HighScoresOverlay() {
        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(ThemeBlack) // Full screen black background
        ) {
            Column(
                modifier = Modifier.fillMaxSize(),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                // 1. Large Top Bar
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .background(ThemeRed) // Red header bar
                        .padding(vertical = 24.dp),
                    contentAlignment = Alignment.Center
                ) {
                    Text(
                        text = "HIGH SCORES",
                        fontSize = 32.sp,
                        fontWeight = FontWeight.Bold,
                        color = ThemeWhite,
                        fontFamily = FontFamily.Monospace,
                        letterSpacing = 4.sp
                    )
                }

                Spacer(modifier = Modifier.height(16.dp))

                // 2. Scrollable Lazy Window
                // weights (1f) ensure it takes up all available space between header and footer
                LazyColumn(
                    modifier = Modifier
                        .weight(1f)
                        .fillMaxWidth()
                        .padding(horizontal = 32.dp),
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    // Dummy data for display purposes
                    items(20) { index ->
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(vertical = 12.dp)
                                .background(Color(0xFF1A1A1A), RoundedCornerShape(4.dp)) // Dark gray item bg
                                .padding(16.dp),
                            horizontalArrangement = Arrangement.SpaceBetween
                        ) {
                            Text(
                                text = "Player ${index + 1}",
                                color = Color.White,
                                fontFamily = FontFamily.Monospace,
                                fontSize = 18.sp
                            )
                            Text(
                                text = "${(20 - index) * 100}", // Descending dummy scores
                                color = ThemeRed,
                                fontFamily = FontFamily.Monospace,
                                fontWeight = FontWeight.Bold,
                                fontSize = 18.sp
                            )
                        }
                    }
                }

                Spacer(modifier = Modifier.height(16.dp))

                // 3. Close Button at the bottom
                MenuButton(text = "CLOSE", onClick = { backToMenu() })

                Spacer(modifier = Modifier.height(32.dp)) // Bottom padding
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
                    color = Color.White,
                    fontSize = 14.sp,
                    fontWeight = FontWeight.Bold
                )
                Text(
                    text = "${currentScore.intValue}",
                    color = Color.White,
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
                .background(Color(0xEE000000)) // Mostly opaque black
                .clickable(enabled = false) {},
            contentAlignment = Alignment.Center
        ) {
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Text(
                    text = "GAME OVER",
                    fontSize = 48.sp,
                    color = ThemeRed,
                    fontWeight = FontWeight.Bold,
                    fontFamily = FontFamily.Monospace
                )

                Spacer(modifier = Modifier.height(16.dp))

                Text("FINAL SCORE", color = Color.Gray, fontSize = 16.sp, fontFamily = FontFamily.Monospace)
                Text(
                    text = "${currentScore.intValue}",
                    color = ThemeWhite,
                    fontSize = 60.sp,
                    fontWeight = FontWeight.Bold,
                    fontFamily = FontFamily.Monospace
                )

                Spacer(modifier = Modifier.height(40.dp))

                MenuButton(text = "RETRY", onClick = { restartGame() })

                Spacer(modifier = Modifier.height(16.dp))

                // Hollow Style for "Menu"
                Button(
                    onClick = { backToMenu() },
                    colors = ButtonDefaults.buttonColors(containerColor = Color.Transparent),
                    border = BorderStroke(2.dp, Color.Gray),
                    shape = RoundedCornerShape(8.dp),
                    modifier = Modifier.width(220.dp).height(50.dp)
                ) {
                    Text("MAIN MENU", fontSize = 18.sp, color = Color.Gray, fontFamily = FontFamily.Monospace)
                }
            }
        }
    }
}





