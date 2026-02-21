package com.example.doodle

import android.os.Bundle
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout
import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
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
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.lifecycle.lifecycleScope
import com.example.doodle.data.GameDatabase
import com.example.doodle.data.HighScoreDao
import com.example.doodle.data.HighScoreEntity
import com.google.androidgamesdk.GameActivity
import kotlinx.coroutines.flow.firstOrNull
import kotlinx.coroutines.launch
import java.util.UUID
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.OutlinedTextFieldDefaults
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.KeyboardCapitalization
import androidx.core.content.edit


//Match game activity states with screen states.
enum class ScreenState {
    START_MENU,
    PLAYING,
    GAME_OVER,
    HIGH_SCORES
}



class MainActivity : GameActivity() {

    private lateinit var dao: HighScoreDao
    private val topScores = mutableStateOf<List<HighScoreEntity>>(emptyList())

    private val showUsernameDialog = mutableStateOf(false)
    private val pendingScore = mutableIntStateOf(0) // holds score while dialog is open

    // We use a mutable state defined outside onCreate so the JNI method can access the setter logic indirectly
    private val currentScore = mutableIntStateOf(0)
    private val currentScreen = mutableStateOf(ScreenState.START_MENU)

    // Theme Colors derived from your screenshot
    private val themeRed = Color(0xFFD32F2F) // Red for platforms/buttons
    private val themeBlack = Color.Black
    private val themeWhite = Color.White

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

        dao = GameDatabase.getInstance(this).highScoreDao()

        lifecycleScope.launch {
            dao.getTopScores().collect { topScores.value = it }
        }

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
            pendingScore.intValue = finalScore

