package com.example.doodle.data

import androidx.room.Entity
import androidx.room.Index
import androidx.room.PrimaryKey

@Entity(
    tableName = "high_scores",
    indices = [Index(value = ["highScore"])] // Speeds up ORDER BY highScore DESC
)
data class HighScoreEntity(
    @PrimaryKey val userId: String,       // Internal: persisted UUID per device install
    val gameId: String,                   // Internal: session UUID that set the record
    val username: String,                 // Display name
    val highScore: Int,                   // Best score achieved
    val gamesPlayed: Int = 0,             // Total games ever played
    val achievedAt: Long = System.currentTimeMillis(),   // When HS was set
    val lastPlayedAt: Long = System.currentTimeMillis()  // Last session time
)