            if (getSavedUsername() == null) {
                showUsernameDialog.value = true
                currentScreen.value = ScreenState.GAME_OVER
            } else {
                currentScreen.value = ScreenState.GAME_OVER
                persistScore(finalScore, getSavedUsername()!!)
            }
        }
    }

    private fun persistScore(score: Int, username: String) {
        lifecycleScope.launch {
            dao.insert(
                HighScoreEntity(
                    userId = getOrCreateUserId(),
                    gameId = UUID.randomUUID().toString(),
                    username = username,
                    score = score,
                    achievedAt = System.currentTimeMillis()
                )
            )
        }
    }


    // Generates and persists a stable userId per device install
    private fun getOrCreateUserId(): String {
        val prefs = getSharedPreferences("game_prefs", MODE_PRIVATE)
        return prefs.getString("user_id", null) ?: UUID.randomUUID().toString().also { id ->
            prefs.edit { putString("user_id", id) }
        }
    }

    // Reads saved username, null if first time
    private fun getSavedUsername(): String? =
        getSharedPreferences("game_prefs", MODE_PRIVATE).getString("username", null)

    private fun saveUsername(name: String) {
        getSharedPreferences("game_prefs", MODE_PRIVATE).edit {
            putString("username", name)
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

    fun backToMenu() {
        currentScreen.value = ScreenState.START_MENU
    }


    private fun openHighScores() {
        currentScreen.value = ScreenState.HIGH_SCORES
    }

    external fun restartGameNative()

    fun restartGame() {
        // 2. Reset UI State
        currentScore.intValue = 0
        currentScreen.value = ScreenState.PLAYING
        // 3. Call C++
        restartGameNative()
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
                .background(themeBlack), // Match game background
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
                            color = themeRed,
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
                            border = BorderStroke(2.dp, themeRed),
                            shape = RoundedCornerShape(8.dp),
                            modifier = Modifier.width(220.dp).height(60.dp)
                        ) {
                            Text(
                                "QUIT",
                                fontSize = 20.sp,
                                color = themeRed,
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
            colors = ButtonDefaults.buttonColors(containerColor = themeRed), // Red platforms
            shape = RoundedCornerShape(8.dp), // Slightly blocky like platforms
            modifier = Modifier
                .width(220.dp)
                .height(60.dp)
        ) {
            Text(
                text = text,
                fontSize = 20.sp,
                fontWeight = FontWeight.Bold,
                color = themeWhite,
                fontFamily = FontFamily.Monospace
            )
        }
    }

    @Composable
    fun HighScoresOverlay() {
        val scores = topScores.value

        Box(modifier = Modifier.fillMaxSize().background(themeBlack)) {
            Column(modifier = Modifier.fillMaxSize(), horizontalAlignment = Alignment.CenterHorizontally) {

                Box(
                    modifier = Modifier.fillMaxWidth().background(themeRed).padding(vertical = 24.dp),
                    contentAlignment = Alignment.Center
                ) {
                    Text("HIGH SCORES", fontSize = 32.sp, fontWeight = FontWeight.Bold,
                        color = themeWhite, fontFamily = FontFamily.Monospace, letterSpacing = 4.sp)
                }

                Spacer(modifier = Modifier.height(16.dp))

                LazyColumn(
                    modifier = Modifier.weight(1f).fillMaxWidth().padding(horizontal = 32.dp),
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    if (scores.isEmpty()) {
                        item {
                            Text(
                                "No scores yet — get climbing!",
                                color = Color.Gray,
                                fontFamily = FontFamily.Monospace,
                                fontSize = 16.sp,
                                modifier = Modifier.padding(32.dp)
                            )
                        }
                    }

                    itemsIndexed(scores) { index, entry ->
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(vertical = 8.dp)
                                .background(Color(0xFF1A1A1A), RoundedCornerShape(4.dp))
                                .padding(horizontal = 16.dp, vertical = 12.dp),
                            horizontalArrangement = Arrangement.SpaceBetween,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Text("#${index + 1}", color = Color.Gray,
                                fontFamily = FontFamily.Monospace, fontSize = 14.sp)
                            Column {
                                Text(entry.username, color = Color.White,
                                    fontFamily = FontFamily.Monospace, fontSize = 18.sp)
                                Text(
                                    // Format timestamp as readable date
                                    java.text.SimpleDateFormat("dd MMM yyyy", java.util.Locale.getDefault())
                                        .format(java.util.Date(entry.achievedAt)),
                                    color = Color.Gray,
                                    fontFamily = FontFamily.Monospace,
                                    fontSize = 11.sp
                                )
                            }
                            Text("${entry.score}", color = themeRed,
                                fontFamily = FontFamily.Monospace,
                                fontWeight = FontWeight.Bold, fontSize = 18.sp)
                        }
                    }

                }

                Spacer(modifier = Modifier.height(16.dp))
                MenuButton(text = "CLOSE", onClick = { backToMenu() })
                Spacer(modifier = Modifier.height(32.dp))
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
    fun UsernameDialog(onConfirm: (String) -> Unit) {
        var text by remember { mutableStateOf("") }
        val isValid = text.trim().length in 2..16

        AlertDialog(
            onDismissRequest = { /* force them to enter a name, no dismiss */ },
            containerColor = Color(0xFF1A1A1A),
            title = {
                Text(
                    "Enter Your Name",
                    color = themeWhite,
                    fontFamily = FontFamily.Monospace,
                    fontWeight = FontWeight.Bold,
                    fontSize = 20.sp
                )
            },
            text = {
                Column {
                    Text(
                        "This will appear on the leaderboard.",
                        color = Color.Gray,
                        fontFamily = FontFamily.Monospace,
                        fontSize = 13.sp,
                        modifier = Modifier.padding(bottom = 12.dp)
                    )
                    OutlinedTextField(
                        value = text,
                        onValueChange = { if (it.length <= 16) text = it },
                        placeholder = { Text("e.g. DemonSlayer", color = Color.Gray) },
                        singleLine = true,
                        colors = OutlinedTextFieldDefaults.colors(
                            focusedBorderColor = themeRed,
                            unfocusedBorderColor = Color.Gray,
                            focusedTextColor = themeWhite,
                            unfocusedTextColor = themeWhite,
                            cursorColor = themeRed
                        ),
                        keyboardOptions = KeyboardOptions(
                            imeAction = ImeAction.Done,
                            capitalization = KeyboardCapitalization.Words
                        ),
                        keyboardActions = KeyboardActions(
                            onDone = { if (isValid) onConfirm(text.trim()) }
                        )
                    )
                    if (text.isNotEmpty() && !isValid) {
                        Text(
                            "Name must be 2–16 characters.",
                            color = themeRed,
                            fontSize = 11.sp,
                            fontFamily = FontFamily.Monospace,
                            modifier = Modifier.padding(top = 4.dp)
                        )
                    }
                }
            },
            confirmButton = {
                Button(
                    onClick = { if (isValid) onConfirm(text.trim()) },
                    enabled = isValid,
                    colors = ButtonDefaults.buttonColors(
                        containerColor = themeRed,
                        disabledContainerColor = Color(0xFF7A1A1A)
                    ),
                    shape = RoundedCornerShape(8.dp)
                ) {
                    Text("SAVE", fontFamily = FontFamily.Monospace, color = themeWhite)
                }
            }
        )
    }


    @Composable
    fun GameOverOverlay() {

        // Show username dialog on top if needed
        if (showUsernameDialog.value) {
            UsernameDialog(onConfirm = { name ->
                saveUsername(name)
                showUsernameDialog.value = false
                persistScore(pendingScore.intValue, name)
            })
        }

        Box(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xEE000000))
                .clickable(enabled = false) {},
            contentAlignment = Alignment.Center
        ) {
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Text("GAME OVER", fontSize = 48.sp, color = themeRed,
                    fontWeight = FontWeight.Bold, fontFamily = FontFamily.Monospace)
                Spacer(modifier = Modifier.height(16.dp))
                Text("FINAL SCORE", color = Color.Gray, fontSize = 16.sp, fontFamily = FontFamily.Monospace)
                Text("${currentScore.intValue}", color = themeWhite, fontSize = 60.sp,
                    fontWeight = FontWeight.Bold, fontFamily = FontFamily.Monospace)
                Spacer(modifier = Modifier.height(40.dp))
                MenuButton(text = "RETRY", onClick = { restartGame() })
                Spacer(modifier = Modifier.height(16.dp))
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





